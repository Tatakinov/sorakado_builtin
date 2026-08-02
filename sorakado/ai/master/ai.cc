#include "sorakado/ai/master/ai.h"
#include "os_preprocess.h"

#include <unordered_set>

#include "sorakado/default_backend_window_factory.h"
#include "sorakado/multiple_window_manager.h"
#include "sorakado/single_window_manager.h"
#include "sorakado/ai/inputbox_window.h"
#include "sorakado/ai/master/window.h"

#include "logger.h"

namespace sorakado::ai::master {
    Ai::Ai(Application *parent, std::filesystem::path dir) : sorakado::ai::Ai(parent, dir), scale_(100) {
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
        font_cache_ = std::make_unique<FontCache>();
        auto family = fontlist::get_default_font();
        font_cache_->setDefaultFont(family);
    }

    void Ai::create(int side) {
        auto window_factory = std::make_unique<AiMasterWindowFactory>();
        auto s = util::side2str(side);
        auto name = std::string("Balloon(") + s + ")";
        if (util::isWayland() && getenv("NINIX_ENABLE_MULTI_MONITOR")) {
            auto backend_window_factory = std::make_unique<DefaultBackendWindowFactory>(SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_RESIZABLE);
            auto manager = std::make_unique<MultipleWindowManager>(std::move(window_factory), std::move(backend_window_factory));
            characters_.try_emplace(side, std::make_unique<Character>(this, std::move(manager), side, name, image_cache_, font_cache_));
            int count = 0;
            auto *monitors = SDL_GetDisplays(&count);
            for (int i = 0; i < count; i++) {
                characters_.at(side)->create(monitors[i]);
            }
            SDL_free(monitors);
        }
        else if (util::isWayland()) {
            auto backend_window_factory = std::make_unique<DefaultBackendWindowFactory>(SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_RESIZABLE);
            auto manager = std::make_unique<SingleWindowManager>(std::move(window_factory), std::move(backend_window_factory));
            characters_.try_emplace(side, std::make_unique<Character>(this, std::move(manager), side, name, image_cache_, font_cache_));
            characters_.at(side)->create(0);
        }
        else {
            auto backend_window_factory = std::make_unique<DefaultBackendWindowFactory>(SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS);
            auto manager = std::make_unique<SingleWindowManager>(std::move(window_factory), std::move(backend_window_factory));
            characters_.try_emplace(side, std::make_unique<Character>(this, std::move(manager), side, name, image_cache_, font_cache_));
            characters_.at(side)->create(0);
        }
        return;
    }

    void Ai::show(int side) {
        if (!characters_.contains(side)) {
            return;
        }
        characters_.at(side)->show();
    }

    void Ai::hide(int side) {
        if (!characters_.contains(side)) {
            return;
        }
        characters_.at(side)->hide();
    }

    void Ai::hideAll() {
        for (auto &[_, v] : characters_) {
            v->hide();
        }
    }

    void Ai::setScale(int scale) {
        if (scale != scale_) {
            scale_ = scale;
            image_cache_->setScale(scale);
            clearCache();
            for (auto &[_, v] : characters_) {
                v->setScale(scale);
            }
        }
    }

