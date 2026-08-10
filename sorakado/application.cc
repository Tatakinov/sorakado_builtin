#include "sorakado/application.h"
#include "os_preprocess.h"

#include <cassert>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>

#if defined(IS_WINDOWS)
#include <fcntl.h>
#include <io.h>
#include <ws2tcpip.h>
#include <afunix.h>
#include <winsock.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef max
#undef min
#else
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#endif // IS_WINDOWS

#include "logger.h"
#include "sorakado/misc.h"
#include "sorakado/socket.h"
#include "lib_skeleton/sstp.h"
#include "sorakado/sorakado.h"

namespace sorakado {
    Application::Application(int argc, char *argv[]) : type_(SorakadoType::Unknown), alive_(true), loaded_(false), is_idle_(true), is_debug_(false) {
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "--debug") {
                is_debug_ = true;
            }
        }
#ifdef IS_WINDOWS
        WSADATA wsa;
        WSAStartup(MAKEWORD(2, 2), &wsa);
        _setmode(_fileno(stdin), _O_BINARY);
        _setmode(_fileno(stdout), _O_BINARY);
#endif // Windows

        th_recv_ = std::make_unique<std::thread>([&]() {
            receiveSorakado();
        });

#if !defined(DEBUG)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cond_.wait(lock, [&] { return loaded_; });
        }
#endif // DEBUG

        th_send_ = std::make_unique<std::thread>([&]() {
            asyncSendDirectSSTP();
        });
    }

    Application::~Application() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            alive_ = false;
        }
        th_send_->join();
        th_recv_->join();
#ifdef IS_WINDOWS
        WSACleanup();
