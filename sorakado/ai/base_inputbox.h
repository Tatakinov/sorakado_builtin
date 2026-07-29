#ifndef SORAKADO_AI_BASE_INPUTBOX_H_
#define SORAKADO_AI_BASE_INPUTBOX_H_

#include "sorakado/character.h"
#include "sorakado/ai/inputbox_info.h"

namespace sorakado::ai {
    class BaseInputbox : public Character {
        private:
            struct State {
                bool press;
                bool drag;
            };
            std::optional<DragPosition> drag_;
            std::unordered_map<button_t, State> mouse_state_;
            InputboxInfo info_;
            region_t region_;

            virtual void activate(const std::string &text) = 0;
            virtual void cancel(const std::string &reason) = 0;
        protected:
            bool alive_;
        public:
            BaseInputbox(sorakado::Sorakado *parent, std::unique_ptr<WindowManager> manager, const Rect &inputbox_r, const Color &color, const std::filesystem::path path, std::unique_ptr<ImageCache> &image_cache, std::unique_ptr<WrapFont> &font) : Character(parent, std::move(manager), -1, "unused"), info_(inputbox_r, color, path, image_cache, font), alive_(true) {}
            virtual ~BaseInputbox() {}

            bool alive() const {
                return alive_;
            }

            void draw(std::unique_ptr<ImageCache> &cache) override;

            void resetPosition(bool initialize) override;

            bool setSize(int w, int h) override {
                return false;
            }

            void press(key_t key, bool down) override;
            void hover(float x, float y) override;
            void click(Window *window, float x, float y, button_t button, bool down, Uint8 clicks) override;

            void inputText(const std::string &text) override;
            void editText(const std::string &text) override;
    };
}

#endif // SORAKADO_AI_BASE_INPUTBOX_H_
