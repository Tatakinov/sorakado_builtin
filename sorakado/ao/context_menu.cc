#include "sorakado/ao/context_menu.h"

#include "sorakado/popup_backend_window_factory.h"
#include "sorakado/sorakado.h"
#include "sorakado/window_manager.h"

#include "logger.h"

namespace sorakado::ao {
    namespace {
        constexpr int invalid = -1;
        constexpr int delay = 5;
    }

    ContextMenu::ContextMenu(sorakado::Sorakado *parent, const Rect &display_r, window_t parent_window, std::unique_ptr<ImageCache> &cache, std::unique_ptr<WrapFont> &font) : sorakado::BaseCharacter(parent, -1), alive_(true), focus_gained_(false), index_in_progress_(-1), display_r_(display_r), parent_window_(parent_window), cache_(cache), font_(font), is_idle_(true), delay_(invalid) {
    }

    ContextMenu::~ContextMenu() {
    }

    void ContextMenu::createSubMenuDefer(const std::vector<MenuModelData> &data, const Position &pos, const Rect &parent_r) {
        initializer_ = {data, pos, parent_r};
        delay_ = delay;
    }

    void ContextMenu::createSubMenu(const std::vector<MenuModelData> &data, const Position &pos, const Rect &parent_r) {
        int x = pos.x, y = pos.y;
        auto menu = std::make_unique<SubMenu>(data, display_r_, font_);
        auto r = menu->rect();
        if (r.w == 0 || r.h == 0) {
            return;
        }
        if (x + r.w > display_r_.x + display_r_.w) {
            x = parent_r.x - r.w;
        }
        if (y + r.h > display_r_.y + display_r_.h) {
            y = display_r_.y + display_r_.h - r.h;
        }
        std::unique_ptr<BackendWindowFactory> factory = std::make_unique<PopupBackendWindowFactory>(parent_window_, x, y, SDL_WINDOW_POPUP_MENU | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS);
        std::string name = "unused";
        windows_.emplace_back(std::make_unique<AoMenuWindow>(this, 0, factory, name, x, y, r.w, r.h, std::move(menu)));
        windows_.back()->show();
        change();
    }

    void ContextMenu::run() {
        if (delay_ > invalid) {
            if (is_idle_) {
                if (delay_-- == 0) {
                    createSubMenu(initializer_.data, initializer_.pos, initializer_.parent_r);
                }
            }
            else {
                delay_ = delay;
            }
        }
        is_idle_ = true;
    }

    void ContextMenu::draw(std::unique_ptr<ImageCache> &cache) {
        if (!changed()) {
            return;
        }
        update();
        for (auto &v : windows_) {
            v->draw(cache);
        }
    }

    bool ContextMenu::swapBuffers() {
        bool redrawn = false;
        for (auto &v : windows_) {
            redrawn = v->swapBuffers() || redrawn;
        }
        return redrawn;
    }

    bool ContextMenu::alive() const {
        return alive_;
    }

    void ContextMenu::press(sorakado::key_t key, bool down) {
        alive_ = false;
    }

    void ContextMenu::hover(float x, float y) {
        if (index_in_progress_ >= 0 && index_in_progress_ + 1 < windows_.size()) {
            windows_.erase(std::next(windows_.begin(), index_in_progress_ + 1), windows_.end());
        }
    }

    void ContextMenu::key(window_id_t id, key_t key, bool down) {
        for (auto &v : windows_) {
            v->key(id, key, down);
        }
    }

    void ContextMenu::motion(window_id_t id, float x, float y) {
        for (index_in_progress_ = 0; index_in_progress_ < windows_.size(); index_in_progress_++) {
            if (windows_[index_in_progress_]->motion(id, x, y)) {
                is_idle_ = false;
                break;
            }
        }
    }

    void ContextMenu::button(window_id_t id, float x, float y, button_t button, bool down, click_t clicks) {
        for (auto &v : windows_) {
            if (v->button(id, x, y, button, down, clicks)) {
                is_idle_ = false;
            }
        }
    }

    void ContextMenu::wheel(window_id_t id, float x, float y) {
        for (auto &v : windows_) {
            if (v->wheel(id, x, y)) {
                is_idle_ = false;
            }
        }
    }

    void ContextMenu::focus(window_id_t id, bool focused) {
        for (auto &v : windows_) {
            if (v->focus(id, focused)) {
                if (focused && !focus_gained_) {
                    focus_gained_ = true;
                }
                is_idle_ = false;
            }
        }
    }

    bool ContextMenu::focused() const {
        for (auto &v : windows_) {
            if (v->focused()) {
                return true;
            }
        }
        return false;
    }
}
