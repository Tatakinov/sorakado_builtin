#include "sorakado/socket.h"

#include <cassert>

#include "logger.h"

namespace {
#ifndef IS_WINDOWS
    inline int closesocket(int fd) {
        return close(fd);
    }
    constexpr int SOCKET_ERROR = -1;
#endif // IS_WINDOWS
}

namespace sorakado {
    lib_skeleton::sstp::Request buildRequest(const SorakadoType type, const std::string &uuid, const directsstp::Request &req) {
        lib_skeleton::sstp::Request ret = {req.method};
        ret["Charset"] = "UTF-8";
        switch (type) {
            case SorakadoType::Ao:
                ret["Ao"] = uuid;
                break;
            case SorakadoType::Ai:
                ret["Ai"] = uuid;
                break;
            default:
                assert(false);
                break;
        }
        ret["Sender"] = "Sorakado_builtin";
        if (req.hide_on_204) {
            ret["Option"] = "nodescript,hideon204";
        }
        else {
            ret["Option"] = "nodescript";
        }
        if (req.method == "EXECUTE") {
            ret["Command"] = req.command;
        }
        else if (req.method == "NOTIFY") {
            ret["Event"] = req.command;
        }
        else if (req.method == "SEND") {
            ret["Script"] = req.script;
        }
        for (int i = 0; i < req.args.size(); i++) {
            ret(i) = req.args[i];
        }
        return ret;
    }

    Socket::Socket(const std::string &path) : state_(SocketState::End), res_({500, "Internal Server Error"}) {
        sockaddr_un addr = {};
        if (path.length() >= sizeof(addr.sun_path)) {
            Logger::log("socket", "too long path");
            return;
        }
        soc_ = ::socket(AF_UNIX, SOCK_STREAM, 0);
        if (soc_ == INVALID_SOCKET) {
            Logger::log("socket", "socket() failed");
            return;
        }
#if defined(IS_WINDOWS)
        u_long iMode = 1; // non-zero for non-blocking IO
        if (ioctlsocket(soc_, FIONBIO, &iMode) != NO_ERROR) {
            Logger::log("socket", "ioctlsocket() failed");
            closesocket(soc_);
            soc_ = INVALID_SOCKET;
            return;
        }
#elif defined(IS__NIX)
        int flags = fcntl(soc_, F_GETFL, 0);
        if (flags == -1) {
            Logger::log("socket", "fcntl() failed");
            closesocket(soc_);
            soc_ = INVALID_SOCKET;
            return;
        }
        if (fcntl(soc_, F_SETFL, flags | O_NONBLOCK) == -1) {
            Logger::log("socket", "fcntl() failed");
            closesocket(soc_);
            soc_ = INVALID_SOCKET;
            return;
        }
#endif // OS
        addr.sun_family = AF_UNIX;
        // null-terminatedも書き込ませる
        strncpy(addr.sun_path, path.c_str(), path.length() + 1);
        if (connect(soc_, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr)) == -1) {
            Logger::log("socket", "connect() failed");
            closesocket(soc_);
            soc_ = INVALID_SOCKET;
            return;
        }
        state_ = SocketState::Idle;
    }

    Socket::~Socket() {
        if (soc_ != INVALID_SOCKET) {
            closesocket(soc_);
            soc_ = INVALID_SOCKET;
        }
    }

    void Socket::enqueue(const std::queue<lib_skeleton::sstp::Request> &queue) {
        if (state_ == SocketState::Idle) {
            queue_ = queue;
            next();
        }
    }

    void Socket::next() {
        if (queue_.size() == 0) {
            Logger::log("socket", "no item exists");
            state_ = SocketState::End;
            return;
        }
        auto req = queue_.front();
        req["Connection"] = "keep-alive";
        remain_ = req;
        state_ = SocketState::Write;
    }

    void Socket::recv() {
        if (state_ != SocketState::Read) {
            return;
        }
        char buf[BUFFER_SIZE] = {};
        auto len = ::recv(soc_, buf, BUFFER_SIZE, 0);
        if (len <= 0) {
            Logger::log("socket", ((len == 0) ? "connection closed" : "recv() failed"));
            closesocket(soc_);
            soc_ = INVALID_SOCKET;
            state_ = SocketState::End;
            return;
        }
        buffer_.append(buf, len);
        if (buffer_.ends_with("\r\n\r\n")) {
            auto res = lib_skeleton::sstp::Response::parse(buffer_);
            if (res.getStatusCode() == 200 && res.getContent().empty()) {
                return;
            }
            buffer_.clear();
            if (res.getStatusCode() == 204 && queue_.size() >= 2) {
                queue_.pop();
                next();
            }
            else {
                res_ = res;
                if (res["Connection"] && res["Connection"].value() == "keep-alive") {
                    state_ = SocketState::Idle;
                }
                else {
                    state_ = SocketState::End;
                }
            }
        }
    }

    void Socket::send() {
        if (state_ != SocketState::Write) {
            return;
        }
        auto len = ::send(soc_, remain_.data(), remain_.size(), 0);
        if (len < 0) {
            Logger::log("socket", "send() failed");
            closesocket(soc_);
            soc_ = INVALID_SOCKET;
            state_ = SocketState::End;
            return;
        }
        else if (len == remain_.size()) {
            state_ = SocketState::Read;
            return;
        }
        remain_ = remain_.substr(len);
    }
}
