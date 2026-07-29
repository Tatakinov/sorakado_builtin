#ifndef SORAKADO_AI_INPUTBOX_H_
#define SORAKADO_AI_INPUTBOX_H_

#include "sorakado/ai/base_inputbox.h"

namespace sorakado::ai {
    class Inputbox : public BaseInputbox {
        private:
            std::string id_;

            void activate(const std::string &text) override;
            void cancel(const std::string &reason) override;
        public:
            Inputbox(sorakado::Sorakado *parent, std::unique_ptr<WindowManager> manager, const Rect &inputbox_r, const Color &color, std::unique_ptr<ImageCache> &image_cache, std::unique_ptr<WrapFont> &font, const std::string &id) : BaseInputbox(parent, std::move(manager), inputbox_r, color, "balloonc0.png", image_cache, font), id_(id) {}
            ~Inputbox() {}
    };
}

#endif // SORAKADO_AI_INPUTBOX_H_
