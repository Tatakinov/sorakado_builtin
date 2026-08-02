#include "./misc.h"
#include "sorakado/window.h"

#include "logger.h"
#include "sorakado/base_character.h"
#include "sorakado/render_info.h"

namespace sorakado {
    Window::Window(sorakado::BaseCharacter *parent, display_t id, std::unique_ptr<BackendWindowFactory> &factory, const std::string &name, int width, int height) : texture_offset_({0, 0}), shown_(false), redrawn_(false), focused_(false), texture_cache_(std::make_unique<TextureCache>()), parent_(parent), window_(nullptr), renderer_(nullptr) {
        if (util::isWayland() && id > 0) {
            SDL_Rect r;
            SDL_GetDisplayBounds(id, &r);
            monitor_rect_ = { r.x, r.y, r.w, r.h };
        }
        else {
            monitor_rect_ = { 0, 0, 1, 1 };
        }
        if (util::isWayland()) {
            if (width == 0 || height == 0) {
                int count = 0;
                auto *displays = SDL_GetDisplays(&count);
                for (int i = 0; i < count; i++) {
                    SDL_Rect r;
                    SDL_GetDisplayBounds(displays[i], &r);
                    if (width == 0 || width > r.w) {
                        width = r.w;
                    }
                    if (height == 0 || height > r.h) {
                        height = r.h;
                    }
                }
                SDL_free(displays);
            }
            monitor_rect_.w = width;
            monitor_rect_.h = height;
            window_ = factory->create(name.c_str(), width, height);
        }
        else {
            if (width == 0 || height == 0) {
                width = height = 200;
            }
            window_ = factory->create(name.c_str(), width, height);
        }
        renderer_ = SDL_CreateRenderer(window_, nullptr);
        SDL_SetRenderVSync(renderer_, 1);
#if defined(IS__NIX)
        if (util::isWayland()) {
            const wl_registry_listener listener = {
                .global = [](void *data, wl_registry *reg, uint32_t id, const char *interface, uint32_t version) {
                    Window *window = static_cast<Window *>(data);
                    std::string s = interface;
                    if (s == "wl_compositor") {
                        window->setCompositor(static_cast<wl_compositor *>(wl_registry_bind(reg, id, &wl_compositor_interface, 1)));
                    }
                },
                .global_remove = [](void *data, wl_registry *reg, uint32_t id) {
                }
            };
            wl_display *display = static_cast<wl_display *>(SDL_GetPointerProperty(SDL_GetWindowProperties(window_), SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr));
            reg_ = wl_display_get_registry(display);
            wl_registry_add_listener(reg_, &listener, this);
            wl_display_roundtrip(display);
        }
#endif // Linux/Unix
        parent_->resetPosition(true);
    }

    Window::~Window() {
        if (renderer_ != nullptr) {
            SDL_DestroyRenderer(renderer_);
        }
        if (window_ != nullptr) {
            SDL_DestroyWindow(window_);
        }
#if defined(IS__NIX)
        if (util::isWayland()) {
            if (reg_ != nullptr) {
                wl_registry_destroy(reg_);
            }
            if (compositor_ != nullptr) {
                wl_compositor_destroy(compositor_);
            }
        }
#endif // Linux/Unix
    }

#if defined(IS__NIX)
    void Window::setCompositor(wl_compositor *compositor) {
        compositor_ = compositor;
    }
#endif // Linux/Unix

    bool Window::swapBuffers() {
        bool ret = redrawn_;
        if (redrawn_) {
            redrawn_ = false;
            SDL_SetRenderTarget(renderer_, nullptr);
            SDL_RenderPresent(renderer_);
        }
        return ret;
    }

    void Window::show() {
        if (SDL_GetWindowFlags(window_) & SDL_WINDOW_HIDDEN) {
            SDL_ShowWindow(window_);
        }
        shown_ = true;
    }

    void Window::hide() {
        SDL_HideWindow(window_);
        shown_ = false;
    }

    Rect Window::getMonitorRect(const Rect &cr) const {
        if (util::isWayland()) {
            return monitor_rect_;
        }
        else {
            auto id = util::getNearestDisplay(cr.x + cr.w / 2, cr.y + cr.h / 2);
            SDL_Rect r;
            SDL_GetDisplayBounds(id, &r);
            return {r.x, r.y, r.w, r.h};
        }
    }

