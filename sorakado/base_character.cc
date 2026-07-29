#include "sorakado/base_character.h"

#include "sorakado/sorakado.h"

namespace sorakado {
    void BaseCharacter::press(key_t key, bool down) {
    }

    void BaseCharacter::dnd(const std::vector<std::string> &file_list) {
    }

    void BaseCharacter::hover(float x, float y) {
    }

    void BaseCharacter::click(Window *window, float x, float y, button_t button, bool down, Uint8 clicks) {
    }

    void BaseCharacter::inputText(const std::string &text)  {
    }

    void BaseCharacter::editText(const std::string &text) {
    }

    std::string BaseCharacter::sendDirectSSTP(std::string method, std::string command, std::vector<std::string> args) {
        return parent_->sendDirectSSTP(method, command, args);
    }

    void BaseCharacter::enqueueDirectSSTP(std::vector<directsstp::Request> list) {
        parent_->enqueueDirectSSTP(list);
    }
}
