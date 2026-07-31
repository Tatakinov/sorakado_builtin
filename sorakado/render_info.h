#ifndef SORAKADO_RENDER_INFO_H_
#define SORAKADO_RENDER_INFO_H_

#include "sorakado/compatible.h"
#include "sorakado/image_cache.h"
#include "sorakado/misc.h"
#include "sorakado/texture.h"
#include "sorakado/texture_cache.h"
#include "sorakado/watcher.h"

namespace sorakado {
    class RenderInfo : public Watcher {
        public:
            RenderInfo() : Watcher() {}
            virtual ~RenderInfo() {}
            virtual Rect getRect(std::unique_ptr<ImageCache> &image_cache) const {
                return {0, 0, 0, 0};
            }
            virtual std::unique_ptr<WrapSurface> getSurface(std::unique_ptr<ImageCache> &cache) const = 0;
            virtual std::unique_ptr<WrapTexture> getTexture(std::unique_ptr<ImageCache> &image_cache, renderer_t *renderer, std::unique_ptr<TextureCache> &texture_cache) const = 0;
            virtual bool equals(const RenderInfo &rhs) const = 0;
            friend bool operator==(const RenderInfo &lhs, const RenderInfo &rhs) {
                return typeid(lhs) == typeid(rhs) && lhs.equals(rhs);
            }
    };
}

#endif // SORAKADO_RENDER_INFO_H_
