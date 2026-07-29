#include "sorakado/ai/master/window.h"

#include <cassert>
#include <cmath>
#include <string>
#include <unordered_map>

#include "logger.h"
#include "sorakado/base_character.h"
#include "lib_skeleton/sstp.h"

namespace {
    std::unordered_map<key_t, int> key_count;
}

namespace sorakado::ai::master {
    std::unique_ptr<Window> AiMasterWindowFactory::create(sorakado::BaseCharacter *parent, display_t id, std::unique_ptr<BackendWindowFactory> &factory, const std::string &name) const {
        return std::make_unique<AiMasterWindow>(parent, id, factory, name);
    }

    AiMasterWindow::AiMasterWindow(sorakado::BaseCharacter *parent, display_t id, std::unique_ptr<BackendWindowFactory> &factory, const std::string &name) : sorakado::Window(parent, id, factory, name) {
    }

    AiMasterWindow::~AiMasterWindow() {
    }

    bool AiMasterWindow::input(sorakado::window_id_t id, const std::string &text) {
        if (id != SDL_GetWindowID(window_)) {
            return false;
        }
        return true;
    }

    bool AiMasterWindow::edit(sorakado::window_id_t id, const std::string &text) {
        if (id != SDL_GetWindowID(window_)) {
            return false;
        }
        return true;
    }

    bool AiMasterWindow::wheel(sorakado::window_id_t id, float x, float y) {
        if (id != SDL_GetWindowID(window_)) {
            return false;
        }
        // TODO stub
        return true;
    }
}
