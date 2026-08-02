#include "sorakado/ai/base_inputbox.h"

#include "logger.h"

#define MOUSE_BUTTON_LEFT 1
#define MOUSE_BUTTON_MIDDLE 2
#define MOUSE_BUTTON_RIGHT 3

namespace sorakado::ai {
    void BaseInputbox::draw(std::unique_ptr<ImageCache> &image_cache) {
        if (!window_manager_->shown()) {
            return;
        }
        Logger::log("draw2", info_.changed(), changed());
        if (!info_.changed() && !changed()) {
            return;
        }
        update();
        if (info_.changed()) {
            region_ = info_.getSurface(image_cache);
        }
        info_.update();
        if (util::isWayland()) {
            window_manager_->draw(image_cache, getRect(), info_, region_);
        }
        else {
            window_manager_->draw(image_cache, {0, 0}, info_, region_);
        }
    }

    void BaseInputbox::resetPosition(bool initialize) {
        auto rect = window_manager_->getMonitorRect(getRect());
        auto r = getRect();
        setPosition((rect.w - r.w) / 2, (rect.h - r.h) / 2);
        change();
    }

    void BaseInputbox::press(key_t key, bool down) {
        if (!down) {
            return;
        }
        switch (key) {
            case SDLK_RETURN:
                activate(info_.getText());
                change();
                break;
            case SDLK_ESCAPE:
                cancel("close");
                change();
                break;
            case SDLK_BACKSPACE:
                info_.erase();
                change();
                break;
            case SDLK_RIGHT:
                info_.incrementCursorIndex();
                change();
                break;
            case SDLK_LEFT:
                info_.decrementCursorIndex();
                change();
                break;
            default:
                break;
        }
    }

    void BaseInputbox::hover(float x, float y) {
        Logger::log("input.hover", x, y);
        if (!drag_.has_value() && mouse_state_[MOUSE_BUTTON_LEFT].press) {
            if (util::isWayland()) {
                drag_ = {x, y};
            }
            else {
                float mouse_x, mouse_y;
                SDL_GetGlobalMouseState(&mouse_x, &mouse_y);
                drag_ = {mouse_x, mouse_y};
            }
        }
        if (drag_.has_value()) {
            auto r = getRect();
            auto [dx, dy] = drag_.value();
            if (util::isWayland()) {
                setPosition(r.x + x - dx, r.y + y - dy);
                drag_ = {x, y};
            }
            else {
                auto r = getRect();
                float mouse_x, mouse_y;
                SDL_GetGlobalMouseState(&mouse_x, &mouse_y);
                setPosition(r.x + mouse_x - drag_->x, r.y + mouse_y - drag_->y);
                drag_ = {mouse_x, mouse_y};
            }
        }
        for (auto &[k, v] : mouse_state_) {
            if (v.press) {
                v.drag = true;
            }
        }
        change();
    }

    void BaseInputbox::click(Window *window, float x, float y, button_t button, bool down, click_t clicks) {
        mouse_state_[button].press = down;
        if (button == MOUSE_BUTTON_LEFT && !mouse_state_[button].press) {
            drag_ = std::nullopt;
        }
        if (!down) {
            mouse_state_[button].drag = false;
        }
        change();
    }

    void BaseInputbox::inputText(const std::string &text) {
        info_.input(text);
        change();
    }

    void BaseInputbox::editText(const std::string &text) {
        info_.edit(text);
        change();
    }
}
