#ifndef SORAKADO_WINDOW_H_
#define SORAKADO_WINDOW_H_

#include "os_preprocess.h"

#include <cmath>
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#if defined(IS__NIX)
#include <wayland-client.h>
#endif // Linux/Unix

#include "sorakado/backend_window_factory.h"
#include "sorakado/compatible.h"
#include "sorakado/texture.h"
#include "sorakado/texture_cache.h"
#include "sorakado/util.h"

namespace sorakado {

    class BaseCharacter;
    class ImageCache;
    class RenderInfo;

    class Window {
        private:
            display_t id_;
            Position texture_offset_;
            bool shown_;
            bool redrawn_;
            bool focused_;
            std::optional<std::vector<int>> shape_;
            std::unique_ptr<TextureCache> texture_cache_;
#if defined(IS__NIX)
            wl_registry *reg_;
            wl_compositor *compositor_;
#endif // Linux/Unix

        protected:
            sorakado::BaseCharacter *parent_;
            SDL_Window *window_;
            SDL_Renderer *renderer_;
            Rect monitor_rect_;

        public:
            Window(sorakado::BaseCharacter *parent, display_t id, std::unique_ptr<BackendWindowFactory> &factory, const std::string &name, int width = 0, int height = 0);

            virtual ~Window();

#if defined(IS__NIX)
            void setCompositor(wl_compositor *compositor);
#endif // Linux/Unix

            window_t getBackendWindow() const {
                return window_;
            }

            bool swapBuffers();

            void show();
            bool shown() const {
                return shown_;
            }
            void hide();

            Rect getMonitorRect(const Rect &cr) const;

            double distance(int x, int y) const;

            void raise();

            virtual void position(int x, int y);
            virtual bool key(window_id_t id, key_t key, bool down);

            virtual void clearCache();
            virtual bool input(window_id_t id, const std::string &text);
            virtual bool edit(window_id_t id, const std::string &text);
            virtual bool wheel(window_id_t id, float x, float y) = 0;

            virtual bool drop(window_id_t id, const std::vector<std::string> &list);
            virtual bool maximized(window_id_t id);
            virtual bool motion(window_id_t id, float x, float y);
            virtual bool button(window_id_t id, float x, float y, button_t button, bool down, Uint8 clicks);
            bool focus(window_id_t id, bool focused);
            bool focused() const;

            void draw(std::unique_ptr<ImageCache> &image_cache, Position offset, const RenderInfo &render_info, region_t &region);
            void redrawn();
    };
}

#endif // SORAKADO_WINDOW_H_
