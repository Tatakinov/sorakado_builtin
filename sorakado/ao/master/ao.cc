#include "sorakado/ao/master/ao.h"
#include "os_preprocess.h"

#include "libfontlist/include/fontlist.hpp"

#include "sorakado/default_backend_window_factory.h"
#include "sorakado/popup_backend_window_factory.h"
#include "sorakado/multiple_window_manager.h"
#include "sorakado/single_window_manager.h"
#include "sorakado/ao/context_menu.h"
#include "sorakado/ao/master/window.h"

#include "logger.h"
#include <chrono>

namespace sorakado::ao::master {
    Ao::Ao(Application *parent, std::filesystem::path dir) : sorakado::ao::Ao(parent, dir), menu_initializer_({nullptr, -1, {0, 0}, {0, 0, 0, 0}}) {
        for (auto &[key, value] : descript_info_) {
            std::string tmp, group, name, category, part;
            int side = -1, id = -1;
            {
                std::istringstream iss(key);
                std::getline(iss, tmp, '.');
                if (tmp == "sakura") {
                    side = 0;
                }
                else if (tmp == "kero") {
                    side = 1;
                }
                else if (tmp.starts_with("char")) {
                    util::to_x(tmp.substr(4), side);
                }
                else {
                    continue;
                }
                std::getline(iss, tmp, '.');
                if (!tmp.starts_with("bindgroup")) {
                    continue;
                }
                util::to_x(tmp.substr(9), id);
                std::getline(iss, tmp, '.');
                if (tmp != "name") {
                    continue;
                }
            }
            {
                std::istringstream iss(value);
                std::getline(iss, category, ',');
                if (category.empty()) {
                    continue;
                }
                std::getline(iss, part, ',');
                if (part.empty()) {
                    continue;
                }
            }
            auto bind_key = category + "," + part;
            bind_id_[side][bind_key] = id;
        }
#ifdef IS_WINDOWS
        std::wstring exe_path;
        exe_path.resize(1024);
        GetModuleFileName(nullptr, exe_path.data(), 1023);
#else
        std::string exe_path;
        exe_path.resize(1024);
        readlink("/proc/self/exe", exe_path.data(), 1023);
#endif // OS
        std::filesystem::path exe_dir = exe_path;
        exe_dir = exe_dir.parent_path();

        image_cache_ = std::make_unique<ImageCache>(dir, exe_dir, getInfo("seriko.use_self_alpha", false) == "1");
        surfaces_ = std::make_unique<Surfaces>(dir);
        font_cache_ = std::make_unique<FontCache>();
        auto family = fontlist::get_default_font();
        font_cache_->setDefaultFont(family);
    }