#endif // Windows
    }

    void Application::receiveSorakado() {
        uint32_t len;
        while (true) {
            std::cin.read(reinterpret_cast<char *>(&len), sizeof(uint32_t));
            if (std::cin.eof() || len == 0) {
                break;
            }
            std::string request;
            request.resize(len);
            std::cin.read(request.data(), len);
            if (std::cin.gcount() < len) {
                break;
            }
            auto req = lib_skeleton::sorakado::Request::parse(request);
            //Logger::log(request);
            auto event = req().value();

            lib_skeleton::sorakado::Response res {204, "No Content"};

            if (event == "Initialize" && req(0) && req(1)) {
                std::string tmp;
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    tmp = req(0).value();
                }
                std::u8string u8dir(tmp.begin(), tmp.end());
                std::filesystem::path dir = u8dir;
                std::string type = req(1).value();
                if (type == "AO") {
                    type_ = SorakadoType::Ao;
                    if (is_debug_) {
                        Logger::configure("ao.log");
                    }
                }
                else if (type == "AI") {
                    type_ = SorakadoType::Ai;
                    if (is_debug_) {
                        Logger::configure("ai.log");
                    }
                }
                else {
                    break;
                }
                sorakado_instance_ = SorakadoFactory::create(type_, this, dir);
            }
            else if (event == "Endpoint" && req(0) && req(1)) {
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    path_ = req(0).value();
                    uuid_ = req(1).value();
                }
                loaded_ = true;
                cond_.notify_one();
            }
            else {
                auto r = sorakado_instance_->sorakadoEventImmediately(req);
                if (r) {
                    res = r.value();
                }
                else {
                    std::vector<std::string> args;
                    args.reserve(std::count(request.begin(), request.end(), '\x0a'));
                    args.push_back(event);
                    for (int i = 0; ; i++) {
                        auto v = req(i);
                        if (v) {
                            args.push_back(v.value());
                        }
                        else {
                            break;
                        }
                    }
                    std::unique_lock<std::mutex> lock(mutex_);
                    queue_.push(args);
                }
            }

            res["Charset"] = "UTF-8";

            std::string response = res;
            //Logger::log(response);
            len = response.size();
            std::cout.write(reinterpret_cast<char *>(&len), sizeof(uint32_t));
            std::cout.write(response.c_str(), len);
        }
        {
            std::unique_lock<std::mutex> lock(mutex_);
            loaded_ = true;
            alive_ = false;
            event_queue_.push({{"", "", {}}});
        }
        cond_.notify_one();
    }

    void Application::asyncSendDirectSSTP() {
        std::unordered_map<SOCKET, std::unique_ptr<Socket>> map;
        std::queue<std::vector<directsstp::Request>> queue;
        std::queue<std::vector<directsstp::Request>> remain_queue;
        std::vector<directsstp::RequestCache> cache;
        while (true) {
            int running = 0;
            for (auto &[_, v] : map) {
                if (v->state() == SocketState::Idle) {
                    continue;
                }
                running++;
            }
            Logger::log("application", running, remain_queue.size());
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cond_.wait(lock, [&] { return !event_queue_.empty() || !alive_ || running > 0 || !remain_queue.empty(); });
                if (!alive_) {
                    break;
                }
                while (!event_queue_.empty()) {
                    auto list = event_queue_.front();
                    if (list.size() > 0) {
                        remain_queue.push(list);
                    }
                    event_queue_.pop();
                }
            }
            std::swap(queue, remain_queue);
            while (!queue.empty()) {
                // reuse socket
                bool found = false;
                for (auto &[_, v] : map) {
                    if (v->state() != SocketState::Idle) {
                        continue;
                    }
                    auto list = queue.front();
                    std::queue<lib_skeleton::sstp::Request> q;
                    for (auto &request : list) {
                        q.push(buildRequest(type_, uuid_, request));
                    }
                    queue.pop();
                    v->enqueue(q);
                    found = true;
                    break;
                }
                if (found) {
                    continue;
                }
                if (map.size() >= POOL_SIZE) {
                    break;
                }
                auto list = queue.front();
                std::queue<lib_skeleton::sstp::Request> q;
                for (auto &request : list) {
                    q.push(buildRequest(type_, uuid_, request));
                }
                queue.pop();
                auto socket = std::make_unique<Socket>(path_);
                auto s = socket->socket();
                if (s == INVALID_SOCKET) {
                    break;
                }
                socket->enqueue(q);
                map.try_emplace(s, std::move(socket));
            }
            while (!queue.empty()) {
                remain_queue.push(queue.front());
                queue.pop();
            }
            SOCKET max = INVALID_SOCKET;
            std::unordered_set<SOCKET> r_set, w_set;
            for (auto &[k, v] : map) {
                switch (v->state()) {
                    case SocketState::Read:
                        if (max == INVALID_SOCKET || max < k) {
                            max = k;
                        }
                        r_set.emplace(k);
                        break;
                    case SocketState::Write:
                        if (max == INVALID_SOCKET || max < k) {
                            max = k;
                        }
                        w_set.emplace(k);
                        break;
                    default:
                        break;
                }
            }
            // initialize
            fd_set rfds, wfds;
            FD_ZERO(&rfds);
            for (auto fd : r_set) {
                FD_SET(fd, &rfds);
            }
            FD_ZERO(&wfds);
            for (auto fd : w_set) {
                FD_SET(fd, &wfds);
            }
            timeval timeout = {0, 1000}; // 1ms
            select(max + 1, &rfds, &wfds, nullptr, &timeout);
            // read
            for (auto fd : r_set) {
                if (!FD_ISSET(fd, &rfds)) {
                    continue;
                }
                map[fd]->recv();
                if (map[fd]->state() == SocketState::End) {
                    map.erase(fd);
                }
            }
            // write
            for (auto fd : w_set) {
                if (!FD_ISSET(fd, &wfds)) {
                    continue;
                }
                map[fd]->send();
            }
        }
    }

    void Application::run() {
        SDL_Event event;
        std::vector<std::string> filelist;
        while ((is_idle_) ? (SDL_WaitEventTimeout(&event, 10)) : (SDL_PollEvent(&event))) {
            is_idle_ = false;
            switch (event.type) {
                case SDL_EVENT_WINDOW_MOVED:
                    Logger::log("window.moved", event.window.windowID, event.window.data1, event.window.data2);
                    break;
                case SDL_EVENT_QUIT:
                    alive_ = false;
                    return;
                case SDL_EVENT_DISPLAY_ADDED:
                    sorakado_instance_->display(event.display.displayID, true);
                    break;
                case SDL_EVENT_DISPLAY_REMOVED:
                    sorakado_instance_->display(event.display.displayID, false);
                    break;
                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP:
                    sorakado_instance_->key(event.key.windowID, event.key.key, event.key.down);
                    break;
                case SDL_EVENT_TEXT_INPUT:
                    sorakado_instance_->input(event.text.windowID, event.text.text);
                    break;
                case SDL_EVENT_TEXT_EDITING:
                    sorakado_instance_->edit(event.edit.windowID, event.edit.text);
                    break;
                case SDL_EVENT_MOUSE_MOTION:
                    sorakado_instance_->motion(event.motion.windowID, event.motion.x, event.motion.y);
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    sorakado_instance_->button(event.button.windowID, event.button.x, event.button.y, event.button.button, event.button.down, event.button.clicks);
                    break;
                case SDL_EVENT_MOUSE_WHEEL:
                    sorakado_instance_->wheel(event.wheel.windowID, event.wheel.x, event.wheel.y);
                    break;
                case SDL_EVENT_DROP_COMPLETE:
                    sorakado_instance_->drop(event.drop.windowID, filelist);
                    break;
                case SDL_EVENT_DROP_FILE:
                    filelist.push_back(event.drop.data);
                    break;
                case SDL_EVENT_WINDOW_FOCUS_GAINED:
                    sorakado_instance_->focus(event.window.windowID, true);
                    break;
                case SDL_EVENT_WINDOW_FOCUS_LOST:
                    sorakado_instance_->focus(event.window.windowID, false);
                    break;
                case SDL_EVENT_WINDOW_MAXIMIZED:
                    Logger::log("maximized");
                    sorakado_instance_->maximized(event.window.windowID);
                    break;
                default:
                    break;
            }
        }
        sorakado_instance_->run();
        {
            std::string err(SDL_GetError());
            if (!err.empty()) {
                Logger::log("Error", err);
                assert(false);
            }
        }
        std::queue<std::vector<std::string>> queue;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            while (!queue_.empty()) {
                queue.push(queue_.front());
                queue_.pop();
            }
        }
        while (!queue.empty()) {
            std::vector<std::string> args = queue.front();
            queue.pop();
            sorakado_instance_->sorakadoEvent(args);
        }
        is_idle_ = !sorakado_instance_->draw();
    }

    lib_skeleton::sstp::Response Application::sendDirectSSTP(const directsstp::Request req) {
        std::queue<lib_skeleton::sstp::Request> queue;
        queue.push(buildRequest(type_, uuid_, req));
        auto socket = std::make_unique<Socket>(path_);
        if (socket->socket() == INVALID_SOCKET) {
            return socket->response();
        }
        socket->enqueue(queue);
        Logger::log("application", "blocking", "begin", socket->socket());
        fd_set fds;
        while (socket->state() == SocketState::Write) {
            FD_ZERO(&fds);
            FD_SET(socket->socket(), &fds);
            select(socket->socket() + 1, nullptr, &fds, nullptr, nullptr);
            if (!FD_ISSET(socket->socket(), &fds)) {
                continue;
            }
            socket->send();
        }
        while (socket->state() == SocketState::Read) {
            FD_ZERO(&fds);
            FD_SET(socket->socket(), &fds);
            select(socket->socket() + 1, &fds, nullptr, nullptr, nullptr);
            if (!FD_ISSET(socket->socket(), &fds)) {
                continue;
            }
            socket->recv();
        }
        Logger::log("application", "blocking", "end");
        return socket->response();
    }

    void Application::enqueueDirectSSTP(std::vector<directsstp::Request> list) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            event_queue_.push(list);
        }
        cond_.notify_one();
    }
}
