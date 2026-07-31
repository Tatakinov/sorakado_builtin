#include "element.h"

#include <cassert>

#include "logger.h"
#include "sorakado/ao/master/surface.h"

namespace {
    const int kInf = 1000000;
}

namespace sorakado::ao::master {
    bool ElementWithChildren::equals(const RenderInfo &r) const {
        const auto &lhs = *this;
        const auto &rhs = static_cast<const ElementWithChildren &>(r);
        if (!(lhs.method == rhs.method && lhs.x == rhs.x && lhs.y == rhs.y)) {
            return false;
        }
        if (rhs.children.size() != lhs.children.size()) {
            return false;
        }
        for (int i = 0; i < rhs.children.size(); i++) {
            if (lhs.children[i] != rhs.children[i]) {
                return false;
            }
        }
        return true;
    }

    Rect ElementWithChildren::getRect(std::unique_ptr<ImageCache> &image_cache) const {
        Rect r = {kInf, kInf, -kInf, -kInf};
        std::vector<std::optional<std::unique_ptr<WrapSurface>>> list;
        for (auto &element : children) {
            std::visit([&](const auto &e) {
                auto cr = e.getRect(image_cache);
                if (r.x > cr.x) {
                    r.x = cr.x;
                }
                if (r.y > cr.y) {
                    r.y = cr.y;
                }
                if (r.w < cr.x + cr.w) {
                    r.w = cr.x + cr.w;
                }
                if (r.h < cr.y + cr.h) {
                    r.h = cr.y + cr.h;
                }
            }, element);
        }
        r.w -= r.x;
        r.h -= r.y;
        r.x += x;
        r.y += y;
        return r;
    }

    std::unique_ptr<WrapSurface> ElementWithChildren::getSurface(std::unique_ptr<ImageCache> &image_cache) const {
        auto rect = getRect(image_cache);
        if (rect.w <= 0 || rect.h <= 0) {
            std::unique_ptr<WrapSurface> invalid;
            //Logger::log("no valid children");
            return invalid;
        }
        std::vector<std::optional<std::unique_ptr<WrapSurface>>> list;
        for (auto &element : children) {
            std::visit([&](const auto &e) {
                auto t = e.getSurface(image_cache);
                if (!t) {
                    list.push_back(std::nullopt);
                    return;
                }
                list.push_back(std::move(t));
            }, element);
        }
        auto surface = std::make_unique<WrapSurface>(rect.w, rect.h);
        SDL_ClearSurface(surface->surface(), 0, 0, 0, 0);
        for (int i = 0; i < list.size(); i++) {
            if (!list[i]) {
                continue;
            }
            std::visit([&](const auto &e) {
                auto &t = list[i].value();
                SDL_SetSurfaceBlendMode(t->surface(), SDL_BLENDMODE_BLEND);
                Logger::log("getSurface.rect", rect.x, rect.y, rect.w, rect.h);
                Logger::log("getSurface.e", e.x, e.y);
                SDL_Rect r = { e.x - rect.x + x, e.y - rect.y + y, t->width(), t->height() };
                assert(r.x >= 0);
                assert(r.y >= 0);
                SDL_BlitSurface(t->surface(), nullptr, surface->surface(), &r);
            }, children[i]);
        }
        return surface;
    }

    std::unique_ptr<WrapTexture> ElementWithChildren::getTexture(std::unique_ptr<ImageCache> &image_cache, renderer_t *renderer, std::unique_ptr<TextureCache> &texture_cache) const {
        auto rect = getRect(image_cache);
        if (rect.w <= 0 || rect.h <= 0) {
            std::unique_ptr<WrapTexture> invalid;
            //Logger::log("no valid children");
            return invalid;
        }
        bool upconverted = true;
        std::vector<std::optional<std::unique_ptr<WrapTexture>>> list;
        for (auto &element : children) {
            std::visit([&](const auto &e) {
                auto t = e.getTexture(image_cache, renderer, texture_cache);
                if (!t) {
                    list.push_back(std::nullopt);
                    return;
                }
                upconverted = upconverted && t->isUpconverted();
                list.push_back(std::move(t));
            }, element);
        }
        auto texture = std::make_unique<WrapTexture>(renderer, rect.w, rect.h, upconverted);
        SDL_SetRenderTarget(renderer, texture->texture());
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);
        for (int i = 0; i < children.size(); i++) {
            if (!list[i]) {
                continue;
            }
            SDL_BlendMode mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD);
            std::visit([&](const auto &e) {
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
                auto &t = list[i].value();
                SDL_SetTextureBlendMode(t->texture(), mode);
                SDL_FRect r = { e.x - rect.x + x, e.y - rect.y + y, t->width(), t->height() };
                assert(r.x >= 0);
                assert(r.y >= 0);
                SDL_RenderTexture(renderer, t->texture(), nullptr, &r);
            }, children[i]);
        }
        SDL_SetRenderTarget(renderer, nullptr);
        return texture;
    }
}
