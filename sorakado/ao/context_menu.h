#ifndef SORAKADO_AO_CONTEXT_MENU_H_
#define SORAKADO_AO_CONTEXT_MENU_H_

#include "sorakado/base_character.h"
#include "sorakado/ao/menu.h"
#include "sorakado/ao/menu_window.h"

namespace sorakado {
    class Sorakado;
    class ImageCache;
    class WindowFactory;
}

namespace sorakado::ao {
    class ContextMenu : public sorakado::BaseCharacter {
        private:
            struct SubMenuInitializer {
                std::vector<MenuModelData> data;
                Position pos;
                Rect parent_r;
            };
            bool alive_;
            bool focus_gained_;
            int index_in_progress_;
            const Rect display_r_;
            window_t parent_window_;
            std::unique_ptr<ImageCache> &cache_;
            std::unique_ptr<WrapFont> &font_;
            std::vector<std::unique_ptr<AoMenuWindow>> windows_;
            bool is_idle_;
            int delay_;
            SubMenuInitializer initializer_;
        public:
            ContextMenu(sorakado::Sorakado *parent, const Rect &display_r, window_t parent_window, std::unique_ptr<ImageCache> &cache, std::unique_ptr<WrapFont> &font);
            ~ContextMenu();

            Rect getRect() const override {
                return {0, 0, 0, 0};
            }
            void createSubMenu(const std::vector<MenuModelData> &data, const Position &pos, const Rect &parent_r);
            void createSubMenuDefer(const std::vector<MenuModelData> &data, const Position &pos, const Rect &parent_r);

            void run();

            void resetPosition(bool initialize) override {
            }

            bool alive() const;

            void draw(std::unique_ptr<ImageCache> &cache);
            bool swapBuffers();

            bool setSize(int w, int h) override {
                return true;
            }

            void press(sorakado::key_t key, bool down) override;

            void dnd(const std::vector<std::string> &file_list) override {
            }

            void hover(float x, float y) override;

            void click(Window *window, float x, float y, button_t button, bool down, click_t clicks) override {
                alive_ = false;
            }

            void key(window_id_t id, key_t key, bool down);
            void motion(window_id_t id, float x, float y);
            void button(window_id_t id, float x, float y, button_t button, bool down, click_t clicks);
            void wheel(window_id_t id, float x, float y);
            void focus(window_id_t id, bool focused);
            bool focused() const;
            bool focusGained() const {
                return focus_gained_;
            }
    };
}

#endif // SORAKADO_AO_CONTEXT_MENU_H_