    void Ai::display(const display_t display, const bool added) {
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

    void Ai::key(sorakado::window_id_t id, sorakado::key_t key, bool down) {
        if (script_inputbox_) {
            script_inputbox_->key(id, key, down);
        }
        for (auto &[_, v] : inputboxes_) {
            v->key(id, key, down);
        }
        for (auto &[_, v] : characters_) {
            v->key(id, key, down);
        }
    }

    void Ai::input(window_id_t id, const std::string &text) {
        if (script_inputbox_) {
            script_inputbox_->input(id, text);
        }
        for (auto &[_, v] : inputboxes_) {
            v->input(id, text);
        }
        for (auto &[_, v] : characters_) {
            v->input(id, text);
        }
    }
    
    void Ai::edit(window_id_t id, const std::string &text) {
        if (script_inputbox_) {
            script_inputbox_->edit(id, text);
        }
        for (auto &[_, v] : inputboxes_) {
            v->edit(id, text);
        }
        for (auto &[_, v] : characters_) {
            v->edit(id, text);
        }
    }

    void Ai::motion(sorakado::window_id_t id, float x, float y) {
        if (script_inputbox_) {
            script_inputbox_->motion(id, x, y);
        }
        for (auto &[_, v] : inputboxes_) {
            v->motion(id, x, y);
        }
        for (auto &[_, v] : characters_) {
            v->motion(id, x, y);
        }
    }

    void Ai::button(sorakado::window_id_t id, float x, float y, sorakado::button_t button, bool down, click_t clicks) {
        if (script_inputbox_) {
            script_inputbox_->button(id, x, y, button, down, clicks);
        }
        for (auto &[_, v] : inputboxes_) {
            v->button(id, x, y, button, down, clicks);
        }
        for (auto &[_, v] : characters_) {
            v->button(id, x, y, button, down, clicks);
        }
    }

    void Ai::wheel(sorakado::window_id_t id, float x, float y) {
        for (auto &[_, v] : characters_) {
            v->wheel(id, x, y);
        }
    }

    void Ai::drop(sorakado::window_id_t id, const std::vector<std::string> &list) {
        for (auto &[_, v] : characters_) {
            v->drop(id, list);
        }
    }

    void Ai::maximized(sorakado::window_id_t id) {
        if (script_inputbox_) {
            script_inputbox_->maximized(id);
        }
        for (auto &[_, v] : inputboxes_) {
            v->maximized(id);
        }
        for (auto &[_, v] : characters_) {
            v->maximized(id);
        }
    }

    void Ai::focus(sorakado::window_id_t id, bool focused) {
        for (auto &[_, v] : characters_) {
            v->focus(id, focused);
        }
    }

    void Ai::hover(int side, float x, float y) {
    }

    void Ai::click(int side, float x, float y, button_t button, bool down, click_t clicks) {
    }

    void Ai::run() {
        if (script_inputbox_ && !script_inputbox_->alive()) {
            script_inputbox_.reset();
        }
        std::unordered_set<std::string> keys;
        for (auto &[k, _] : inputboxes_) {
            keys.emplace(k);
        }
        for (auto &k : keys) {
            if (!inputboxes_.at(k)->alive()) {
                inputboxes_.erase(k);
            }
        }
    }

    bool Ai::draw() {
        if (script_inputbox_) {
            Logger::log("script-input.draw");
            script_inputbox_->draw(image_cache_);
        }
        for (auto &[_, v] : inputboxes_) {
            v->draw(image_cache_);
        }
        for (auto &[_, v] : characters_) {
            v->draw(image_cache_);
        }
        bool redrawn = false;
        if (script_inputbox_) {
            redrawn = script_inputbox_->swapBuffers() || redrawn;
        }
        for (auto &[_, v] : inputboxes_) {
            v->swapBuffers() || redrawn;
        }
        for (auto &[_, v] : characters_) {
            redrawn = v->swapBuffers() || redrawn;
        }
        return redrawn;
    }

    void Ai::clearCache() {
        image_cache_->clearCache();
        for (auto &[_, v] : characters_) {
            v->clearCache();
        }
    }

    void Ai::raise(int side) {
        if (!characters_.contains(side)) {
            return;
        }
        characters_[side]->raise();
    }

    void Ai::raiseOnTalk(int side) {
        if (!characters_.contains(side)) {
            return;
        }
        characters_[side]->raiseOnTalk();
    }

    void Ai::setBalloonID(int side, int id) {
        if (!characters_.contains(side)) {
            return;
        }
        characters_.at(side)->setBalloonID(id);
    }

    void Ai::resetBalloonID(int side) {
        if (side == -1) {
            for (auto &[_, v] : characters_) {
                v->setBalloonID(0);
            }
        }
        else {
            if (!characters_.contains(side)) {
                return;
            }
            characters_.at(side)->setBalloonID(0);
        }
    }

    void Ai::setBalloonPosition(int side, int x, int y) {
        if (!characters_.contains(side)) {
            return;
        }
        characters_.at(side)->setBalloonPosition(x, y);
    }

    void Ai::setBalloonDirection(int side, int direction) {
        if (!characters_.contains(side)) {
            return;
        }
        characters_.at(side)->setBalloonDirection(direction);
    }

    void Ai::setFont(const std::string &name) {
        auto &font = font_cache_->getDefaultFont();
        if (font && font->name() == name) {
            return;
        }
        auto family = fontlist::get_default_font();
        auto family_list = fontlist::enumerate_font();
        for (auto &f : family_list) {
            if (f.name == name) {
                family = f;
                break;
            }
        }
        if (font && font->name() == family.name) {
            return;
        }
        font_cache_->setDefaultFont(family);
    }

    void Ai::clearText(int side, bool initialize) {
        if (!characters_.contains(side)) {
            return;
        }
        characters_.at(side)->clearText(initialize);
    }

    void Ai::clearTextAll() {
        for (auto &[_, v] : characters_) {
            v->clearText(true);
        }
    }

    void Ai::appendText(int side, const std::string &text) {
        if (!characters_.contains(side)) {
            return;
        }
        characters_.at(side)->appendText(text);
    }

    void Ai::appendLinkBegin(int side, bool is_anchor, const std::string &event, const std::vector<std::string> &args) {
        if (!characters_.contains(side)) {
            return;
        }
        characters_.at(side)->appendLinkBegin(is_anchor, event, args);
    }

    void Ai::appendLinkEnd(int side) {
        if (!characters_.contains(side)) {
            return;
        }
        characters_.at(side)->appendLinkEnd();
    }

    void Ai::setCursorPosition(int side, std::string axis, double value, bool is_absolute, MoveUnit unit) {
        if (!characters_.contains(side)) {
            return;
        }
        characters_.at(side)->setCursorPosition(axis, value, is_absolute, unit);
    }

    void Ai::newLine(int side) {
        if (!characters_.contains(side)) {
            return;
        }
        characters_.at(side)->newLine();
    }

    void Ai::openInputBox(const std::string &id) {
        int r, g, b, a = 0xff;
        std::string s;
        s = getInfo("communicatebox.font.color.r", false);
        if (s.empty()) {
            r = 0;
        }
        else {
            util::to_x(s, r);
        }
        s = getInfo("communicatebox.font.color.g", false);
        if (s.empty()) {
            g = 0;
        }
        else {
            util::to_x(s, g);
        }
        s = getInfo("communicatebox.font.color.b", false);
        if (s.empty()) {
            b = 0;
        }
        else {
            util::to_x(s, b);
        }
        Color color = {r, g, b, a};
        int x, y, w, h;
        s = getInfo("communicatebox.x", false);
        if (s.empty()) {
            x = 20;
        }
        else {
            util::to_x(s, x);
        }
        s = getInfo("communicatebox.y", false);
        if (s.empty()) {
            y = 20;
        }
        else {
            util::to_x(s, y);
        }
        s = getInfo("communicatebox.w", false);
        if (s.empty()) {
            w = 200;
        }
        else {
            util::to_x(s, w);
        }
        s = getInfo("communicatebox.h", false);
        if (s.empty()) {
            h = 20;
        }
        else {
            util::to_x(s, h);
        }
        Rect inputbox_r = {x, y, w, h};
        auto window_factory = std::make_unique<AiInputboxWindowFactory>(inputbox_r, font_cache_->get("default"));
        if (util::isWayland() && getenv("NINIX_ENABLE_MULTI_MONITOR")) {
            auto backend_window_factory = std::make_unique<DefaultBackendWindowFactory>(SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_RESIZABLE);
            auto manager = std::make_unique<MultipleWindowManager>(std::move(window_factory), std::move(backend_window_factory));
            inputboxes_.try_emplace(id, std::make_unique<Inputbox>(this, std::move(manager), inputbox_r, color, image_cache_, font_cache_->get("default"), id));
            int count = 0;
            auto *monitors = SDL_GetDisplays(&count);
            for (int i = 0; i < count; i++) {
                inputboxes_.at(id)->create(monitors[i]);
            }
            SDL_free(monitors);
        }
        else if (util::isWayland()) {
            auto backend_window_factory = std::make_unique<DefaultBackendWindowFactory>(SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_RESIZABLE);
            auto manager = std::make_unique<SingleWindowManager>(std::move(window_factory), std::move(backend_window_factory));
            inputboxes_.try_emplace(id, std::make_unique<Inputbox>(this, std::move(manager), inputbox_r, color, image_cache_, font_cache_->get("default"), id));
            inputboxes_.at(id)->create(0);
        }
        else {
            auto backend_window_factory = std::make_unique<DefaultBackendWindowFactory>(SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS);
            auto manager = std::make_unique<SingleWindowManager>(std::move(window_factory), std::move(backend_window_factory));
            inputboxes_.try_emplace(id, std::make_unique<Inputbox>(this, std::move(manager), inputbox_r, color, image_cache_, font_cache_->get("default"), id));
            inputboxes_.at(id)->create(0);
        }
        inputboxes_.at(id)->show();
    }

    void Ai::openScriptInputBox() {
        if (script_inputbox_) {
            return;
        }
        Logger::log("script-input");
        int r, g, b, a = 0xff;
        std::string s;
        s = getInfo("communicatebox.font.color.r", false);
        if (s.empty()) {
            r = 0;
        }
        else {
            util::to_x(s, r);
        }
        s = getInfo("communicatebox.font.color.g", false);
        if (s.empty()) {
            g = 0;
        }
        else {
            util::to_x(s, g);
        }
        s = getInfo("communicatebox.font.color.b", false);
        if (s.empty()) {
            b = 0;
        }
        else {
            util::to_x(s, b);
        }
        Color color = {r, g, b, a};
        int x, y, w, h;
        s = getInfo("communicatebox.x", false);
        if (s.empty()) {
            x = 20;
        }
        else {
            util::to_x(s, x);
        }
        s = getInfo("communicatebox.y", false);
        if (s.empty()) {
            y = 20;
        }
        else {
            util::to_x(s, y);
        }
        s = getInfo("communicatebox.w", false);
        if (s.empty()) {
            w = 200;
        }
        else {
            util::to_x(s, w);
        }
        s = getInfo("communicatebox.h", false);
        if (s.empty()) {
            h = 20;
        }
        else {
            util::to_x(s, h);
        }
        Rect inputbox_r = {x, y, w, h};
        auto window_factory = std::make_unique<AiInputboxWindowFactory>(inputbox_r, font_cache_->get("default"));
        if (util::isWayland() && getenv("NINIX_ENABLE_MULTI_MONITOR")) {
            auto backend_window_factory = std::make_unique<DefaultBackendWindowFactory>(SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_RESIZABLE);
            auto manager = std::make_unique<MultipleWindowManager>(std::move(window_factory), std::move(backend_window_factory));
            script_inputbox_ = std::make_unique<ScriptInputbox>(this, std::move(manager), inputbox_r, color, image_cache_, font_cache_->get("default"));
            int count = 0;
            auto *monitors = SDL_GetDisplays(&count);
            for (int i = 0; i < count; i++) {
                script_inputbox_->create(monitors[i]);
            }
            SDL_free(monitors);
        }
        else if (util::isWayland()) {
            auto backend_window_factory = std::make_unique<DefaultBackendWindowFactory>(SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_RESIZABLE);
            auto manager = std::make_unique<SingleWindowManager>(std::move(window_factory), std::move(backend_window_factory));
            script_inputbox_ = std::make_unique<ScriptInputbox>(this, std::move(manager), inputbox_r, color, image_cache_, font_cache_->get("default"));
            script_inputbox_->create(0);
        }
        else {
            auto backend_window_factory = std::make_unique<DefaultBackendWindowFactory>(SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS);
            auto manager = std::make_unique<SingleWindowManager>(std::move(window_factory), std::move(backend_window_factory));
            script_inputbox_ = std::make_unique<ScriptInputbox>(this, std::move(manager), inputbox_r, color, image_cache_, font_cache_->get("default"));
            script_inputbox_->create(0);
        }
        script_inputbox_->show();
    }
}
