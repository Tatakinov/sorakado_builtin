#include "sorakado/base_character.h"

#include "sorakado/sorakado.h"
#include "logger.h"

namespace sorakado {
    bool BaseCharacter::setOffset(int x, int y) {
        if (offset_.x == x && offset_.y == y) {
            return false;
        }
        Logger::log("offset", x, y);
        offset_ = {x, y};
        return true;
    }
    void BaseCharacter::press(key_t key, bool down) {
    }

    void BaseCharacter::dnd(const std::vector<std::string> &file_list) {
    }

    void BaseCharacter::hover(float x, float y) {
    }

    void BaseCharacter::click(Window *window, float x, float y, button_t button, bool down, click_t clicks) {
    }

    void BaseCharacter::inputText(const std::string &text)  {
    }

    void BaseCharacter::editText(const std::string &text) {
    }

    lib_skeleton::sstp::Response BaseCharacter::sendDirectSSTP(const directsstp::Request req) {
        return parent_->sendDirectSSTP(req);
    }

    void BaseCharacter::enqueueDirectSSTP(std::vector<directsstp::Request> list) {
        for (auto &v : list) {
            v.side = side();
        }
        parent_->enqueueDirectSSTP(list);
    }
}
