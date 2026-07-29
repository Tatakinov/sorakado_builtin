#ifndef SORAKADO_AI_SCRIPT_INPUTBOX_H_
#define SORAKADO_AI_SCRIPT_INPUTBOX_H_

#include "sorakado/ai/base_inputbox.h"

namespace sorakado::ai {
    class ScriptInputbox : public BaseInputbox {
        private:
            void activate(const std::string &text) override;
            void cancel(const std::string &reason) override;
        public:
            ScriptInputbox(sorakado::Sorakado *parent, std::unique_ptr<WindowManager> manager, const Rect &inputbox_r, const Color &color, std::unique_ptr<ImageCache> &image_cache, std::unique_ptr<WrapFont> &font) : BaseInputbox(parent, std::move(manager), inputbox_r, color, "balloonc1.png", image_cache, font) {}
            ~ScriptInputbox() {}
    };
}

#endif // SORAKADO_AI_SCRIPT_INPUTBOX_H_
