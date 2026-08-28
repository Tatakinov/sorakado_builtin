#ifndef SORAKADO_AI_MASTER_CHARACTER_H_
#define SORAKADO_AI_MASTER_CHARACTER_H_

#include <memory>
#include <string>
#include <vector>

#include "sorakado/character.h"
#include "sorakado/font.h"
#include "sorakado/ai/misc.h"
#include "sorakado/ai/master/render_info.h"

namespace sorakado {
    class Sorakado;
}

namespace sorakado::ai::master {
    class Character : public sorakado::Character {
        private:
            struct State {
                bool press;
                bool drag;
            };
            std::optional<DragPosition> drag_;
            std::unordered_map<button_t, State> mouse_state_;
            Position offset_;
            RenderInfo info_;
            bool raise_on_talk_;
            region_t region_;
            Link prev_link_;
            std::optional<Color> default_color_, disable_color_;
        protected:
        public:
            Character(sorakado::Sorakado *parent, std::unique_ptr<WindowManager> window_manager, int side, const std::string &name, std::unique_ptr<ImageCache> &image_cache, std::unique_ptr<FontCache> &font_cache);

            std::string getInfo(int id, std::string key, std::string fallback);
            void resetPosition(bool initialize) override;

            void press(key_t key, bool down) override;
            void dnd(const std::vector<std::string> &file_list) override;
            void hover(float x, float y) override;
            void click(Window *window, float x, float y, button_t button, bool down, click_t clicks) override;
            void scroll(float x, float y, float mouse_x, float mouse_y) override;

            void draw(std::unique_ptr<ImageCache> &cache) override;

            void setBalloonID(int id);
            void setBalloonPosition(int x, int y);
            void setBalloonDirection(int direction);
            void raiseOnTalk();
            bool setOffset(int x, int y);
            bool setSize(int w, int h) override;
            void clearText(bool initialize);
            void appendText(const std::string &text);
            void appendLinkBegin(bool is_anchor, const std::string &event, const std::vector<std::string> &args);
            void appendLinkEnd();
            void setCursorPosition(std::string axis, double value, bool is_absolute, MoveUnit unit);
            void newLine();
            void setScale(int scale);
            Color getDefaultColor(int id);
            Color getDisableColor(int id);
    };
}

#endif // SORAKADO_AI_MASTER_CHARACTER_H_