    void Ao::create(int side) {
        auto window_factory = std::make_unique<AoMasterWindowFactory>();
        auto s = util::side2str(side);
        auto name = getInfo(s + ".name", true);
        if (util::isWayland() && getenv("NINIX_ENABLE_MULTI_MONITOR")) {
            auto backend_window_factory = std::make_unique<DefaultBackendWindowFactory>(SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
            auto manager = std::make_unique<MultipleWindowManager>(std::move(window_factory), std::move(backend_window_factory));
            characters_.try_emplace(side, std::make_unique<Character>(this, std::move(manager), side, name, surfaces_->getSeriko()));
            int count = 0;
            auto *monitors = SDL_GetDisplays(&count);
            for (int i = 0; i < count; i++) {
                characters_.at(side)->create(monitors[i]);
            }
            SDL_free(monitors);
        }
        else if (util::isWayland()) {
            auto backend_window_factory = std::make_unique<DefaultBackendWindowFactory>(SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
            auto manager = std::make_unique<SingleWindowManager>(std::move(window_factory), std::move(backend_window_factory));
            characters_.try_emplace(side, std::make_unique<Character>(this, std::move(manager), side, name, surfaces_->getSeriko()));
            characters_.at(side)->create(0);
        }
        else {
            auto backend_window_factory = std::make_unique<DefaultBackendWindowFactory>(SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_HIDDEN);
            auto manager = std::make_unique<SingleWindowManager>(std::move(window_factory), std::move(backend_window_factory));
            characters_.try_emplace(side, std::make_unique<Character>(this, std::move(manager), side, name, surfaces_->getSeriko()));
            characters_.at(side)->create(0);
        }
        return;
    }

    void Ao::show(int side) {
        if (!characters_.contains(side)) {
            return;
        }
        characters_.at(side)->show();
    }

    void Ao::hide(int side) {
        if (!characters_.contains(side)) {
            return;
        }
        characters_.at(side)->hide();
    }

    void Ao::setScale(int scale) {
        image_cache_->setScale(scale);
        for (auto &[_, v] : characters_) {
            v->setScale(scale);
        }
    }

    void Ao::display(const display_t display, const bool added) {
        if (added) {
            for (auto &[_, v] : characters_) {
                v->create(display);
            }
        }
        else {
            for (auto &[_, v] : characters_) {
                v->destroy(display);
            }
        }
    }

    void Ao::key(sorakado::window_id_t id, sorakado::key_t key, bool down) {
        if (menu_) {
            menu_->key(id, key, down);
        }
        for (auto &[_, v] : characters_) {
            v->key(id, key, down);
        }
    }

    void Ao::input(window_id_t id, const std::string &text) {
        for (auto &[_, v] : characters_) {
            v->input(id, text);
        }
    }
    
    void Ao::edit(window_id_t id, const std::string &text) {
        for (auto &[_, v] : characters_) {
            v->edit(id, text);
        }
    }

    void Ao::motion(sorakado::window_id_t id, float x, float y) {
        if (menu_) {
            menu_->motion(id, x, y);
        }
        for (auto &[_, v] : characters_) {
            v->motion(id, x, y);
        }
    }

    void Ao::button(sorakado::window_id_t id, float x, float y, sorakado::button_t button, bool down, Uint8 clicks) {
        if (menu_) {
            menu_->button(id, x, y, button, down, clicks);
        }
        for (auto &[_, v] : characters_) {
            v->button(id, x, y, button, down, clicks);
        }
    }

    void Ao::wheel(sorakado::window_id_t id, float x, float y) {
        for (auto &[_, v] : characters_) {
            v->wheel(id, x, y);
        }
    }

    void Ao::drop(sorakado::window_id_t id, const std::vector<std::string> &list) {
        for (auto &[_, v] : characters_) {
            v->drop(id, list);
        }
    }

    void Ao::maximized(sorakado::window_id_t id) {
        for (auto &[_, v] : characters_) {
            v->maximized(id);
        }
    }

    void Ao::focus(sorakado::window_id_t id, bool focused) {
        if (menu_) {
            menu_->focus(id, focused);
        }
        for (auto &[_, v] : characters_) {
            v->focus(id, focused);
        }
    }

    void Ao::hover(int side, float x, float y) {
    }

    void Ao::click(int side, float x, float y, button_t button, bool down, Uint8 clicks) {
    }

    void Ao::run() {
        if (menu_ && !(menu_->alive() && (!menu_->focusGained() || menu_->focused()))) {
            menu_.reset();
        }
        if (menu_) {
            menu_->run();
        }
    }

    bool Ao::draw() {
auto b = std::chrono::system_clock::now();
auto a = b;
        if (menu_) {
            menu_->draw(image_cache_);
        }
b = std::chrono::system_clock::now();
//Logger::log("chrono.ao1", std::chrono::duration_cast<std::chrono::microseconds>(b - a));
a = b;
        for (auto &[_, v] : characters_) {
            v->draw(image_cache_);
        }
b = std::chrono::system_clock::now();
//Logger::log("chrono.ao2", std::chrono::duration_cast<std::chrono::microseconds>(b - a));
a = b;
        bool redrawn = false;
        if (menu_) {
            redrawn = menu_->swapBuffers() || redrawn;
        }
b = std::chrono::system_clock::now();
//Logger::log("chrono.ao3", std::chrono::duration_cast<std::chrono::microseconds>(b - a));
a = b;
        for (auto &[_, v] : characters_) {
            redrawn = v->swapBuffers() || redrawn;
        }
b = std::chrono::system_clock::now();
//Logger::log("chrono.ao4", std::chrono::duration_cast<std::chrono::microseconds>(b - a));
a = b;
        return redrawn;
    }

    void Ao::createMenu(const std::vector<MenuModelData> &data) {
        if (SDL_GetWindowID(menu_initializer_.parent) == 0) {
            return;
        }
        menu_ = std::make_unique<ContextMenu>(this, menu_initializer_.r, menu_initializer_.parent, image_cache_, font_cache_->getDefaultFont());
        Rect parent_r = {menu_initializer_.pos.x, menu_initializer_.pos.y, 0, 0};
        menu_->createSubMenu(data, menu_initializer_.pos, parent_r);
    }

    void Ao::surfaceChanged(int side, int id) {
        std::vector<std::string> args;

        if (characters_.contains(0)) {
            args.push_back(util::to_s(characters_.at(0)->getSurfaceID()));
        }
        else {
            args.push_back("-1");
        }
        if (characters_.contains(1)) {
            args.push_back(util::to_s(characters_.at(1)->getSurfaceID()));
        }
        else {
            args.push_back("-1");
        }
        // FIXME w, h
        args.push_back(util::to_s(side) + "," + util::to_s(id) + ",0,0");
        directsstp::Request req = {"NOTIFY", "OnSurfaceChange", args};
        enqueueDirectSSTP({req});
    }

    void Ao::setSurfaceID(int side, std::string id) {
        if (!characters_.contains(side)) {
            return;
        }
        characters_.at(side)->setSurfaceID(id);
    }

    void Ao::startAnimation(int side, std::string id) {
        if (!characters_.contains(side)) {
            return;
        }
        characters_.at(side)->startAnimation(id);
    }

    bool Ao::isPlayingAnimation(int side, std::string id) const {
        if (!characters_.contains(side)) {
            return false;
        }
        return characters_.at(side)->isPlayingAnimation(id);
    }

    std::unordered_set<int> Ao::getActiveAnimationList(int side) const {
        if (!characters_.contains(side)) {
            return {};
        }
        return characters_.at(side)->getActiveAnimationList();
    }

    void Ao::bind(int side, std::string category, std::string parts, std::string from, BindFlag flag) {
        if (!characters_.contains(side)) {
            return;
        }
        auto key = category + "," + parts;
        if (!bind_id_.contains(side)) {
            return;
        }
        if (!bind_id_.at(side).contains(key)) {
            return;
        }
        int id = bind_id_.at(side).at(key);
        characters_.at(side)->bind(id, from, flag);
    }

    void Ao::clearCache() {
        image_cache_->clearCache();
        for (auto &[_, v] : characters_) {
            v->clearCache();
        }
    }

    void Ao::raise(int side) {
        if (!characters_.contains(side)) {
            return;
        }
        characters_[side]->raise();
    }

    std::vector<MenuModelData> Ao::getDressUpList() {
        std::vector<MenuModelData> data;
        if (!bind_id_.contains(menu_initializer_.side)) {
            return data;
        }
        for (auto &[k, _] : bind_id_[menu_initializer_.side]) {
            // FIXME clickable
            MenuModelDataActionWithBoolean action = {
                .action = ActionType::DressUp,
                .valid = false,
                .caption = k,
                .state = false,
            };
            data.push_back(action);
        }
        return data;
    }

    std::optional<Position> Ao::getCharacterPosition(int side) const {
        int s = side;
        for (; s >= 0; s--) {
            if (characters_.contains(s)) {
                break;
            }
        }
        if (s == -1) {
            return std::nullopt;
        }
        return characters_.at(s)->getRect();
    }
    
    void Ao::reserveMenu(window_t parent, int side, Position pos, Rect r) {
        Logger::log("reserve.ao", pos.x, pos.y);
        menu_initializer_ = {parent, side, pos, r};
    }
}
