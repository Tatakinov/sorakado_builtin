#include "sorakado/ai/master/character.h"

#include "sorakado/sorakado.h"
#include "sorakado/window_manager.h"

#include "logger.h"

#define MOUSE_BUTTON_LEFT 1
#define MOUSE_BUTTON_MIDDLE 2
#define MOUSE_BUTTON_RIGHT 3

namespace sorakado::ai::master {
    namespace {
        std::unordered_map<key_t, std::string> key2s = {
            { SDLK_0, "0" },
            { SDLK_1, "1" },
            { SDLK_2, "2" },
            { SDLK_3, "3" },
            { SDLK_4, "4" },
            { SDLK_5, "5" },
            { SDLK_6, "6" },
            { SDLK_7, "7" },
            { SDLK_8, "8" },
            { SDLK_9, "9" },
            { SDLK_A, "a" },
            { SDLK_B, "b" },
            { SDLK_C, "c" },
            { SDLK_D, "d" },
            { SDLK_E, "e" },
            { SDLK_F, "f" },
            { SDLK_G, "g" },
            { SDLK_H, "h" },
            { SDLK_I, "i" },
            { SDLK_J, "j" },
            { SDLK_K, "k" },
            { SDLK_L, "l" },
            { SDLK_M, "m" },
            { SDLK_N, "n" },
            { SDLK_O, "o" },
            { SDLK_P, "p" },
            { SDLK_Q, "q" },
            { SDLK_R, "r" },
            { SDLK_S, "s" },
            { SDLK_T, "t" },
            { SDLK_U, "u" },
            { SDLK_V, "v" },
            { SDLK_W, "w" },
            { SDLK_X, "x" },
            { SDLK_Y, "y" },
            { SDLK_Z, "z" },
        };
    }

    Character::Character(sorakado::Sorakado *parent, std::unique_ptr<WindowManager> window_manager, int side, const std::string &name, std::unique_ptr<ImageCache> &image_cache, std::unique_ptr<FontCache> &font_cache) : sorakado::Character(parent, std::move(window_manager), side, name), offset_(0, 0), info_(this, side, image_cache, font_cache), raise_on_talk_(false) {
    }

    void Character::resetPosition(bool initialize) {
        offset_ = {0, 0};
        std::vector<std::string> args = {util::to_s(side()), util::to_s(offset_.x), util::to_s(offset_.y)};
        directsstp::Request req = {"EXECUTE", "UpdateBalloonOffset", args};
        enqueueDirectSSTP({req});
        args = {util::to_s(side())};
        req = {"EXECUTE", "ResetBalloonPosition", args};
        enqueueDirectSSTP({req});
    }

    void Character::press(key_t key, bool down) {
        if (!down) {
            return;
        }
        std::vector<std::string> args = { key2s[key], util::to_s(key) };
        directsstp::Request req = {"NOTIFY", "OnKeyPress", args};
        parent_->enqueueDirectSSTP({req});
    }

    void Character::dnd(const std::vector<std::string> &file_list) {
        std::vector<std::string> args;
        args.reserve(1 + file_list.size());
        args.push_back(util::to_s(side()));
        for (auto &path : file_list) {
            args.push_back(path);
        }
        directsstp::Request req = {"EXECUTE", "AnalyzeFileMagic", args};
        parent_->enqueueDirectSSTP({req});
    }

    void Character::hover(float x, float y) {
        if (util::isWayland()) {
            auto r = getRect();
            x -= r.x;
            y -= r.y;
        }
        info_.hit(x, y);
        auto link = info_.getLink();
        Logger::log("hover", x, y, link.content.event);
        if (prev_link_ != link) {
            prev_link_ = link;
            if (link.content.event.empty()) {
                if (link.content.is_anchor) {
                    directsstp::Request anchor = {"NOTIFY", "OnAnchorEnter", {}};
                    parent_->enqueueDirectSSTP({anchor});
                }
                else {
                    directsstp::Request choice = {"NOTIFY", "OnChoiceEnter", {}};
                    parent_->enqueueDirectSSTP({choice});
                }
            }
            else {
                auto args = link.content.args;
                args.insert(args.begin(), link.content.event);
                args.insert(args.begin(), link.content.text);
                if (link.content.is_anchor) {
                    directsstp::Request anchor = {"NOTIFY", "OnAnchorEnter", args};
                    parent_->enqueueDirectSSTP({anchor});
                }
                else {
                    directsstp::Request choice = {"NOTIFY", "OnChoiceEnter", args};
                    parent_->enqueueDirectSSTP({choice});
                }
            }
        }
    }

    void Character::scroll(float x, float y, float mouse_x, float mouse_y) {
        info_.scroll(y);
    }

    void Character::draw(std::unique_ptr<ImageCache> &image_cache) {
        if (!window_manager_->shown()) {
            return;
        }
        if (!info_.changed() && !changed()) {
            return;
        }
        update();
        if (info_.changed()) {
            region_ = info_.getSurface(image_cache);
            if (region_) {
                setSize(region_->width(), region_->height());
            }
        }
        info_.update();
        if (util::isWayland()) {
            window_manager_->draw(image_cache, getRect(), info_, region_);
        }
        else {
            window_manager_->draw(image_cache, {0, 0}, info_, region_);
        }
    }

