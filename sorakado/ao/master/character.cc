#include "sorakado/ao/master/character.h"

#include "logger.h"
#include "sorakado/sorakado.h"
#include "sorakado/window_manager.h"
#include "sorakado/ao/misc.h"
#include "sorakado/ao/master/ao.h"

#define MOUSE_BUTTON_LEFT 1
#define MOUSE_BUTTON_MIDDLE 2
#define MOUSE_BUTTON_RIGHT 3

namespace sorakado::ao::master {
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

    Character::Character(sorakado::Sorakado *parent, std::unique_ptr<WindowManager> window_manager, int side, const std::string &name, std::unique_ptr<Seriko> seriko) : sorakado::Character(parent, std::move(window_manager), side, name), seriko_(std::move(seriko)) {
        seriko_->setParent(this);
    }

    bool Character::setPosition(int x, int y) {
        if (!sorakado::Character::setPosition(x, y)) {
            return false;
        }
        change();
        SDL_Rect r;
        auto rect = getRect();
        if (util::isWayland() && !getenv("NINIX_ENABLE_MULTI_MONITOR")) {
            Rect monitor_rect = window_manager_->getMonitorRect(rect);
            r = {monitor_rect.x, monitor_rect.y, monitor_rect.w, monitor_rect.h};
        }
        else {
            auto id = util::getNearestDisplay(rect.x + rect.w / 2, rect.y + rect.h / 2);
            SDL_GetDisplayBounds(id, &r);
        }
        Logger::log("monitor_rect:", r.x,",", r.y,",", r.w, ",",r.h);
        Logger::log("surface_rect:", rect.x,",", rect.y,",", rect.w, ",",rect.h);
        std::vector<std::string> args = {util::to_s(side()), util::to_s(r.x), util::to_s(r.y), util::to_s(r.w), util::to_s(r.h)};
        directsstp::Request req = {"EXECUTE", "UpdateMonitorRect", args};
        enqueueDirectSSTP({req});
        args = {util::to_s(side()), util::to_s(rect.x), util::to_s(rect.y), util::to_s(rect.w), util::to_s(rect.h)};
        req = {"EXECUTE", "UpdateSurfaceRect", args};
        enqueueDirectSSTP({req});
        args = {util::to_s(side())};
        req = {"EXECUTE", "ResetBalloonPosition", args};
        enqueueDirectSSTP({req});
        return true;
    }

    bool Character::setOffset(int x, int y) {
        if (!sorakado::Character::setOffset(x, y)) {
            return false;
        }
        auto r = getRect();
        window_manager_->position(r.x + x, r.y + y);
        alignmentPosition();
        return true;
    }

    bool Character::setSize(int w, int h) {
        if (!sorakado::Character::setSize(w, h)) {
            return false;
        }
        Logger::log("setSize");
        change();
        SDL_Rect r;
        auto rect = getRect();
        if (util::isWayland() && !getenv("NINIX_ENABLE_MULTI_MONITOR")) {
            Rect monitor_rect = window_manager_->getMonitorRect(rect);
            r = {monitor_rect.x, monitor_rect.y, monitor_rect.w, monitor_rect.h};
        }
        else {
            auto id = util::getNearestDisplay(rect.x + rect.w / 2, rect.y + rect.h / 2);
            SDL_GetDisplayBounds(id, &r);
        }
        Logger::log("monitor_rect:", r.x,",", r.y,",", r.w, ",",r.h);
        Logger::log("surface_rect:", rect.x,",", rect.y,",", rect.w, ",",rect.h);
        std::vector<std::string> args = {util::to_s(side()), util::to_s(r.x), util::to_s(r.y), util::to_s(r.w), util::to_s(r.h)};
        directsstp::Request req = {"EXECUTE", "UpdateMonitorRect", args};
        enqueueDirectSSTP({req});
        args = {util::to_s(side()), util::to_s(rect.x), util::to_s(rect.y), util::to_s(rect.w), util::to_s(rect.h)};
        req = {"EXECUTE", "UpdateSurfaceRect", args};
        enqueueDirectSSTP({req});
        args = {util::to_s(side())};
        req = {"EXECUTE", "ResetBalloonPosition", args};
        enqueueDirectSSTP({req});
        return true;
    }

