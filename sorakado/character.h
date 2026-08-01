#ifndef SORAKADO_CHARACTER_H_
#define SORAKADO_CHARACTER_H_

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include <SDL3/SDL_video.h>

#include "sorakado/base_character.h"
#include "sorakado/compatible.h"
#include "sorakado/image_cache.h"
#include "sorakado/misc.h"
#include "sorakado/window.h"
#include "sorakado/window_manager.h"

namespace sorakado {
    class Sorakado;

    class Character : public BaseCharacter {
        private:
            int side_;
            std::string name_;
            Rect rect_;
        protected:
            std::unique_ptr<WindowManager> window_manager_;
        public:
            Character(Sorakado *parent, std::unique_ptr<WindowManager> manager, int side, const std::string &name);
            virtual ~Character();
            std::string getInfo(std::string key, bool fallback, bool freeze = true);
            virtual void create(display_t id);
            virtual void destroy(display_t id);
            virtual void draw(std::unique_ptr<ImageCache> &cache) = 0;
            bool swapBuffers();
            int side() const {
                return side_;
            }
            const std::string &name() const {
                return name_;
            }
            void show();
            void raise();
            void hide();
            void clearCache();
            Rect getRect() const override {
                return rect_;
            }
            virtual bool setPosition(int x, int y);
            virtual bool setSize(int w, int h);
            virtual void resetPosition(bool initialize) = 0;

            virtual void scroll(float x, float y, float mouse_x, float mouse_y);

            void key(window_id_t id, key_t key, bool down);
            void input(window_id_t id, const std::string &text);
            void edit(window_id_t id, const std::string &text);
            void motion(window_id_t id, float x, float y);
            void button(window_id_t id, float x, float y, button_t button, bool down, Uint8 clicks);
            void wheel(window_id_t id, float x, float y);
            void drop(window_id_t id, const std::vector<std::string> &list);
            void maximized(window_id_t id);
            void focus(window_id_t id, bool focused);
            bool focused() const;
    };
}

#endif // SORAKADO_CHARACTER_H_
