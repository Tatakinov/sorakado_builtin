#include "texture.h"

#include <cassert>

#include "sorakado/image_cache.h"
#include "logger.h"

namespace {
    std::unique_ptr<sorakado::WrapTexture> invalid_texture;
}

namespace sorakado {
    WrapSurface::WrapSurface(int w, int h) : is_upconverted_(false) {
        surface_ = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_ABGR8888);
    }

    WrapSurface::WrapSurface(ImageInfo &info) : is_upconverted_(info.isUpconverted()) {
        surface_ = SDL_CreateSurfaceFrom(info.width(), info.height(), SDL_PIXELFORMAT_ABGR8888, info.get().data(), info.width() * 4);
    }

    WrapSurface::~WrapSurface() {
        if (surface_ != nullptr) {
            SDL_DestroySurface(surface_);
        }
    }


    WrapTexture::WrapTexture(SDL_Renderer *renderer, int w, int h, bool is_upconverted) : is_upconverted_(is_upconverted) {
        texture_ = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, w, h);
    }

    WrapTexture::WrapTexture(SDL_Renderer *renderer, SDL_Surface *surface, bool is_upconverted) : is_upconverted_(is_upconverted) {
        texture_ = SDL_CreateTextureFromSurface(renderer, surface);
        {
            std::string err(SDL_GetError());
            if (!err.empty()) {
                Logger::log("TextureError", err);
                assert(false);
            }
        }
        assert(texture_);
    }

    WrapTexture::~WrapTexture() {
        if (texture_ != nullptr) {
            SDL_DestroyTexture(texture_);
        }
    }
}
