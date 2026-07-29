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

    bool AoMenuWindow::button(window_id_t id, float x, float y, button_t button, bool down, Uint8 clicks) {
        if (button != MOUSE_BUTTON_LEFT || down) {
            return false;
        }
        auto action = menu_->getAction();
        if (action == ActionType::None) {
            return false;
        }
        else if (action == ActionType::Site) {
            auto data = menu_->get<MenuModelDataActionWithArgs>();
            if (data && data->args.size() > 0) {
                std::vector<std::string> args;
                args.push_back(data->args[0]);
                if (data->args.size() >= 3) {
                    args.push_back(data->args[2]);
                }
                parent_->enqueueDirectSSTP({{"EXECUTE", "VisitSite", {data->args[0]}}});
            }
        }
        else if (action == ActionType::StayOnTop) {
            // TODO stub
        }
        else if (action == ActionType::Preferences) {
            parent_->enqueueDirectSSTP({{"EXECUTE", "OpenPreferences", {}}});
        }
        else if (action == ActionType::Switch) {
            auto data = menu_->get<MenuModelDataActionWithArgs>();
            if (data && data->args.size() > 0) {
                parent_->enqueueDirectSSTP({{"EXECUTE", "ChangeGhost", {data->args[0]}}});
            }
        }
        else if (action == ActionType::Call) {
            auto data = menu_->get<MenuModelDataActionWithArgs>();
            if (data && data->args.size() > 0) {
                parent_->enqueueDirectSSTP({{"EXECUTE", "SummonGhost", {data->args[0]}}});
            }
        }
        else if (action == ActionType::Shell) {
            auto data = menu_->get<MenuModelDataActionWithArgs>();
            if (data && data->args.size() > 0) {
                parent_->enqueueDirectSSTP({{"EXECUTE", "ChangeShell", {data->args[0]}}});
            }
        }
        else if (action == ActionType::DressUp) {
            // TODO stub
        }
        else if (action == ActionType::Balloon) {
            auto data = menu_->get<MenuModelDataActionWithArgs>();
            if (data && data->args.size() > 0) {
                parent_->enqueueDirectSSTP({{"EXECUTE", "ChangeBalloon", {data->args[0]}}});
            }
        }
        else if (action == ActionType::BasewareVersion) {
            parent_->enqueueDirectSSTP({{"EXECUTE", "ShowBasewareVersion", {}}});
        }
        else if (action == ActionType::Close) {
            parent_->enqueueDirectSSTP({{"EXECUTE", "CloseGhost", {}}});
        }
        else if (action == ActionType::CloseAll) {
            parent_->enqueueDirectSSTP({{"EXECUTE", "CloseAllGhost", {}}});
        }
        else if (action == ActionType::ScriptInputBox) {
            parent_->enqueueDirectSSTP({{"EXECUTE", "OpenScriptInputBox", {}}});
        }
        parent_->click(this, x, y, button, down, clicks);
        return true;
    }
}
