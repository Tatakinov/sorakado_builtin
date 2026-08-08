#ifndef SORAKADO_AO_MASTER_CHARACTER_H_
#define SORAKADO_AO_MASTER_CHARACTER_H_

#include <chrono>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>

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
            struct MoveInfo {
                bool is_async;
                Position dst;
                int time; // ms
                std::chrono::system_clock::time_point start;
                int prev;
            };
            std::optional<DragPosition> drag_;
            std::unordered_map<button_t, State> mouse_state_;
            std::unique_ptr<Seriko> seriko_;
            std::optional<ElementWithChildren> prev_info_;
            region_t current_surface_;
            std::optional<MoveInfo> move_info_;

            void notifyRectInfo();
        public:
            Character(sorakado::Sorakado *parent, std::unique_ptr<WindowManager> window_manager, int side, const std::string &name, std::unique_ptr<Seriko> seriko);

            void create(display_t id) override;
            void destroy(display_t id) override;
            void draw(std::unique_ptr<ImageCache> &cache) override;

            bool setPosition(int x, int y) override;
            bool setOffset(int x, int y) override;
            bool setSize(int w, int h) override;
            void resetPosition(bool initialize) override;
            void alignmentPosition();
            void move(bool is_async, const std::string &x, const std::string &y, int time, const std::string &base, const std::string &base_offset, const std::string &move_offset, const std::vector<std::string> &options);

            void press(key_t key, bool down) override;
            void dnd(const std::vector<std::string> &file_list) override;
            void hover(float x, float y) override;
            void click(Window *window, float x, float y, button_t button, bool down, click_t clicks) override;
            void scroll(float x, float y, float mouse_x, float mouse_y) override;

            int getSurfaceID() const;
            void setSurfaceID(const std::string &id);
            bool isPlayingAnimation(const std::string &id) const;
            std::unordered_set<int> getActiveAnimationList() const;
            void startAnimation(const std::string &id);
            void bind(int id, std::string from, BindFlag flag);

            std::string getHitBoxName(int x, int y);
            void setScale(int scale);

            void displayChanged();

            void run();
    };
}

#endif // SORAKADO_AO_MASTER_CHARACTER_H_