    void Character::setBalloonID(int id) {
        if (id == -1) {
            info_.setID(-1);
            info_.hide();
            return;
        }
        info_.setID(id);
    }

    std::string Character::getInfo(int id, std::string key, std::string fallback) {
        // TODO id
        std::string ret = sorakado::Character::getInfo(key, false);
        if (ret.empty()) {
            return fallback;
        }
        return ret;
    }

    void Character::setBalloonPosition(int x, int y) {
        setPosition(x - offset_.x, y - offset_.y);
    }

    void Character::setBalloonDirection(int direction) {
        info_.setDirection(direction == 1);
    }

    void Character::raiseOnTalk() {
        raise_on_talk_ = true;
    }

    bool Character::setOffset(int x, int y) {
        auto r = getRect();
        if (r.x + offset_.x == x && r.y + offset_.y == y) {
            return false;
        }
        offset_ = {x - r.x, y - r.y};
        window_manager_->position(x, y);
        change();
        std::vector<std::string> args = {util::to_s(side()), util::to_s(offset_.x), util::to_s(offset_.y)};
        directsstp::Request req = {"EXECUTE", "UpdateBalloonOffset", args};
        enqueueDirectSSTP({req});
        args = {util::to_s(side())};
        req = {"EXECUTE", "ResetBalloonPosition", args};
        enqueueDirectSSTP({req});
        return true;
    }

    bool Character::setSize(int w, int h) {
        if (!sorakado::Character::setSize(w, h)) {
            return false;
        }
        auto r = getRect();
        std::vector<std::string> args = {util::to_s(side()), util::to_s(r.x), util::to_s(r.y), util::to_s(r.w), util::to_s(r.h)};
        directsstp::Request req = {"EXECUTE", "UpdateBalloonRect", args};
        enqueueDirectSSTP({req});
        args = {util::to_s(side())};
        req = {"EXECUTE", "ResetBalloonPosition", args};
        enqueueDirectSSTP({req});
        return true;
    }

    void Character::click(Window *window, float x, float y, button_t button, bool down, Uint8 clicks) {
        if (util::isWayland()) {
            auto r = getRect();
            x -= r.x;
            y -= r.y;
        }
        info_.hit(x, y);
        if (button == MOUSE_BUTTON_LEFT && !down && !mouse_state_[button].drag) {
            auto link = info_.getLink();
            if (prev_link_ != link) {
                prev_link_ = link;
            }
            if (!link.content.event.empty()) {
                if (link.content.event.starts_with("On")) {
                    directsstp::Request req = {"NOTIFY", link.content.event, link.content.args, {}, true};
                    parent_->enqueueDirectSSTP({req});
                }
                else if (link.content.event.starts_with("script:")) {
                    // TODO stub
                }
                else {
                    auto args = link.content.args;
                    args.insert(args.begin(), link.content.event);
                    args.insert(args.begin(), link.content.text);
                    if (link.content.is_anchor) {
                        Logger::log("anchor.", link.content.event);
                        directsstp::Request anchor_ex = {"NOTIFY", "OnAnchorSelectEx", args};
                        directsstp::Request anchor = {"NOTIFY", "OnAnchorSelect", {link.content.event}, {}, true};
                        parent_->enqueueDirectSSTP({anchor_ex, anchor});
                    }
                    else {
                        Logger::log("choice.", link.content.event);
                        directsstp::Request choice_ex = {"NOTIFY", "OnChoiceSelectEx", args};
                        directsstp::Request choice = {"NOTIFY", "OnChoiceSelect", {link.content.event}, {}, true};
                        parent_->enqueueDirectSSTP({choice_ex, choice});
                    }
                }
            }
            else {
                // FIXME button enum / click count
                std::vector<std::string> args = {util::to_s(button), "1", util::to_s(side())};
                directsstp::Request req = {"EXECUTE", "NotifyBalloonClick", args};
                parent_->enqueueDirectSSTP({req});
            }
        }
    }

    void Character::appendText(const std::string &text) {
        if (raise_on_talk_) {
            raise_on_talk_ = false;
            raise();
        }
        info_.appendText(text);
        info_.show();
    }

    void Character::appendLinkBegin(bool is_anchor, const std::string &event, const std::vector<std::string> &args) {
        info_.appendLinkBegin(is_anchor, event, args);
    }

    void Character::appendLinkEnd() {
        info_.appendLinkEnd();
    }

    void Character::setCursorPosition(std::string axis, double value, bool is_absolute, MoveUnit unit) {
        info_.setCursorPosition(axis, value, is_absolute, unit);
    }

    void Character::newLine() {
        info_.newBuffer(true);
    }

    void Character::clearText(bool initialize) {
        info_.clear(initialize);
    }

    void Character::setScale(int scale) {
        info_.setScale(scale);
    }
}
