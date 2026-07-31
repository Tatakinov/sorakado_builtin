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

#if defined(_WIN32) || defined(WIN32)
#include <fcntl.h>
#include <io.h>
#include <ws2tcpip.h>
#include <afunix.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef max
#undef min
#else
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#endif // WIN32

#include "logger.h"
#include "sorakado/misc.h"
#include "lib_skeleton/sstp.h"
#include "sorakado/sorakado.h"

#include <chrono>

namespace {
#ifndef IS_WINDOWS
    inline int closesocket(int fd) {
        return close(fd);
    }
    const auto SD_SEND = SHUT_WR;
#endif
}

namespace sorakado {
    Application::Application() : type_(SorakadoType::Unknown), alive_(true), loaded_(false), is_idle_(true) {
#ifdef IS_WINDOWS
        WSADATA wsa;
        WSAStartup(MAKEWORD(2, 2), &wsa);
        _setmode(_fileno(stdin), _O_BINARY);
        _setmode(_fileno(stdout), _O_BINARY);
#endif // Windows

        th_recv_ = std::make_unique<std::thread>([&]() {
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
                        //Logger::configure("ao.log");
                    }
                    else if (type == "AI") {
                        type_ = SorakadoType::Ai;
                        //Logger::configure("ai.log");
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
        });

#if !defined(DEBUG)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cond_.wait(lock, [&] { return loaded_; });
        }
#endif // DEBUG

        th_send_ = std::make_unique<std::thread>([&]() {
            while (true) {
                std::vector<directsstp::Request> list;
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    cond_.wait(lock, [this] { return !event_queue_.empty() || !alive_; });
                    if (!alive_) {
                        break;
                    }
                    list = event_queue_.front();
                    event_queue_.pop();
                }
                for (auto &request : list) {
                    auto res = lib_skeleton::sstp::Response::parse(sendDirectSSTP(request.method, request.command, request.args, request.script, request.hide_on_204));
                    // not fallback unless 204
                    if (res.getStatusCode() != 204) {
                        break;
                    }
                }
            }
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

    void Application::run() {
        SDL_Event event;
        std::vector<std::string> filelist;
auto a = std::chrono::system_clock::now();
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
auto b = std::chrono::system_clock::now();
//Logger::log("chrono", std::chrono::duration_cast<std::chrono::microseconds>(b - a));
a = b;
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
b = std::chrono::system_clock::now();
//Logger::log("chrono2", std::chrono::duration_cast<std::chrono::microseconds>(b - a));
a = b;
        is_idle_ = !sorakado_instance_->draw();
b = std::chrono::system_clock::now();
//Logger::log("chrono3", std::chrono::duration_cast<std::chrono::microseconds>(b - a));
a = b;
    }

    std::string Application::sendDirectSSTP(std::string method, std::string command, std::vector<std::string> args, std::string script, bool hide_on_204) {
        lib_skeleton::sstp::Request req {method};
        lib_skeleton::sstp::Response res {500, "Internal Server Error"};
        req["Charset"] = "UTF-8";
        switch (type_) {
            case SorakadoType::Ao:
                req["Ao"] = uuid_;
                break;
            case SorakadoType::Ai:
                req["Ai"] = uuid_;
                break;
            default:
                assert(false);
                break;
        }
        if (path_.empty()) {
            return res;
        }
        req["Sender"] = "Sorakado_builtin";
        if (hide_on_204) {
            req["Option"] = "nodescript,hideon204";
        }
        else {
            req["Option"] = "nodescript";
        }
        if (req.getCommand() == "EXECUTE") {
            req["Command"] = command;
        }
        else if (req.getCommand() == "NOTIFY") {
            req["Event"] = command;
        }
        else if (req.getCommand() == "SEND") {
            req["Script"] = script;
        }
        for (int i = 0; i < args.size(); i++) {
            req(i) = args[i];
        }
        //Logger::log(static_cast<std::string>(req));
        sockaddr_un addr;
        if (path_.length() >= sizeof(addr.sun_path)) {
            return res;
        }
        int soc = socket(AF_UNIX, SOCK_STREAM, 0);
        if (soc == -1) {
            return res;
        }
        memset(&addr, 0, sizeof(sockaddr_un));
        addr.sun_family = AF_UNIX;
        // null-terminatedも書き込ませる
        strncpy(addr.sun_path, path_.c_str(), path_.length() + 1);
        if (connect(soc, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr)) == -1) {
            return res;
        }
        std::string request = req;
        if (send(soc, request.c_str(), request.size(), 0) != request.size()) {
            closesocket(soc);
            return res;
        }
        shutdown(soc, SD_SEND);
        char buffer[BUFFER_SIZE] = {};
        std::string data;
        while (true) {
            int ret = recv(soc, buffer, BUFFER_SIZE, 0);
            if (ret == -1) {
                closesocket(soc);
                return res;
            }
            if (ret == 0) {
                closesocket(soc);
                break;
            }
            data.append(buffer, ret);
        }
        return data;
    }

    void Application::enqueueDirectSSTP(std::vector<directsstp::Request> list) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            event_queue_.push(list);
        }
        cond_.notify_one();
    }
}
