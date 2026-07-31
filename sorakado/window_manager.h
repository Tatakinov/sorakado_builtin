#ifndef SORAKADO_WINDOW_MANAGER_H_
#define SORAKADO_WINDOW_MANAGER_H_

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include <SDL3/SDL_video.h>

#include "sorakado/backend_window_factory.h"
#include "sorakado/compatible.h"
#include "sorakado/image_cache.h"
#include "sorakado/misc.h"
#include "sorakado/window_factory.h"

namespace sorakado {
    class BaseCharacter;
    class RenderInfo;

    class WindowManager {
        private:
        protected:
            std::unique_ptr<WindowFactory> factory_;
            std::unique_ptr<BackendWindowFactory> backend_factory_;
        public:
            WindowManager(std::unique_ptr<WindowFactory> factory, std::unique_ptr<BackendWindowFactory> backend_factory);
            virtual ~WindowManager();
            virtual void create(BaseCharacter *parent, display_t id, const std::string &name) = 0;
            virtual void destroy(display_t id) = 0;
            virtual void draw(std::unique_ptr<ImageCache> &image_cache, Position offset, const RenderInfo &render_info, region_t &region) = 0;
            virtual bool swapBuffers() = 0;

            virtual Rect getMonitorRect(const Rect &r) const = 0;

            virtual void show() = 0;
            virtual bool shown() const = 0;
            virtual void raise() = 0;
            virtual void hide() = 0;
            virtual void clearCache() = 0;

            virtual void position(int x, int y) = 0;
            virtual void resize(int w, int h) = 0;
            virtual void key(window_id_t id, key_t key, bool down) = 0;
            virtual void input(window_id_t id, const std::string &text) = 0;
            virtual void edit(window_id_t id, const std::string &text) = 0;
            virtual void motion(window_id_t id, float x, float y) = 0;
            virtual void button(window_id_t id, float x, float y, button_t button, bool down, Uint8 clicks) = 0;
            virtual void wheel(window_id_t id, float x, float y) = 0;
            virtual void drop(window_id_t id, const std::vector<std::string> &list) = 0;
            virtual void maximized(window_id_t id) = 0;
            virtual void focus(window_id_t id, bool focused) = 0;
            virtual bool focused() const = 0;
    };
}

#endif // SORAKADO_WINDOW_MANAGER_H_
