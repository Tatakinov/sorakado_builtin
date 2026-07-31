#ifndef SORAKADO_ISORAKADO_H_
#define SORAKADO_ISORAKADO_H_

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "lib_skeleton/sorakado.h"
#include "sorakado/compatible.h"
#include "sorakado/misc.h"

namespace sorakado {
    class Application;

    class Sorakado {
        private:
            std::filesystem::path dir_;
        protected:
            Application *parent_;
            std::unordered_map<std::string, std::string> descript_info_;
        public:
            Sorakado(Application *parent, std::filesystem::path dir);
            virtual ~Sorakado() {}
            std::string getInfo(std::string key, bool fallback);

            // sorakado event
            virtual std::optional<lib_skeleton::sorakado::Response> sorakadoEventImmediately(const lib_skeleton::sorakado::Request &req) = 0;
            virtual void sorakadoEvent(const std::vector<std::string> &args) = 0;
            // window event
            virtual void display(const display_t display, const bool added) = 0;
            virtual void key(sorakado::window_id_t id, sorakado::key_t key, bool down) = 0;
            virtual void input(window_id_t id, const std::string &text) = 0;
            virtual void edit(window_id_t id, const std::string &text) = 0;
            virtual void motion(sorakado::window_id_t id, float x, float y) = 0;
            virtual void button(sorakado::window_id_t id, float x, float y, sorakado::button_t button, bool down, Uint8 clicks) = 0;
            virtual void wheel(sorakado::window_id_t id, float x, float y) = 0;
            virtual void drop(sorakado::window_id_t id, const std::vector<std::string> &list) = 0;
            virtual void maximized(sorakado::window_id_t id) = 0;
            virtual void focus(sorakado::window_id_t id, bool focused) = 0;

            virtual void hover(int side, float x, float y) = 0;
            virtual void click(int side, float x, float y, button_t button, bool down, Uint8 clicks) = 0;

            virtual void run();

            virtual bool draw() = 0;
            std::string sendDirectSSTP(std::string method, std::string command, std::vector<std::string> args, std::string script, bool hide_on_204);
            void enqueueDirectSSTP(std::vector<directsstp::Request> list);
    };
}

#endif // SORAKADO_ISORAKADO_H_
