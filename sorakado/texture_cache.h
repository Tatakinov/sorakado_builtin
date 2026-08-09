#ifndef SORAKADO_TEXTURE_CACHE_H_
#define SORAKADO_TEXTURE_CACHE_H_

#include "sorakado/compatible.h"
#include "sorakado/image_cache.h"
#include "sorakado/texture.h"

namespace sorakado {
    enum class RenderType {
        PreMultiplied, Raw
    };

    struct texture_cache_t {
        std::filesystem::path path;
        std::optional<int> index;
        RenderType type;
        bool operator==(const texture_cache_t &rhs) const {
            return path == rhs.path && index == rhs.index && type == rhs.type;
        }
    };
}

template<>
struct std::hash<sorakado::texture_cache_t> {
    size_t operator ()(const sorakado::texture_cache_t &c) const {
        size_t hash = std::hash<int>()(static_cast<int>(c.type));
        if (c.index) {
            hash ^= c.index.value();
            hash = std::hash<size_t>()(hash);
        }
        hash ^= std::hash<std::filesystem::path>()(c.path);
        return hash;
    }
};


namespace sorakado {
    class TextureCache {
        private:
            std::unordered_map<texture_cache_t, std::unique_ptr<WrapTexture>> map_;
        public:
            TextureCache() {}
            ~TextureCache() {}
            std::optional<WrapTexture *> get(const texture_cache_t &key, renderer_t *renderer, std::unique_ptr<ImageCache> &image_cache);
            void clear() {
                map_.clear();
            }
    };
}

#endif // SORAKADO_TEXTURE_CACHE_H_
