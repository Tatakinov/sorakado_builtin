#ifndef SORAKADO_AO_MASTER_TEXTURE_CACHE_H_
#define SORAKADO_AO_MASTER_TEXTURE_CACHE_H_

#include "sorakado/texture_cache.h"
#include "sorakado/ao/master/element.h"

namespace sorakado::ao::master {
    class TextureCache {
        private:
            sorakado::TextureCache &texture_cache_;
            std::unordered_map<VElement, std::unique_ptr<WrapTexture>> map_;
        public:
            TextureCache(sorakado::TextureCache &texture_cache) : texture_cache_(texture_cache) {}
            ~TextureCache() {}
            WrapTexture *get(const ElementWithChildren &key, renderer_t *renderer);
    };
}

#endif // SORAKADO_AO_MASTER_TEXTURE_CACHE_H_
