#ifndef SORAKADO_FONT_H_
#define SORAKADO_FONT_H_

#include <filesystem>
#include <string>

#include <SDL3_ttf/SDL_ttf.h>

#include "libfontlist/include/fontlist.hpp"

namespace sorakado {
    class FontResizer {
        private:
            TTF_Font *font_;
            float old_size_;
        public:
            FontResizer(TTF_Font *font, int scale);
            ~FontResizer();
    };

    class FontStyleChanger {
        private:
            TTF_Font *font_;
            TTF_FontStyleFlags old_flags_;
        public:
            FontStyleChanger(TTF_Font *font, TTF_FontStyleFlags flags);
            ~FontStyleChanger();
    };

    class WrapFont {
        private:
            TTF_Font *font_;
            std::string name_;
            float size_;
        public:
            WrapFont(const fontlist::fontfamily &family);
            WrapFont(const std::filesystem::path &path);
            ~WrapFont();
            TTF_Font *font();
            std::string name() const {
                return name_;
            }
    };
}

#endif // SORAKADO_FONT_H_
