#include "sorakado/ai/script_inputbox.h"

#include "sorakado/sorakado.h"

namespace sorakado::ai {
    void ScriptInputbox::activate(const std::string &text) {
        directsstp::Request req = {"SEND", "", {}, text};
        parent_->enqueueDirectSSTP({req});
    }

    void ScriptInputbox::cancel(const std::string &reason) {
        alive_ = false;
    }
}
