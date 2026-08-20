#include "sorakado/ao/menu_window.h"

#include "sorakado/ao/context_menu.h"

#include "logger.h"

#define MOUSE_BUTTON_LEFT 1
#define MOUSE_BUTTON_MIDDLE 2
#define MOUSE_BUTTON_RIGHT 3

namespace sorakado::ao {
    AoMenuWindow::AoMenuWindow(sorakado::BaseCharacter *parent, display_t id, std::unique_ptr<BackendWindowFactory> &factory, const std::string &name, int x, int y, int width, int height, std::unique_ptr<SubMenu> menu) : sorakado::Window(parent, id, factory, name, width, height), x_(x), y_(y), menu_(std::move(menu)) {
    }

    void AoMenuWindow::draw(std::unique_ptr<ImageCache> &cache) {
        auto region = menu_->getSurface(cache);
        Window::draw(cache, {0, 0}, *menu_, region);
    }

    bool AoMenuWindow::key(sorakado::window_id_t id, sorakado::key_t key, bool down) {
        // TODO stub
        if (id != SDL_GetWindowID(Window::getBackendWindow())) {
            return false;
        }
        return true;
    }

    bool AoMenuWindow::input(sorakado::window_id_t id, const std::string &text) {
        if (id != SDL_GetWindowID(Window::getBackendWindow())) {
            return false;
        }
        return true;
    }

    bool AoMenuWindow::edit(sorakado::window_id_t id, const std::string &text) {
        if (id != SDL_GetWindowID(Window::getBackendWindow())) {
            return false;
        }
        return true;
    }

    bool AoMenuWindow::wheel(sorakado::window_id_t id, float x, float y) {
        if (id != SDL_GetWindowID(Window::getBackendWindow())) {
            return false;
        }
        return true;
    }

    bool AoMenuWindow::drop(sorakado::window_id_t id, const std::vector<std::string> &list) {
        if (id != SDL_GetWindowID(Window::getBackendWindow())) {
            return false;
        }
        return true;
    }

    bool AoMenuWindow::motion(window_id_t id, float x, float y) {
        if (id != SDL_GetWindowID(Window::getBackendWindow())) {
            return false;
        }
        menu_->unhighlight();
        if (menu_->highlight(x, y)) {
            parent_->change();
            parent_->hover(x, y);
            auto submenu = menu_->get<MenuModelDataSubMenu>();
            if (submenu) {
                auto parent_r = menu_->rect();
                parent_r.x += x_;
                parent_r.y += y_;
                Position pos = menu_->rect();
                pos.x += x_ + parent_r.w;
                pos.y += y_ + menu_->getSelectedItemY();
                static_cast<ContextMenu *>(parent_)->createSubMenuDefer(submenu.value().children, pos, parent_r);
            }
        }
        return true;
    }

    bool AoMenuWindow::button(window_id_t id, float x, float y, button_t button, bool down, click_t clicks) {
        if (button != MOUSE_BUTTON_LEFT || down) {
            return false;
        }
        auto submenu = menu_->get<MenuModelDataSubMenu>();
        if (submenu) {
            return false;
        }
        auto item = menu_->get<MenuModelDataItem>();
        if (item) {
            parent_->enqueueDirectSSTP({{"EXECUTE", item->command, item->args}});
        }
        // TODO stub: Check, DressUp
        parent_->click(this, x, y, button, down, clicks);
        return true;
    }
}
