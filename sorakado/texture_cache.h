#ifndef SORAKADO_TEXTURE_CACHE_H_
#define SORAKADO_TEXTURE_CACHE_H_

#include "sorakado/compatible.h"
#include "sorakado/image_cache.h"
#include "sorakado/texture.h"

namespace sorakado {
    class TextureCache {
        private:
            std::unordered_map<ImagePath, std::unique_ptr<WrapTexture>> map_;
        public:
            TextureCache() {}
            ~TextureCache() {}
            std::unique_ptr<WrapTexture> &get(const ImagePath &key, renderer_t *renderer, std::unique_ptr<ImageCache> &image_cache);
            void clear() {
                map_.clear();
            }
    };
}

#endif // SORAKADO_TEXTURE_CACHE_H_