    void Character::resetPosition(bool initialize) {
        auto r = getRect();
        auto m = window_manager_->getMonitorRect(r);
        Logger::log("resetPosition.rect", r.x, r.y, r.w, r.h, side());
        Logger::log("resetPosition.monitor", m.x, m.y, m.w, m.h);
        int origin_x = r.x;
        if (initialize || origin_x < m.x || origin_x + r.w > m.x + m.w) {
            origin_x = m.x + m.w;
            if (side() > 0) {
                auto o = static_cast<Ao *>(parent_)->getCharacterPosition(side() - 1);
                if (o) {
                    if (o->x < origin_x) {
                        origin_x = o->x;
                    }
                }
            }
            origin_x -= r.w;
            if (origin_x < m.x) {
                origin_x = m.x;
            }
        }
        int origin_y = r.y;
        if (initialize || origin_y < m.y || origin_y + r.h > m.y + m.h) {
            origin_y = m.y + m.h;
            origin_y -= r.h;
        }
        Logger::log("resetPosition", initialize, origin_x, origin_y);
        setPosition(origin_x, origin_y);
        alignmentPosition();
    }

    void Character::alignmentPosition() {
        auto f = [](const std::string &v) {
            if (v == "top") {
                return Alignment::Top;
            }
            else if (v == "free") {
                return Alignment::Free;
            }
            else {
                return Alignment::Bottom;
            }
        };
        Alignment align = Alignment::Bottom;
        std::string key_side = util::side2str(side()) + ".seriko.alignmenttodesktop";
        std::string key_all = "seriko.alignmenttodesktop";
        // 優先度が低い順に調べる
        std::string value = parent_->getInfo(key_all, true);
        if (!value.empty()) {
            align = f(value);
        }
        value = parent_->getInfo(key_side, true);
        if (!value.empty()) {
            align = f(value);
        }
        value = parent_->getInfo(key_all, false);
        if (!value.empty()) {
            align = f(value);
        }
        value = parent_->getInfo(key_side, false);
        if (!value.empty()) {
            align = f(value);
        }

        SDL_Rect r;
        auto rect = getRect();
        if (util::isWayland() && !getenv("NINIX_ENABLE_MULTI_MONITOR")) {
            Rect rect;
            rect = window_manager_->getMonitorRect(getRect());
            r = {rect.x, rect.y, rect.w, rect.h};
        }
        else {
            auto id = util::getNearestDisplay(rect.x + rect.w / 2, rect.y + rect.h / 2);
            SDL_GetDisplayBounds(id, &r);
        }
        auto o = getOffset();
        switch (align) {
            case Alignment::Bottom:
                Logger::log("alignmentPosition.surface", rect.x, rect.y, rect.w, rect.h);
                Logger::log("alignmentPosition.monitor", r.x, r.y, r.w, r.h);
                Logger::log("alignmentPosition", side(), rect.x, r.y + r.h - rect.h);
                if (o.y > 0) {
                    setPosition(rect.x, r.y + r.h + o.y - rect.h);
                }
                else if (o.y + rect.h < 0) {
                    setPosition(rect.x, r.y + r.h + o.y - rect.h);
                }
                else {
                    setPosition(rect.x, r.y + r.h - rect.h);
                }
                break;
            case Alignment::Top:
                setPosition(rect.x, r.y);
                break;
            case Alignment::Free:
                // nop
                break;
        }
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
        if (!drag_.has_value()) {
            auto name = getHitBoxName(x, y);
            if (name.empty()) {
                SDL_Cursor *cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
                SDL_SetCursor(cursor);
            }
            else {
                SDL_Cursor *cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
                SDL_SetCursor(cursor);
            }
        }
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
        parent_->hover(side(), x, y);
    }

