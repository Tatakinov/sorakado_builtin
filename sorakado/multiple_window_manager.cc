#include "sorakado/multiple_window_manager.h"

#include "sorakado/window.h"
#include "sorakado/window_factory.h"

namespace sorakado {
    MultipleWindowManager::MultipleWindowManager(std::unique_ptr<WindowFactory> factory, std::unique_ptr<BackendWindowFactory> backend_factory) : WindowManager(std::move(factory), std::move(backend_factory)) {
    }

    MultipleWindowManager::~MultipleWindowManager() {
    }

    void MultipleWindowManager::create(BaseCharacter *parent, display_t id, const std::string &name) {
        windows_.try_emplace(id, factory_->create(parent, id, backend_factory_, name));
    }

    void MultipleWindowManager::destroy(display_t id) {
        if (windows_.contains(id)) {
            windows_.erase(id);
        }
    }

    void MultipleWindowManager::draw(std::unique_ptr<ImageCache> &image_cache, Position offset, const RenderInfo &render_info, region_t &region) {
        for (auto &[_, window] : windows_) {
            window->draw(image_cache, offset, render_info, region);
        }
    }

    bool MultipleWindowManager::swapBuffers() {
        bool redrawn = false;
        for (auto &[_, window] : windows_) {
            redrawn = window->swapBuffers() || redrawn;
        }
        return redrawn;
    }

    Rect MultipleWindowManager::getMonitorRect(const Rect &r) const {
        for (auto &[_, window] : windows_) {
            return window->getMonitorRect(r);
        }
        return {0, 0, 0, 0};
    }

    void MultipleWindowManager::show() {
        for (auto &[_, window] : windows_) {
            window->show();
        }
    }

    bool MultipleWindowManager::shown() const {
        for (auto &[_, window] : windows_) {
            if (window->shown()) {
                return true;
            }
        }
        return false;
    }

    void MultipleWindowManager::raise() {
        for (auto &[_, window] : windows_) {
            window->raise();
        }
    }

    void MultipleWindowManager::hide() {
        for (auto &[_, window] : windows_) {
            window->hide();
        }
    }

    void MultipleWindowManager::clearCache() {
        for (auto &[_, window] : windows_) {
            window->clearCache();
        }
    }

    void MultipleWindowManager::position(int x, int y) {
        for (auto &[_, window] : windows_) {
            window->position(x, y);
        }
    }

    void MultipleWindowManager::resize(int w, int h) {
        for (auto &[_, window] : windows_) {
            window->position(w, h);
        }
    }

    void MultipleWindowManager::key(window_id_t id, key_t key, bool down) {
        for (auto &[_, window] : windows_) {
            window->key(id, key, down);
        }
    }

    void MultipleWindowManager::input(window_id_t id, const std::string &text) {
        for (auto &[_, window] : windows_) {
            window->input(id, text);
        }
    }

    void MultipleWindowManager::edit(window_id_t id, const std::string &text) {
        for (auto &[_, window] : windows_) {
            window->edit(id, text);
        }
    }

    void MultipleWindowManager::motion(window_id_t id, float x, float y) {
        for (auto &[_, window] : windows_) {
            window->motion(id, x, y);
        }
    }

    void MultipleWindowManager::button(window_id_t id, float x, float y, button_t button, bool down, Uint8 clicks) {
        for (auto &[_, window] : windows_) {
            window->button(id, x, y, button, down, clicks);
        }
    }
    
    void MultipleWindowManager::wheel(window_id_t id, float x, float y) {
        for (auto &[_, window] : windows_) {
            window->wheel(id, x, y);
        }
    }

    void MultipleWindowManager::drop(window_id_t id, const std::vector<std::string> &list) {
        for (auto &[_, window] : windows_) {
            window->drop(id, list);
        }
    }

    void MultipleWindowManager::maximized(window_id_t id) {
        for (auto &[_, window] : windows_) {
            window->maximized(id);
        }
    }

    void MultipleWindowManager::focus(window_id_t id, bool focused) {
        for (auto &[_, window] : windows_) {
            window->focus(id, focused);
        }
    }

    bool MultipleWindowManager::focused() const {
        for (auto &[_, window] : windows_) {
            if (window->focused()) {
                return true;
            }
        }
        return false;
    }
}
