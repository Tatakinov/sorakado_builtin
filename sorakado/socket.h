#ifndef SORAKADO_SOCKET_H_
#define SORAKADO_SOCKET_H_

#include <string>
#include <queue>

#include "os_preprocess.h"

#if defined(IS_WINDOWS)
#include <fcntl.h>
#include <io.h>
#include <ws2tcpip.h>
#include <afunix.h>
#include <winsock.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

using SOCKET = int;
constexpr SOCKET INVALID_SOCKET = -1;
#endif // IS_WINDOWS

#include "lib_skeleton/sstp.h"
#include "sorakado/misc.h"

namespace sorakado {
    constexpr size_t BUFFER_SIZE = 1024;
    constexpr size_t POOL_SIZE = 10;

    enum class SocketState {
        Idle, Read, Write, End,
    };

    lib_skeleton::sstp::Request buildRequest(const SorakadoType type, const std::string &uuid, const directsstp::Request &req);

    class Socket {
        private:
            SocketState state_;
            lib_skeleton::sstp::Response res_;
            SOCKET soc_;
            std::queue<lib_skeleton::sstp::Request> queue_;
            std::string remain_, buffer_;

            void next();
        public:
            Socket(const std::string &path);
            ~Socket();
            SOCKET socket() const {
                return soc_;
            }
            lib_skeleton::sstp::Response response() const {
                return res_;
            }
            SocketState state() const {
                return state_;
            }
            void enqueue(const std::queue<lib_skeleton::sstp::Request> &queue);
            void recv();
            void send();
    };
}

#endif // SORAKADO_SOCKET_H_
