#include "sorakado/ai/inputbox_window.h"

#include "sorakado/character.h"

namespace sorakado::ai {
    AiInputboxWindowFactory::AiInputboxWindowFactory(const Rect &inputbox_r, std::unique_ptr<WrapFont> &font) : inputbox_r_(inputbox_r), font_(font) {
    }

    std::unique_ptr<Window> AiInputboxWindowFactory::create(sorakado::BaseCharacter *parent, display_t id, std::unique_ptr<BackendWindowFactory> &factory, const std::string &name) const {
        return std::make_unique<AiInputboxWindow>(parent, id, factory, name, inputbox_r_, font_);
    }

    AiInputboxWindow::AiInputboxWindow(sorakado::BaseCharacter *parent, display_t id, std::unique_ptr<BackendWindowFactory> &factory, const std::string &name, const Rect &inputbox_r, std::unique_ptr<WrapFont> &font) : Window(parent, id, factory, name), inputbox_r_(inputbox_r), font_(font) {
        SDL_PropertiesID p = SDL_CreateProperties();
        // FIXME password, number, etc
        SDL_SetNumberProperty(p, SDL_PROP_TEXTINPUT_TYPE_NUMBER, SDL_TEXTINPUT_TYPE_TEXT);
        SDL_SetBooleanProperty(p, SDL_PROP_TEXTINPUT_MULTILINE_BOOLEAN, false);
        SDL_StartTextInputWithProperties(getBackendWindow(), p);
    }

    bool AiInputboxWindow::motion(window_id_t id, float x, float y) {
        if (!Window::motion(id, x, y)) {
            return false;
        }
        resetInputArea();
        return true;
    }

    bool AiInputboxWindow::maximized(window_id_t id) {
        if (!Window::maximized(id)) {
            return false;
        }
        if (util::isWayland()) {
            resetInputArea();
        }
        return true;
    }

    bool AiInputboxWindow::button(window_id_t id, float x, float y, button_t button, bool down, click_t clicks) {
        if (!Window::button(id, x, y, button, down, clicks)) {
            return false;
        }
        resetInputArea();
        return true;
    }

    void AiInputboxWindow::resetInputArea() {
        SDL_Rect rect = {inputbox_r_.x, inputbox_r_.y, inputbox_r_.w, inputbox_r_.h};
        if (util::isWayland()) {
            auto r = parent_->getRect();
            auto m = getMonitorRect(r);
            rect.x += r.x - m.x;
            rect.y += r.y - m.y;
        }
        SDL_SetTextInputArea(getBackendWindow(), &rect, 0);
    }
}
