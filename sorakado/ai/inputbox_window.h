#ifndef SORAKADO_AI_INPUTBOX_WINDOW_H_
#define SORAKADO_AI_INPUTBOX_WINDOW_H_

#include "sorakado/misc.h"

#include <memory>

#include "sorakado/backend_window_factory.h"
#include "sorakado/base_character.h"
#include "sorakado/font.h"
#include "sorakado/window.h"
#include "sorakado/window_factory.h"

namespace sorakado::ai {
    class AiInputboxWindowFactory : public WindowFactory {
        private:
            Rect inputbox_r_;
            std::unique_ptr<WrapFont> &font_;
        public:
            AiInputboxWindowFactory(const Rect &inputbox_r, std::unique_ptr<WrapFont> &font);

            std::unique_ptr<sorakado::Window> create(sorakado::BaseCharacter *parent, display_t id, std::unique_ptr<BackendWindowFactory> &factory, const std::string &name) const override;
    };

    class AiInputboxWindow : public sorakado::Window {
        private:
            const Rect inputbox_r_;
            std::unique_ptr<WrapFont> &font_;
        public:
            AiInputboxWindow(sorakado::BaseCharacter *parent, display_t id, std::unique_ptr<BackendWindowFactory> &factory, const std::string &name, const Rect &inputbox_r, std::unique_ptr<WrapFont> &font);
            ~AiInputboxWindow() {}

            bool wheel(sorakado::window_id_t id, float x, float y) override {
                return false;
            }
            bool maximized(window_id_t id) override;
            bool motion(window_id_t id, float x, float y) override;
            bool button(window_id_t id, float x, float y, button_t button, bool down, click_t clicks) override;

            void resetInputArea();
    };
}

#endif // SORAKADO_AI_INPUTBOX_WINDOW_H_
