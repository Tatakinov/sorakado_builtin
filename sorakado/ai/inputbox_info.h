#ifndef SORAKADO_AI_INPUTBOX_INFO_H_
#define SORAKADO_AI_INPUTBOX_INFO_H_

#include "sorakado/font.h"
#include "sorakado/misc.h"
#include "sorakado/render_info.h"

namespace sorakado::ai {
    class InputboxInfo : public sorakado::RenderInfo {
        private:
            Rect inputbox_r_;
            int w_, h_;
            Color color_;
            std::filesystem::path path_;
            WrapFont *font_;
            int cursor_index_;
            std::vector<std::string> input_;
            std::string edit_;
        public:
            InputboxInfo(const Rect &inputbox_r, const Color &color, const std::filesystem::path &path, ImageCache &image_cache, WrapFont *font);

            void input(const std::string &text);
            void edit(const std::string &text);

            std::string getText() const;

            void erase();
            void incrementCursorIndex();
            void decrementCursorIndex();

            Region getRegion() const override;
            std::unique_ptr<WrapSurface> getSurface() const override;
            std::unique_ptr<WrapTexture> getTexture(renderer_t *renderer, TextureCache &texture_cache) const override;
            bool equals(const RenderInfo &r) const override {
                const auto &rhs = static_cast<const InputboxInfo &>(r);
                return input_ == rhs.input_ && edit_ == rhs.edit_;
            }
    };
}

#endif // SORAKADO_AI_INPUTBOX_INFO_H_
