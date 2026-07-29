#ifndef SORAKADO_AO_MASTER_CHARACTER_H_
#define SORAKADO_AO_MASTER_CHARACTER_H_

#include <memory>
#include <optional>
#include <unordered_map>

#include "sorakado/misc.h"
#include "sorakado/character.h"
#include "sorakado/ao/master/element.h"
#include "sorakado/ao/master/seriko.h"

namespace sorakado::ao::master {
    class Character : public sorakado::Character {
        private:
            struct State {
                bool press;
                bool drag;
            };
            std::optional<DragPosition> drag_;
            std::unordered_map<button_t, State> mouse_state_;
            std::unique_ptr<Seriko> seriko_;
            std::optional<ElementWithChildren> prev_info_;
            region_t current_surface_;
        public:
            Character(sorakado::Sorakado *parent, std::unique_ptr<WindowManager> window_manager, int side, const std::string &name, std::unique_ptr<Seriko> seriko);

            void draw(std::unique_ptr<ImageCache> &cache) override;

            bool setOffset(int x, int y) override;
            bool setSize(int w, int h) override;
            void resetPosition(bool initialize) override;
            void alignmentPosition();

            void press(key_t key, bool down) override;
            void dnd(const std::vector<std::string> &file_list) override;
            void hover(float x, float y) override;
            void click(Window *window, float x, float y, button_t button, bool down, Uint8 clicks) override;
            void scroll(float x, float y, float mouse_x, float mouse_y) override;

            void setSurfaceID(const std::string &id);
            bool isPlayingAnimation(const std::string &id) const;
            void startAnimation(const std::string &id);
            void bind(int id, std::string from, BindFlag flag);

            std::string getHitBoxName(int x, int y);
            void setScale(int scale);
    };
}

#endif // SORAKADO_AO_MASTER_CHARACTER_H_
