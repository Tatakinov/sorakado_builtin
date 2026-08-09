#include "texture_cache.h"

namespace sorakado {
    std::optional<WrapTexture *> TextureCache::get(const texture_cache_t &key, renderer_t *renderer, std::unique_ptr<ImageCache> &image_cache) {
        if (!map_.contains(key) || (map_.at(key) && !map_.at(key)->isUpconverted())) {
            auto image = image_cache->get(key.path, key.index);
            if (image) {
                auto surface = std::make_unique<WrapSurface>(image.value());
                auto src = std::make_unique<WrapTexture>(renderer, surface->surface(), surface->isUpconverted());
                auto dst = std::make_unique<WrapTexture>(renderer, src->width(), src->height(), src->isUpconverted());
                SDL_BlendMode mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD);
                switch (key.type) {
                    case RenderType::PreMultiplied:
                        // nop
                        break;
                    // TODO others
                    default:
                        break;
                }
                SDL_SetRenderTarget(renderer, dst->texture());
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
                SDL_RenderClear(renderer);
                SDL_SetTextureBlendMode(src->texture(), mode);
                SDL_RenderTexture(renderer, src->texture(), nullptr, nullptr);
                SDL_SetRenderTarget(renderer, nullptr);
                map_.insert_or_assign(key, std::move(dst));
            }
            else {
                std::unique_ptr<WrapTexture> invalid;
                map_.emplace(key, std::move(invalid));
            }
        }
        auto &data = map_.at(key);
        if (data) {
            return data.get();
        }
        else {
            return std::nullopt;
        }
    }
}
