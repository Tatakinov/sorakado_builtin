#include "sorakado/ao/master/texture_cache.h"

#include <cassert>

namespace sorakado::ao::master {
    WrapTexture *TextureCache::get(const ElementWithChildren &key, renderer_t *renderer) {
        if (map_.contains(key)) {
            return map_.at(key).get();
        }
        auto rect = key.getRect();
        if (rect.w <= 0 || rect.h <= 0) {
            return nullptr;
        }
        bool upconverted = true;
        std::vector<WrapTexture *> list;
        for (auto &element : key.children) {
            if (std::holds_alternative<ElementWithNoChildren>(element)) {
                auto e = std::get<ElementWithNoChildren>(element);
                auto *t = e.getTexture(renderer, texture_cache_);
                if (t) {
                    upconverted = upconverted && t->isUpconverted();
                }
                list.push_back(std::move(t));
            }
            else if (std::holds_alternative<ElementWithChildren>(element)) {
                auto e = std::get<ElementWithChildren>(element);
                auto *t = get(e, renderer);
                if (t) {
                    upconverted = upconverted && t->isUpconverted();
                }
                list.push_back(std::move(t));
            }
            else {
                assert(false);
            }
        }
        auto texture = std::make_unique<WrapTexture>(renderer, rect.w, rect.h, upconverted);
        SDL_SetRenderTarget(renderer, texture->texture());
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);
        for (int i = 0; i < key.children.size(); i++) {
            SDL_BlendMode mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD);
            std::visit([&](const auto &e) {
                auto *t = list[i];
                if (!t) {
                    return;
                }
                SDL_FRect r = { e.x - rect.x + key.x, e.y - rect.y + key.y, t->width(), t->height() };
                if (r.x + r.w < 0 || r.x > rect.w || r.y + r.h < 0 || r.y > rect.h) {
                    return;
                }
                switch (e.method) {
                    case Method::Base:
                    case Method::Add:
                    case Method::Overlay:
                        break;
                    case Method::OverlayFast:
                        mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_DST_ALPHA, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD);
                        break;
                    case Method::OverlayMultiply:
                        // FIXME
                        mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_SRC_COLOR, SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD);
                        break;
                    case Method::Replace:
                        mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ZERO, SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD);
                        break;
                    case Method::Interpolate:
                        mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE_MINUS_DST_ALPHA, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD);
                        break;
                    case Method::Reduce:
                        // FIXME
                        mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD);
                        break;
                    default:
                        break;
                }
                SDL_SetTextureBlendMode(t->texture(), mode);
                SDL_RenderTexture(renderer, t->texture(), nullptr, &r);
            }, key.children[i]);
        }
        SDL_SetRenderTarget(renderer, nullptr);
        map_[key] = std::move(texture);
        return map_.at(key).get();
    }
}