    double Window::distance(int x, int y) const {
        if (monitor_rect_.x <= x && monitor_rect_.x + monitor_rect_.w >= x &&
                monitor_rect_.y <= y && monitor_rect_.y + monitor_rect_.h >= y) {
            return 0;
        }
        if (monitor_rect_.x <= x && monitor_rect_.x + monitor_rect_.w >= x) {
            return std::min(std::abs(monitor_rect_.x - x), std::abs(monitor_rect_.x + monitor_rect_.w - x));
        }
        if (monitor_rect_.y <= y && monitor_rect_.y + monitor_rect_.h >= y) {
            return std::min(std::abs(monitor_rect_.y - y), std::abs(monitor_rect_.y + monitor_rect_.h - y));
        }
        double d;
        {
            int dx = monitor_rect_.x - x;
            int dy = monitor_rect_.y - y;
            d = sqrt(dx * dx + dy * dy);
        }
        {
            int dx = monitor_rect_.x + monitor_rect_.w - x;
            int dy = monitor_rect_.y - y;
            d = std::min(d, sqrt(dx * dx + dy * dy));
        }
        {
            int dx = monitor_rect_.x + monitor_rect_.w - x;
            int dy = monitor_rect_.y + monitor_rect_.h - y;
            d = std::min(d, sqrt(dx * dx + dy * dy));
        }
        {
            int dx = monitor_rect_.x - x;
            int dy = monitor_rect_.y + monitor_rect_.h - y;
            d = std::min(d, sqrt(dx * dx + dy * dy));
        }
        return d;
    }

    void Window::raise() {
        if (SDL_GetWindowFlags(window_) & SDL_WINDOW_HIDDEN) {
            SDL_RaiseWindow(window_);
        }
    }

    void Window::position(int x, int y) {
        if (!util::isWayland()) {
            SDL_SetWindowPosition(window_, x, y);
        }
    }

    void Window::resize(int w, int h) {
        if (!util::isWayland()) {
            SDL_SetWindowSize(window_, w, h);
        }
    }

    bool Window::focused() const {
        return focused_;
    }

