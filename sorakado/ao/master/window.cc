#include "sorakado/ao/master/window.h"

#include <cassert>
#include <cmath>
#include <string>
#include <unordered_map>

#include "logger.h"
#include "sorakado/ao/master/character.h"
#include "lib_skeleton/sstp.h"

namespace {
    std::unordered_map<key_t, int> key_count;
}

namespace sorakado::ao::master {
    std::unique_ptr<Window> AoMasterWindowFactory::create(sorakado::BaseCharacter *parent, display_t id, std::unique_ptr<BackendWindowFactory> &factory, const std::string &name) const {
        return std::make_unique<AoMasterWindow>(parent, id, factory, name);
    }

    AoMasterWindow::AoMasterWindow(sorakado::BaseCharacter *parent, display_t id, std::unique_ptr<BackendWindowFactory> &factory, const std::string &name) : sorakado::Window(parent, id, factory, name) {
    }

    AoMasterWindow::~AoMasterWindow() {
    }

    bool AoMasterWindow::key(sorakado::window_id_t id, key_t key, bool down) {
        if (id != SDL_GetWindowID(window_)) {
            return false;
        }
        parent_->press(key, down);
        return true;
    }

    bool AoMasterWindow::input(sorakado::window_id_t id, const std::string &text) {
        if (id != SDL_GetWindowID(window_)) {
            return false;
        }
        return true;
    }

    bool AoMasterWindow::edit(sorakado::window_id_t id, const std::string &text) {
        if (id != SDL_GetWindowID(window_)) {
            return false;
        }
        return true;
    }

    bool AoMasterWindow::wheel(sorakado::window_id_t id, float x, float y) {
        if (id != SDL_GetWindowID(window_)) {
            return false;
        }
        // TODO stub
        return true;
    }

    bool AoMasterWindow::drop(sorakado::window_id_t id, const std::vector<std::string> &file_list) {
        if (id != SDL_GetWindowID(window_)) {
            return false;
        }
        parent_->dnd(file_list);
        return true;
    }

    bool AoMasterWindow::maximized(window_id_t id) {
        if (!Window::maximized(id)) {
            return false;
        }
        static_cast<Character *>(parent_)->displayChanged();
        return true;
    }
}
