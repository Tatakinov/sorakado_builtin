#include "texture_cache.h"

namespace sorakado {
    std::unique_ptr<WrapTexture> &TextureCache::get(const ImagePath &key, renderer_t *renderer, std::unique_ptr<ImageCache> &image_cache) {
        if (!map_.contains(key) || (map_.at(key) && !map_.at(key)->isUpconverted())) {
            auto image = image_cache->get(key.path, key.index);
            if (image) {
                auto surface = std::make_unique<WrapSurface>(image.value());
                map_.insert_or_assign(key, std::make_unique<WrapTexture>(renderer, surface->surface(), surface->isUpconverted()));
            }
            else {
                std::unique_ptr<WrapTexture> invalid;
                map_.emplace(key, std::move(invalid));
            }
        }
        return map_.at(key);
    }
}