    void Window::draw(std::unique_ptr<ImageCache> &image_cache, Position offset, const RenderInfo &render_info, region_t &region) {
        SDL_SetRenderTarget(renderer_, nullptr);
        SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer_);
        auto texture = render_info.getTexture(image_cache, renderer_, texture_cache_);
        // render next frame unless texture is up-converted
        if (!(texture && texture->isUpconverted())) {
            parent_->change();
        }
        if (texture) {
            Rect rect = {offset.x, offset.y, texture->width(), texture->height()};
            auto m = getMonitorRect(rect);
            SDL_SetRenderTarget(renderer_, nullptr);
            SDL_BlendMode mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD);
            SDL_SetTextureBlendMode(texture->texture(), mode);
            SDL_FRect r = { offset.x - m.x, offset.y - m.y, texture->width(), texture->height() };
            Logger::log("window.draw", r.x, r.y, r.w, r.h);
            SDL_RenderTexture(renderer_, texture->texture(), nullptr, &r);
        }
        if (region) {
            Rect rect = {offset.x, offset.y, region->width(), region->height()};
            auto m = getMonitorRect(rect);
            std::vector<int> shape;
#if defined(IS__NIX)
            bool is_wayland = util::isWayland();
            int x_begin = -1;
            wl_region *r = nullptr;
            if (is_wayland) {
                r = wl_compositor_create_region(compositor_);
            }
#endif // Linux/Unix
            {
                SDL_LockSurface(region->surface());
                for (int y = 0; y < region->height(); y++) {
                    for (int x = 0; x < region->width(); x++) {
                        unsigned char *p = static_cast<unsigned char *>(region->surface()->pixels);
                        int index = y * region->width() + x;
                        if (p[4 * index + 3]) {
#if !defined(IS__NIX)
                            shape.push_back(index);
#else
                            shape.push_back(offset.y * region->width() + offset.x + index);
                            if (x_begin == -1 && is_wayland) {
                                x_begin = x;
                            }
                        }
                        else {
                            if (x_begin != -1) {
                                wl_region_add(r, offset.x - m.x + x_begin, offset.y - m.y + y, x - x_begin, 1);
                                x_begin = -1;
                            }
#endif // Linux/Unix
                        }
                    }
#if defined(IS__NIX)
                    if (x_begin != -1 && is_wayland) {
                        wl_region_add(r, offset.x - m.x + x_begin, offset.y - m.y + y, region->width() - x_begin, 1);
                        x_begin = -1;
                    }
#endif // Linux/Unix
                }
                SDL_UnlockSurface(region->surface());
            }
            if (!shape_ || shape_ != shape) {
#if defined(IS__NIX)
                if (is_wayland) {
                    wl_surface *surface = static_cast<wl_surface *>(SDL_GetPointerProperty(SDL_GetWindowProperties(window_), SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr));
                    wl_surface_set_input_region(surface, r);
                }
                else
#endif // Linux/Unix
                {
                    SDL_SetWindowShape(window_, region->surface());
                }
                shape_ = shape;
            }
        }
        else if (!shape_ || shape_->size() > 0) {
            shape_ = std::make_optional<std::vector<int>>();
#if defined(IS__NIX)
            if (util::isWayland()) {
                wl_region *r = wl_compositor_create_region(compositor_);
                wl_surface *surface = static_cast<wl_surface *>(SDL_GetPointerProperty(SDL_GetWindowProperties(window_), SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr));
                wl_surface_set_input_region(surface, r);
            }
            else
#endif // Linux/Unix
            {
                int w, h;
                SDL_GetWindowSize(window_, &w, &h);
                auto s = std::make_unique<WrapSurface>(w, h);
                SDL_ClearSurface(s->surface(), 0, 0, 0, 0);
                SDL_SetWindowShape(window_, s->surface());
            }
        }
        redrawn();
    }

    void Window::clearCache() {
        texture_cache_->clear();
    }

    bool Window::key(window_id_t id, key_t key, bool down) {
        if (id != SDL_GetWindowID(window_)) {
            return false;
        }
        parent_->press(key, down);
        return true;
    }

    bool Window::input(window_id_t id, const std::string &text) {
        if (id != SDL_GetWindowID(window_)) {
            return false;
        }
        parent_->inputText(text);
        return true;
    }

    bool Window::edit(window_id_t id, const std::string &text) {
        if (id != SDL_GetWindowID(window_)) {
            return false;
        }
        parent_->editText(text);
        return true;
    }

    bool Window::drop(window_id_t id, const std::vector<std::string> &file_list) {
        if (id != SDL_GetWindowID(window_)) {
            return false;
        }
        parent_->dnd(file_list);
        return true;
    }

    bool Window::maximized(sorakado::window_id_t id) {
        if (id != SDL_GetWindowID(window_)) {
            return false;
        }
        Logger::log("window.maximized");
        if (util::isWayland()) {
            int w, h;
            SDL_GetWindowSize(window_, &w, &h);
            monitor_rect_.w = w;
            monitor_rect_.h = h;
            parent_->change();
            parent_->resetPosition(true);
        }
        return true;
    }

    bool Window::motion(sorakado::window_id_t id, float x, float y) {
        if (id != SDL_GetWindowID(window_)) {
            return false;
        }
        if (util::isWayland()) {
            x = x + monitor_rect_.x;
            y = y + monitor_rect_.y;
        }
        parent_->hover(x, y);
        return true;
    }

    bool Window::button(sorakado::window_id_t id, float x, float y, button_t button, bool down, click_t clicks) {
        Logger::log("window.button");
        if (id != SDL_GetWindowID(window_)) {
            return false;
        }
        if (util::isWayland()) {
            x = x + monitor_rect_.x;
            y = y + monitor_rect_.y;
        }
        Logger::log("window.click");
        parent_->click(this, x, y, button, down, clicks);
        return true;
    }

    bool Window::focus(sorakado::window_id_t id, bool focused) {
        if (id != SDL_GetWindowID(window_)) {
            return false;
        }
        focused_ = focused;
        return true;
    }

    void Window::redrawn() {
        redrawn_ = true;
    }
}
