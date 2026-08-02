#ifndef SORAKADO_AI_MASTER_AI_H_
#define SORAKADO_AI_MASTER_AI_H_

#include <memory>
#include <string>
#include <vector>

#include "sorakado/font_cache.h"
#include "sorakado/image_cache.h"
#include "sorakado/ai/ai.h"
#include "sorakado/ai/inputbox.h"
#include "sorakado/ai/script_inputbox.h"
#include "sorakado/ai/misc.h"
#include "sorakado/ai/master/character.h"

namespace sorakado::ai::master {
    class Ai : public sorakado::ai::Ai {
        private:
            int scale_;
            std::unique_ptr<ImageCache> image_cache_;
            std::unique_ptr<FontCache> font_cache_;
            std::unordered_map<int, std::unique_ptr<Character>> characters_;
            std::unordered_map<std::string, std::unique_ptr<Inputbox>> inputboxes_;
            std::unique_ptr<ScriptInputbox> script_inputbox_;
        protected:
        public:
            Ai(sorakado::Application *parent, std::filesystem::path dir);
            ~Ai() {}

            void display(const display_t display, const bool added) override;
            void key(sorakado::window_id_t id, sorakado::key_t key, bool down) override;
            void input(window_id_t id, const std::string &text) override;
            void edit(window_id_t id, const std::string &text) override;
            void motion(sorakado::window_id_t id, float x, float y) override;
            void button(sorakado::window_id_t id, float x, float y, sorakado::button_t button, bool down, click_t clicks) override;
            void wheel(sorakado::window_id_t id, float x, float y) override;
            void drop(sorakado::window_id_t id, const std::vector<std::string> &list) override;
            void maximized(sorakado::window_id_t id) override;
            void focus(sorakado::window_id_t id, bool focused) override;

            void hover(int side, float x, float y) override;
            void click(int side, float x, float y, button_t button, bool down, click_t clicks) override;

            void run() override;
            bool draw() override;

            void setBalloonID(int side, int id) override;
            void resetBalloonID(int side = -1) override;
            void setBalloonPosition(int side, int x, int y) override;
            void setBalloonDirection(int side, int direction) override;
            void create(int side) override;
            void show(int side) override;
            void raise(int side) override;
            void raiseOnTalk(int side) override;
            void hide(int side) override;
            void hideAll() override;
            void setScale(int value) override;
            void setFont(const std::string &name) override;
            void clearText(int side, bool initialize) override;
            void clearTextAll() override;
            void clearCache() override;
            void appendText(int side, const std::string &text) override;
            void appendLinkBegin(int side, bool is_anchor, const std::string &event, const std::vector<std::string> &args) override;
            void appendLinkEnd(int side) override;
            void setCursorPosition(int side, std::string axis, double value, bool is_absolute, MoveUnit unit) override;
            void newLine(int side) override;
            void openInputBox(const std::string &id) override;
            void openScriptInputBox() override;
    };
}

#endif // SORAKADO_AI_MASTER_AI_H_
