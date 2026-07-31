#ifndef SORAKADO_AO_MASTER_AO_IMPL_H_
#define SORAKADO_AO_MASTER_AO_IMPL_H_

#include <memory>
#include <string>
#include <vector>

#include "sorakado/font_cache.h"
#include "sorakado/ao/ao.h"
#include "sorakado/ao/context_menu.h"
#include "sorakado/ao/master/character.h"
#include "sorakado/ao/master/surfaces.h"

namespace sorakado::ao::master {
    struct MenuInitializer {
        window_t parent;
        int side;
        Position pos;
        Rect r;
    };

    class Ao : public sorakado::ao::Ao {
        private:
            std::unique_ptr<ContextMenu> menu_;
            std::unordered_map<int, std::unique_ptr<Character>> characters_;
            std::unordered_map<int, std::unordered_map<std::string, int>> bind_id_;
            std::unique_ptr<ImageCache> image_cache_;
            std::unique_ptr<Surfaces> surfaces_;
            std::unique_ptr<FontCache> font_cache_;
            MenuInitializer menu_initializer_;
        public:
            Ao(sorakado::Application *parent, std::filesystem::path dir);
            ~Ao() {}

            void display(const display_t display, const bool added) override;
            void key(sorakado::window_id_t id, sorakado::key_t key, bool down) override;
            void input(window_id_t id, const std::string &text) override;
            void edit(window_id_t id, const std::string &text) override;
            void motion(sorakado::window_id_t id, float x, float y) override;
            void button(sorakado::window_id_t id, float x, float y, sorakado::button_t button, bool down, Uint8 clicks) override;
            void wheel(sorakado::window_id_t id, float x, float y) override;
            void drop(sorakado::window_id_t id, const std::vector<std::string> &list) override;
            void maximized(sorakado::window_id_t id) override;
            void focus(sorakado::window_id_t id, bool focused) override;

            void hover(int side, float x, float y) override;
            void click(int side, float x, float y, button_t button, bool down, Uint8 clicks) override;

            void run() override;
            bool draw() override;

            bool isPlayingAnimation(int side, std::string id) const override;
            std::unordered_set<int> getActiveAnimationList(int side) const override;
            void create(int side) override;
            void show(int side) override;
            void raise(int side) override;
            void hide(int side) override;
            void setScale(int value) override;
            void clearCache() override;
            void createMenu(const std::vector<MenuModelData> &data) override;
            std::vector<MenuModelData> getDressUpList() override;
            void surfaceChanged(int side, int id);
            void setSurfaceID(int side, std::string id) override;
            void startAnimation(int side, std::string id) override;
            void bind(int side, std::string category, std::string parts, std::string from, BindFlag flag) override;
            std::optional<Position> getCharacterPosition(int side) const override;
            void reserveMenu(window_t parent, int side, Position pos, Rect r);
    };
}

#endif // SORAKADO_AO_MASTER_AO_IMPL_H_
