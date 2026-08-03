#ifndef SORAKADO_APPLICATION_H_
#define SORAKADO_APPLICATION_H_

#include <condition_variable>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "sorakado/misc.h"
#include "sorakado/compatible.h"
#include "sorakado/sorakado_factory.h"

namespace sorakado {
    class Application {
        private:
            std::mutex mutex_;
            std::condition_variable cond_;
            std::queue<std::vector<std::string>> queue_;
            std::queue<std::vector<directsstp::Request>> event_queue_;
            std::unique_ptr<std::thread> th_recv_;
            std::unique_ptr<std::thread> th_send_;
            std::unique_ptr<Sorakado> sorakado_instance_;
            SorakadoType type_;
            std::string path_;
            std::string uuid_;
            bool alive_;
            int scale_;
            bool loaded_;
            bool is_idle_;
            bool is_debug_;

        public:
            Application();
            ~Application();

            operator bool() {
                return alive_;
            }

            void run();

            std::string sendDirectSSTP(std::string method, std::string command, std::vector<std::string> args, std::string script = "", bool hide_on_204 = false);
            void enqueueDirectSSTP(std::vector<directsstp::Request> list);
    };
}

#endif // SORAKADO_APPLICATION_H_
