#include "sorakado/ao/master/surface.h"

#include "sorakado/image_cache.h"
#include "logger.h"

namespace sorakado::ao::master {

    bool Element::operator==(const Element &rhs) const {
        const auto &lhs = *this;
        return lhs.method == rhs.method && lhs.x == rhs.x && lhs.y == rhs.y && lhs.filename == rhs.filename && lhs.index == rhs.index;
    }

    WrapTexture *Element::getTexture(std::unique_ptr<ImageCache> &image_cache, renderer_t *renderer, std::unique_ptr<TextureCache> &texture_cache) const {
        auto data = texture_cache->get({filename, index}, renderer, image_cache);
        if (!data) {
            return nullptr;
        }
        return data.value();
    }

    Rect Element::getRect(std::unique_ptr<ImageCache> &image_cache, bool include_empty_image) const {
        auto &info = image_cache->get(filename, index);
        if (!info) {
            return {0, 0, 0, 0};
        }
        if (!include_empty_image && info->empty()) {
            return {0, 0, 0, 0};
        }
        return {x, y, info->width(), info->height()};
    }

    Region Element::getRegion(std::unique_ptr<ImageCache> &image_cache) const {
        auto &info = image_cache->get(filename, index);
        if (!info) {
            return {};
        }
        return info->region();
    }

    std::unique_ptr<WrapSurface> Element::getSurface(std::unique_ptr<ImageCache> &image_cache) const {
        auto &info = image_cache->get(filename, index);
        if (!info) {
            Logger::log("invalid info");
            std::unique_ptr<WrapSurface> invalid;
            return invalid;
        }
        WrapSurface src(info.value());
        auto dst = std::make_unique<WrapSurface>(x + src.width(), y + src.height());
        SDL_ClearSurface(dst->surface(), 0, 0, 0, 0);
        SDL_SetSurfaceBlendMode(src.surface(), SDL_BLENDMODE_BLEND);
        SDL_Rect r = { x, y, src.width(), src.height() };
        SDL_BlitSurface(src.surface(), nullptr, dst->surface(), &r);
        return dst;
    }
}
