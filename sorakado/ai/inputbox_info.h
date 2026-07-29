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
            std::unique_ptr<WrapFont> &font_;
            int cursor_index_;
            std::vector<std::string> input_;
            std::string edit_;
        public:
            InputboxInfo(const Rect &inputbox_r, const Color &color, const std::filesystem::path &path, std::unique_ptr<ImageCache> &image_cache, std::unique_ptr<WrapFont> &font);

            void input(const std::string &text);
            void edit(const std::string &text);

            std::string getText() const;

            void erase();
            void incrementCursorIndex();
            void decrementCursorIndex();

            std::unique_ptr<WrapSurface> getSurface(std::unique_ptr<ImageCache> &cache) const override;
            std::unique_ptr<WrapTexture> getTexture(std::unique_ptr<ImageCache> &image_cache, renderer_t *renderer, std::unique_ptr<TextureCache> &texture_cache) const override;
            bool equals(const RenderInfo &r) const override {
                const auto &rhs = static_cast<const InputboxInfo &>(r);
                return input_ == rhs.input_ && edit_ == rhs.edit_;
            }
    };
}

#endif // SORAKADO_AI_INPUTBOX_INFO_H_
