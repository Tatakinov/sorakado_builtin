#include "sorakado/single_window_manager.h"

#include "sorakado/window.h"
#include "sorakado/window_factory.h"

namespace sorakado {
    SingleWindowManager::SingleWindowManager(std::unique_ptr<WindowFactory> factory, std::unique_ptr<BackendWindowFactory> backend_factory) : WindowManager(std::move(factory), std::move(backend_factory)) {
    }

    SingleWindowManager::~SingleWindowManager() {
    }

    void SingleWindowManager::create(BaseCharacter *parent, display_t id, const std::string &name) {
        if (!window_) {
            window_ = factory_->create(parent, id, backend_factory_, name);
        }
    }

    void SingleWindowManager::destroy(display_t id) {
        window_.reset();
    }

    void SingleWindowManager::draw(std::unique_ptr<ImageCache> &image_cache, Position offset, const RenderInfo &render_info, region_t &region) {
        if (!window_) {
            return;
        }
        window_->draw(image_cache, offset, render_info, region);
    }

    bool SingleWindowManager::swapBuffers() {
        if (!window_) {
            return false;
        }
        return window_->swapBuffers();
    }
    
    Rect SingleWindowManager::getMonitorRect(const Rect &r) const {
        if (!window_) {
            return {0, 0, 0, 0};
        }
        return window_->getMonitorRect(r);
    }

    void SingleWindowManager::show() {
        if (!window_) {
            return;
        }
        window_->show();
    }

    bool SingleWindowManager::shown() const {
        if (!window_) {
            return false;
        }
        return window_->shown();
    }

    void SingleWindowManager::raise() {
        if (!window_) {
            return;
        }
        window_->raise();
    }

    void SingleWindowManager::hide() {
        if (!window_) {
            return;
        }
        window_->hide();
    }

    void SingleWindowManager::clearCache() {
        if (!window_) {
            return;
        }
        window_->clearCache();
    }

    void SingleWindowManager::position(int x, int y) {
        if (!window_) {
            return;
        }
        window_->position(x, y);
    }

    void SingleWindowManager::resize(int w, int h) {
        if (!window_) {
            return;
        }
        window_->resize(w, h);
    }

    void SingleWindowManager::key(window_id_t id, key_t key, bool down) {
        if (!window_) {
            return;
        }
        window_->key(id, key, down);
    }

    void SingleWindowManager::input(window_id_t id, const std::string &text) {
        if (!window_) {
            return;
        }
        window_->input(id, text);
    }

    void SingleWindowManager::edit(window_id_t id, const std::string &text) {
        if (!window_) {
            return;
        }
        window_->edit(id, text);
    }

    void SingleWindowManager::motion(window_id_t id, float x, float y) {
        if (!window_) {
            return;
        }
        window_->motion(id, x, y);
    }

    void SingleWindowManager::button(window_id_t id, float x, float y, button_t button, bool down, Uint8 clicks) {
        if (!window_) {
            return;
        }
        window_->button(id, x, y, button, down, clicks);
    }
    
    void SingleWindowManager::wheel(window_id_t id, float x, float y) {
        if (!window_) {
            return;
        }
        window_->wheel(id, x, y);
    }

    void SingleWindowManager::drop(window_id_t id, const std::vector<std::string> &list) {
        if (!window_) {
            return;
        }
        window_->drop(id, list);
    }

    void SingleWindowManager::maximized(window_id_t id) {
        if (!window_) {
            return;
        }
        window_->maximized(id);
    }

    void SingleWindowManager::focus(window_id_t id, bool focused) {
        if (!window_) {
            return;
        }
        window_->focus(id, focused);
    }

    bool SingleWindowManager::focused() const {
        if (!window_) {
            return false;
        }
        return window_->focused();
    }
}
