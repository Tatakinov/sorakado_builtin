#ifndef SORAKADO_SINGLE_WINDOW_MANAGER_H_
#define SORAKADO_SINGLE_WINDOW_MANAGER_H_

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include <SDL3/SDL_video.h>

#include "sorakado/compatible.h"
#include "sorakado/misc.h"
#include "sorakado/window_manager.h"

namespace sorakado {
    class BaseCharacter;
    class Sorakado;
    class Window;
    class WindowFactory;

    class SingleWindowManager : public WindowManager {
        private:
            std::unique_ptr<Window> window_;
        public:
            SingleWindowManager(std::unique_ptr<WindowFactory> factory, std::unique_ptr<BackendWindowFactory> backend_factory);
            ~SingleWindowManager();

            void create(BaseCharacter *parent, display_t id, const std::string &name) override;
            void destroy(display_t id) override;
            void draw(std::unique_ptr<ImageCache> &image_cache, Position offset, const RenderInfo &render_info, region_t &region) override;
            bool swapBuffers() override;

            Rect getMonitorRect(const Rect &r) const override;

            void show() override;
            bool shown() const override;
            void raise() override;
            void hide() override;
            void clearCache() override;

            void position(int x, int y) override;
            void resize(int w, int h) override;
            void key(window_id_t id, key_t key, bool down) override;
            void input(window_id_t id, const std::string &text) override;
            void edit(window_id_t id, const std::string &text) override;
            void motion(window_id_t id, float x, float y) override;
            void button(window_id_t id, float x, float y, button_t button, bool down, Uint8 clicks) override;
            void wheel(window_id_t id, float x, float y) override;
            void drop(window_id_t id, const std::vector<std::string> &list) override;
            void maximized(window_id_t id) override;
            void focus(window_id_t id, bool focused) override;
            bool focused() const override;
    };
}

#endif // SORAKADO_SINGLE_WINDOW_MANAGER_H_