    void Character::click(Window *window, float x, float y, button_t button, bool down, Uint8 clicks) {
        Logger::log("character.click");
        if (down) {
            directsstp::Request req = {"EXECUTE", "RaiseBalloon", {util::to_s(side())}};
            parent_->enqueueDirectSSTP({req});
        }
        mouse_state_[button].press = down;
        if (button == MOUSE_BUTTON_LEFT && !mouse_state_[button].press) {
            drag_ = std::nullopt;
            alignmentPosition();
        }
        if (!down) {
            mouse_state_[button].drag = false;
        }
        if (!mouse_state_[button].press && !mouse_state_[button].drag) {
            int b = -1;
            switch (button) {
                case 1:
                    b = 0;
                    break;
                case 2:
                    b = 2;
                    break;
                case 3:
                    b = 1;
                    break;
                default:
                    break;
            }
            auto name = getHitBoxName(x, y);
            int surface_x = x;
            int surface_y = y;
            if (util::isWayland()) {
                Position offset = getRect();
                surface_x -= offset.x;
                surface_y -= offset.y;
            }

            std::vector<std::string> args;
            args = {util::to_s(surface_x), util::to_s(surface_y), util::to_s(0), util::to_s(side()), name, util::to_s(b)};
            Logger::log("clicks.", button == MOUSE_BUTTON_RIGHT, clicks);

            if (clicks % 2 == 0) {
                directsstp::Request req = {"NOTIFY", "OnMouseDoubleClick", args};
                parent_->enqueueDirectSSTP({req});
            }
            else if (button != MOUSE_BUTTON_RIGHT) {
                directsstp::Request up = {"NOTIFY", "OnMouseUp", args};
                directsstp::Request click = {"NOTIFY", "OnMouseClick", args};
                parent_->enqueueDirectSSTP({up, click});
            }
            else {
                directsstp::Request up = {"NOTIFY", "OnMouseUp", args};
                directsstp::Request click = {"NOTIFY", "OnMouseClick", args};
#if 0
                // 右クリックメニューを呼び出す
                args = {util::to_s(side()), util::to_s(surface_x), util::to_s(surface_y)};
                directsstp::Request menu = {"EXECUTE", "OpenMenu", args};
                parent_->enqueueDirectSSTP({up, click, menu});
#else
                parent_->enqueueDirectSSTP({up, click});
#endif
                Rect r = {x, y, 0, 0};
                Logger::log("reserve", x, y);
                static_cast<Ao *>(parent_)->reserveMenu(window->getBackendWindow(), side(), {x, y}, window->getMonitorRect(r));
            }
        }
        else if (down) {
            int b = -1;
            switch (button) {
                case 1:
                    b = 0;
                    break;
                case 2:
                    b = 2;
                    break;
                case 3:
                    b = 1;
                    break;
                default:
                    break;
            }
            auto name = getHitBoxName(x, y);
            if (util::isWayland()) {
                Position offset = getRect();
                x = x - offset.x;
                y = y - offset.y;
            }

            std::vector<std::string> args;
            args = {util::to_s(x), util::to_s(y), util::to_s(0), util::to_s(side()), name, util::to_s(b)};
            directsstp::Request req = {"NOTIFY", "OnMouseDown", args};
            parent_->enqueueDirectSSTP({req});
        }
    }

    void Character::scroll(float x, float y, float mouse_x, float mouse_y) {
    }

    void Character::draw(std::unique_ptr<ImageCache> &image_cache) {
        if (!window_manager_->shown()) {
            return;
        }
        auto info = seriko_->get();
        if (prev_info_ && prev_info_.value() == info && !changed()) {
            return;
        }
        update();
        if (!prev_info_ || prev_info_.value() != info) {
            prev_info_ = info;
            current_surface_ = info.getSurface(image_cache);
            if (current_surface_) {
                setSize(current_surface_->width(), current_surface_->height());
                auto o = info.getRect(image_cache);
                setOffset(o.x, o.y);
            }
        }
        if (util::isWayland()) {
            window_manager_->draw(image_cache, getRect(), info, current_surface_);
        }
        else {
            window_manager_->draw(image_cache, {0, 0}, info, current_surface_);
        }
    }

    int Character::getSurfaceID() const {
        return seriko_->getSurfaceID();
    }

