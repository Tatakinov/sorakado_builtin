#include "sorakado/ai/inputbox.h"

#include "sorakado/sorakado.h"

namespace sorakado::ai {
    void Inputbox::activate(const std::string &text) {
        // TODO support --reference
        if (id_.starts_with("On")) {
            std::vector<std::string> args = { text, "" };
            directsstp::Request req = {"NOTIFY", id_, args};
            parent_->enqueueDirectSSTP({req});
        }
        else {
            std::vector<std::string> args = { id_, text, "" };
            directsstp::Request req = {"NOTIFY", "OnUserInput", args};
            parent_->enqueueDirectSSTP({req});
        }
        alive_ = false;
    }

    void Inputbox::cancel(const std::string &reason) {
        // TODO support --reference
        std::vector<std::string> args = { id_, reason, "" };
        if (reason == "timeout") {
            directsstp::Request req = {"NOTIFY", "OnUserInputCancel", args};
            directsstp::Request fallback = {"NOTIFY", "OnUserInput", args};
            parent_->enqueueDirectSSTP({req, fallback});
        }
        else {
            directsstp::Request req = {"NOTIFY", "OnUserInputCancel", args};
            parent_->enqueueDirectSSTP({req});
        }
    }
}
