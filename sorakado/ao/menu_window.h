#ifndef SORAKADO_AO_MENU_WINDOW_H_
#define SORAKADO_AO_MENU_WINDOW_H_

#include "sorakado/misc.h"

#include <memory>

#include "sorakado/backend_window_factory.h"
#include "sorakado/base_character.h"
#include "sorakado/window.h"
#include "sorakado/window_factory.h"
#include "sorakado/ao/menu.h"

namespace sorakado::ao {
    class AoMenuWindow : public sorakado::Window {
        private:
            int x_, y_;
            std::unique_ptr<SubMenu> menu_;
        public:
            AoMenuWindow(sorakado::BaseCharacter *parent, display_t id, std::unique_ptr<BackendWindowFactory> &factory, const std::string &name, int x, int y, int width, int height, std::unique_ptr<SubMenu> menu);
            ~AoMenuWindow() {}

            void draw(std::unique_ptr<ImageCache> &cache);
            bool key(sorakado::window_id_t id, sorakado::key_t key, bool down) override;
            bool input(sorakado::window_id_t id, const std::string &text) override;
            bool edit(sorakado::window_id_t id, const std::string &text) override;
            bool wheel(sorakado::window_id_t id, float x, float y) override;
            bool drop(sorakado::window_id_t id, const std::vector<std::string> &list) override;
            bool motion(window_id_t id, float x, float y) override;
            bool button(window_id_t id, float x, float y, button_t button, bool down, click_t clicks) override;
    };
}

#endif // SORAKADO_AO_MASTER_WINDOW_H_
