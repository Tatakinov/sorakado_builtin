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

    std::string BaseCharacter::sendDirectSSTP(std::string method, std::string command, std::vector<std::string> args, std::string script, bool hide_on_204) {
        return parent_->sendDirectSSTP(method, command, args, script, hide_on_204);
    }

    void BaseCharacter::enqueueDirectSSTP(std::vector<directsstp::Request> list) {
        parent_->enqueueDirectSSTP(list);
    }
}
