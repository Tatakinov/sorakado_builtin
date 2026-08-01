#include "sorakado/character.h"

#include <cassert>

#include "logger.h"
#include "sorakado/sorakado.h"
#include "sorakado/window_manager.h"
#include "lib_skeleton/sstp.h"

namespace sorakado {

    Character::Character(Sorakado *parent, std::unique_ptr<WindowManager> manager, int side, const std::string &name) : BaseCharacter(parent), side_(side), name_(name), rect_({0, 0, 0, 0}), window_manager_(std::move(manager)) {
    }

    Character::~Character() {
    }

    std::string Character::getInfo(std::string key, bool fallback) {
        return parent_->getInfo(key, fallback);
    }

    void Character::create(SDL_DisplayID id) {
        window_manager_->create(this, id, name_);
    }

    void Character::destroy(SDL_DisplayID id) {
        window_manager_->destroy(id);
    }

    bool Character::swapBuffers() {
        return window_manager_->swapBuffers();
    }

    void Character::show() {
        window_manager_->show();
    }

    void Character::hide() {
        window_manager_->hide();
    }

    void Character::clearCache() {
        window_manager_->clearCache();
    }

    void Character::raise() {
        window_manager_->raise();
    }

    void Character::key(window_id_t id, sorakado::key_t key, bool down) {
        window_manager_->key(id, key, down);
    }
    
    void Character::input(window_id_t id, const std::string &text) {
        window_manager_->input(id, text);
    }
    void Character::edit(window_id_t id, const std::string &text) {
        window_manager_->edit(id, text);
    }

    void Character::motion(window_id_t id, float x, float y) {
        window_manager_->motion(id, x, y);
    }

    void Character::button(window_id_t id, float x, float y, button_t button, bool down, Uint8 clicks) {
        window_manager_->button(id, x, y, button, down, clicks);
    }

    void Character::wheel(window_id_t id, float x, float y) {
        window_manager_->wheel(id, x, y);
    }
    void Character::drop(const window_id_t id, const std::vector<std::string> &list) {
        window_manager_->drop(id, list);
    }

    void Character::maximized(window_id_t id) {
        window_manager_->maximized(id);
    }

    void Character::focus(window_id_t id, bool focus) {
        window_manager_->focus(id, focus);
    }

    bool Character::focused() const {
        return window_manager_->focused();
    }

    bool Character::setPosition(int x, int y) {
        Logger::log("position", rect_.x, rect_.y, x, y, side());
        if (rect_.x == x && rect_.y == y) {
            return false;
        }
        rect_.x = x;
        rect_.y = y;
        auto offset = getOffset();
        if (!util::isWayland()) {
            window_manager_->position(x + offset.x, y + offset.y);
        }
        change();
        return true;
    }

    bool Character::setSize(int w, int h) {
        Logger::log("resize", rect_.w, rect_.h, w, h, side());
        if (rect_.w == w && rect_.h == h) {
            return false;
        }
        rect_.w = w;
        rect_.h = h;
        change();
        window_manager_->resize(w, h);
        resetPosition(false);
        return true;
    }

    void Character::scroll(float x, float y, float mouse_x, float mouse_y) {
    }
}
