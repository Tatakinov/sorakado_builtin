#ifndef SORAKADO_TEXTURE_H_
#define SORAKADO_TEXTURE_H_

#include <cassert>
#include <filesystem>
#include <memory>
#include <optional>
#include <unordered_map>

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>

namespace sorakado {
    class ImageInfo;
    class WrapSurface {
        private:
            SDL_Surface *surface_;
            bool is_upconverted_;
        public:
            WrapSurface(int w, int h);
            WrapSurface(ImageInfo &info);
            ~WrapSurface();
            SDL_Surface *surface() {
                return surface_;
            }
            int width() const {
                assert(surface_);
                return surface_->w;
            }
            int height() const {
                assert(surface_);
                return surface_->h;
            }
            bool isUpconverted() const {
                return is_upconverted_;
            }
    };

    class WrapTexture {
        private:
            SDL_Texture *texture_;
            bool is_upconverted_;
        public:
            WrapTexture(SDL_Renderer *renderer, int w, int h, bool is_upconverted);
            WrapTexture(SDL_Renderer *renderer, SDL_Surface *surface, bool is_upconverted);
            ~WrapTexture();
            SDL_Texture *texture() {
                return texture_;
            }
            int width() const {
                assert(texture_);
                return texture_->w;
            }
            int height() const {
                assert(texture_);
                return texture_->h;
            }
            bool isUpconverted() const {
                return is_upconverted_;
            }
    };
}

#endif // SORAKADO_TEXTURE_H_
