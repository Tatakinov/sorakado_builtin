#include "sorakado/ai/inputbox_info.h"

#include <numeric>

#include "sorakado/util.h"

namespace sorakado::ai {
    InputboxInfo::InputboxInfo(const Rect &inputbox_r, const Color &color, const std::filesystem::path &path, std::unique_ptr<ImageCache> &image_cache, std::unique_ptr<WrapFont> &font) : sorakado::RenderInfo(), inputbox_r_(inputbox_r), color_(color), path_(path), font_(font), cursor_index_(0) {
        if (inputbox_r_.x < 0) {
            inputbox_r_.x = 0;
        }
        if (inputbox_r_.y < 0) {
            inputbox_r_.y = 0;
        }
        auto &info = image_cache->get(path);
        if (info) {
            w_ = info->width();
            h_ = info->height();
        }
        else {
            w_ = inputbox_r_.x + 200;
            h_ = inputbox_r_.y + 20;
        }
        change();
    }

    std::string InputboxInfo::getText() const {
        return std::accumulate(input_.begin(), input_.end(), std::string());
    }

    void InputboxInfo::input(const std::string &text) {
        for (auto &s : util::UTF8Split(text)) {
            input_.insert(std::next(input_.begin(), cursor_index_++), s);
        }
        change();
    }
    
    void InputboxInfo::edit(const std::string &text) {
        edit_ = text;
        change();
    }

    void InputboxInfo::erase() {
        if (input_.size() > 0 && cursor_index_ > 0) {
            input_.erase(std::next(input_.begin(), --cursor_index_));
        }
        change();
    }

    void InputboxInfo::incrementCursorIndex() {
        cursor_index_ = std::min(cursor_index_ + 1, static_cast<int>(input_.size()));
        change();
    }
    
    void InputboxInfo::decrementCursorIndex() {
        cursor_index_ = std::max(cursor_index_ - 1, 0);
        change();
    }

    std::unique_ptr<WrapSurface> InputboxInfo::getSurface(std::unique_ptr<ImageCache> &cache) const {
        auto s = std::make_unique<WrapSurface>(w_, h_);
        SDL_ClearSurface(s->surface(), 1, 1, 1, 1);
        return s;
    }

    std::unique_ptr<WrapTexture> InputboxInfo::getTexture(std::unique_ptr<ImageCache> &image_cache, renderer_t *renderer, std::unique_ptr<TextureCache> &texture_cache) const {
        auto cursor_texture = std::make_unique<WrapTexture>(renderer, 1, 1, true);
        SDL_SetRenderTarget(renderer, cursor_texture->texture());
        SDL_SetRenderDrawColor(renderer, color_.r, color_.g, color_.b, color_.a);
        SDL_RenderClear(renderer);

        auto editing_texture = std::make_unique<WrapTexture>(renderer, 1, 1, true);
        SDL_SetRenderTarget(renderer, editing_texture->texture());
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(renderer);
        int width = 0;
        {
            std::string str;
            for (int i = 0; i < input_.size(); i++) {
                if (i >= cursor_index_) {
                    break;
                }
                str += input_[i];
            }
            if (str.length() > 0) {
                TTF_MeasureString(font_->font(), str.data(), str.length(), 0, &width, nullptr);
            }
        }
        auto dst = std::make_unique<WrapTexture>(renderer, w_, h_, true);
        SDL_SetRenderTarget(renderer, dst->texture());
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);
        ImagePath key = {path_, std::nullopt};
        auto &background = texture_cache->get(key, renderer, image_cache);
        SDL_RenderTexture(renderer, background->texture(), nullptr, nullptr);
        {
            SDL_FRect r = {inputbox_r_.x + width, inputbox_r_.y, 1, TTF_GetFontHeight(font_->font())};
            SDL_RenderTexture(renderer, cursor_texture->texture(), nullptr, &r);
        }
        if (!input_.empty()) {
            std::string str;
            for (auto &s : input_) {
                str += s;
            }
            SDL_Surface *surface = TTF_RenderText_Blended(font_->font(), str.data(), str.length(), {color_.r, color_.g, color_.b, color_.a});
            WrapTexture t(renderer, surface, true);
            SDL_FRect r = {inputbox_r_.x, inputbox_r_.y, surface->w, surface->h};
            SDL_RenderTexture(renderer, t.texture(), nullptr, &r);
            SDL_DestroySurface(surface);
        }
        if (!edit_.empty()) {
            SDL_Color c = {0x00, 0x00, 0x00, 0xff};
            SDL_Surface *surface = TTF_RenderText_Blended(font_->font(), edit_.data(), edit_.length(), c);
            WrapTexture t(renderer, surface, true);
            SDL_FRect r = {inputbox_r_.x + width, inputbox_r_.y, surface->w, surface->h};
            SDL_RenderTexture(renderer, editing_texture->texture(), nullptr, &r);
            SDL_RenderTexture(renderer, t.texture(), nullptr, &r);
            SDL_DestroySurface(surface);
        }
        SDL_SetRenderTarget(renderer, nullptr);
        return dst;
    }
}
