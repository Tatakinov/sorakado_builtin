#ifndef SORAKADO_BASE_CHARACTER
#define SORAKADO_BASE_CHARACTER

#include <string>
#include <vector>

#include "sorakado/compatible.h"
#include "sorakado/misc.h"
#include "sorakado/watcher.h"

namespace sorakado {
    class Sorakado;
    class Window;
    class BaseCharacter : public Watcher {
        protected:
            Sorakado *parent_;
            Position offset_;
        public:
            BaseCharacter(Sorakado *parent) : Watcher(), parent_(parent), offset_({0, 0}) {}
            virtual ~BaseCharacter() {}
            virtual Rect getRect() const = 0;
            Position getOffset() const {
                return offset_;
            }
            virtual void resetPosition(bool initialize) = 0;
            virtual bool setOffset(int x, int y);
            virtual bool setSize(int w, int h) = 0;
            virtual void press(key_t key, bool down);
            virtual void dnd(const std::vector<std::string> &file_list);
            virtual void hover(float x, float y);
            virtual void click(Window *window, float x, float y, button_t button, bool down, Uint8 clicks);
            virtual void inputText(const std::string &text);
            virtual void editText(const std::string &text);

            std::string sendDirectSSTP(std::string method, std::string command, std::vector<std::string> args, std::string script, bool hide_on_204);
            void enqueueDirectSSTP(std::vector<directsstp::Request> list);
    };
}

#endif // SORAKADO_BASE_CHARACTER
