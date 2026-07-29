#include "sorakado/font.h"

#include <cassert>

#include "logger.h"

namespace sorakado {
    FontResizer::FontResizer(TTF_Font *font, int scale) : font_(font) {
        old_size_ = TTF_GetFontSize(font_);
        TTF_SetFontSize(font_, old_size_ * scale / 100.0f);
    }

    FontResizer::~FontResizer() {
        TTF_SetFontSize(font_, old_size_);
    }

    FontStyleChanger::FontStyleChanger(TTF_Font *font, TTF_FontStyleFlags flags) : font_(font) {
        old_flags_ = TTF_GetFontStyle(font_);
        TTF_SetFontStyle(font_, flags);
    }

    FontStyleChanger::~FontStyleChanger() {
        TTF_SetFontStyle(font_, old_flags_);
    }

    WrapFont::WrapFont(const fontlist::fontfamily &family) : name_(family.name) {
        fontlist::font font = family.fonts[0];
        int threshold = std::abs(400 - font.weight);
        for (auto &f : family.fonts) {
            if (f.style != fontlist::fontstyle::normal) {
                continue;
            }
            if (std::abs(400 - f.weight) < threshold) {
                threshold = std::abs(400 - f.weight);
                font = f;
            }
        }
        assert(font.size > 0);
        font_ = TTF_OpenFont(font.file.string().c_str(), font.size);
        //TTF_SetFontSizeDPI(font_, font.size, 96, 96);
        size_ = font.size;
    }

    WrapFont::WrapFont(const std::filesystem::path &path) : name_("") {
        // FIXME font pt size
        font_ = TTF_OpenFont(path.string().c_str(), 12);
        size_ = 12;
    }

    WrapFont::~WrapFont() {
        if (font_ != nullptr) {
            TTF_CloseFont(font_);
        }
    }

    TTF_Font *WrapFont::font() {
        assert(font_ != nullptr);
        return font_;
    }
}