    void Character::setSurfaceID(const std::string &id) {
        if (seriko_->setSurfaceID(id)) {
            static_cast<Ao *>(parent_)->surfaceChanged(side(), getSurfaceID());
        }
    }

    bool Character::isPlayingAnimation(const std::string &id) const {
        return seriko_->active(id);
    }

    std::unordered_set<int> Character::getActiveAnimationList() const {
        return seriko_->getActiveAnimationList();
    }

    void Character::startAnimation(const std::string &id) {
        seriko_->activate(From::User, id, 0);
    }

    void Character::bind(int id, std::string from, BindFlag flag) {
        bool is_binding = seriko_->isBinding(id);
        if (flag == BindFlag::True && is_binding) {
            return;
        }
        if (flag == BindFlag::False && !is_binding) {
            return;
        }
        seriko_->bind(id, !is_binding);
    }


    std::string Character::getHitBoxName(int x, int y) {
        if (util::isWayland()) {
            auto r = getRect();
            x -= r.x;
            y -= r.y;
        }
        auto list = seriko_->getCollision();
        for (auto &info : list) {
            for (auto c : info.list) {
                if (c.type == CollisionType::Rect) {
                    if (c.point.size() != 4) {
                        Logger::log("invalid collision type: rect");
                        continue;
                    }
                    int x1 = c.point[0];
                    int y1 = c.point[1];
                    int x2 = c.point[2];
                    int y2 = c.point[3];
                    if (x1 <= x && x2 >= x && y1 <= y && y2 >= y) {
                        return c.id;
                    }
                }
                else if (c.type == CollisionType::Ellipse) {
                    if (c.point.size() != 4) {
                        Logger::log("invalid collision type: ellipse");
                        continue;
                    }
                    int x1 = c.point[0];
                    int y1 = c.point[1];
                    int x2 = c.point[2];
                    int y2 = c.point[3];
                    double xr = std::abs(x1 - x2);
                    double xo = (x1 + x2) / 2.0 - x;
                    double yr = std::abs(y1 - y2);
                    double yo = (y1 + y2) / 2.0 - y;
                    assert(xr);
                    assert(yr);
                    if (xo * xo / xr * xr + yo * yo / yr * yr) {
                        return c.id;
                    }
                }
                else if (c.type == CollisionType::Circle) {
                    if (c.point.size() != 3) {
                        Logger::log("invalid collision type: circle");
                        continue;
                    }
                    int cx = c.point[0] - x;
                    int cy = c.point[1] - y;
                    int cr = c.point[2];
                    if (cx * cx + cy * cy <= cr * cr) {
                        return c.id;
                    }
                }
                else if (c.type == CollisionType::Polygon) {
                    if (c.point.size() % 2 == 1 && c.point.size() >= 6) {
                        Logger::log("invalid collision type: polygon");
                        continue;
                    }
                    c.point.push_back(c.point[0]);
                    c.point.push_back(c.point[1]);
                    int count = 0;
                    while (c.point.size() >= 4) {
                        double x1 = c.point[0];
                        double y1 = c.point[1];
                        double x2 = c.point[2];
                        double y2 = c.point[3];
                        c.point.erase(c.point.begin());
                        c.point.erase(c.point.begin());
                        if (y1 == y2) {
                            continue;
                        }
                        if (y1 > y2) {
                            if (y == y1) {
                                continue;
                            }
                            else if (y == y2 && x <= x2) {
                                count++;
                                continue;
                            }
                        }
                        else {
                            if (y == y1 && x < x1) {
                                count++;
                                continue;
                            }
                            else if (y == y2) {
                                continue;
                            }
                        }
                        if (y < y1 && y < y2) {
                            continue;
                        }
                        else if (y > y1 && y > y2) {
                            continue;
                        }
                        double intersection_x = x1 + (y - y1) * (x2 - x1) / (y2 - y1);
                        if (intersection_x > x) {
                            count++;
                        }
                    }
                    if (count % 2 == 1) {
                        return c.id;
                    }
                }
                else if (c.type == CollisionType::Region) {
                    // TODO stub
                }
            }
        }
        return "";
    }

    void Character::setScale(int scale) {
        seriko_->setScale(scale);
        change();
    }
}
