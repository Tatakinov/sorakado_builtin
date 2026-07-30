#ifndef SORAKADO_AO_MENU_H_
#define SORAKADO_AO_MENU_H_

#include <json/json.h>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "sorakado/font.h"
#include "sorakado/misc.h"
#include "sorakado/render_info.h"
#include "sorakado/texture.h"

namespace {
    const int invalid = -1;
}

namespace sorakado {
    class Character;
}

namespace sorakado::ao {
    enum class ActionType {
        None, Site, StayOnTop, Preferences, Switch, Call, Shell, DressUp,
        Balloon, BasewareVersion, Close, CloseAll, ScriptInputBox, ScriptLog,
    };

    struct MenuModelDataAction {
        ActionType action;
        bool valid;
        std::string caption;
    };

    struct MenuModelDataActionWithBoolean {
        ActionType action;
        bool valid;
        std::string caption;
        bool state;
    };

    struct MenuModelDataActionWithArgs {
        ActionType action;
        bool valid;
        std::string caption;
        std::vector<std::string> args;
    };

    struct MenuModelDataSubMenu {
        ActionType action;
        std::string caption;
        std::vector<std::variant<MenuModelDataAction, MenuModelDataActionWithArgs, MenuModelDataActionWithBoolean, MenuModelDataSubMenu>> children;
    };

    using MenuModelData = std::variant<MenuModelDataAction, MenuModelDataActionWithArgs, MenuModelDataActionWithBoolean, MenuModelDataSubMenu>;

    std::vector<std::string> toList(const Json::Value &value);

    std::vector<MenuModelData> parseMenuInfo(const Json::Value &value, const std::vector<MenuModelData> dress_up_list);

    class MenuItem {
        private:
            const MenuModelData data_;
            const std::unique_ptr<WrapFont> &font_;
            SDL_Surface *surface_;
            bool highlight_;
        public:
            MenuItem(const MenuModelData &data, const std::unique_ptr<WrapFont> &font);
            ~MenuItem();
            std::optional<std::vector<MenuModelData>> getModel();
            template <typename T>
                std::optional<T> get() {
                    if (std::holds_alternative<T>(data_)) {
                        return std::get<T>(data_);
                    }
                    return std::nullopt;
                }
            ActionType getAction() const {
                ActionType type = ActionType::None;
                std::visit([&](const auto &x) {
                    type = x.action;
                }, data_);
                return type;
            }
            int width() const;
            int height() const;
            SDL_Surface *surface();
            bool highlight(int y);
            void unhighlight();
    };

    class SubMenu : public sorakado::RenderInfo {
        private:
            std::vector<std::unique_ptr<MenuItem>> item_list_;
            Rect r_;
            int scroll_;
            int prev_index_;
            int index_;
        public:
            SubMenu(const std::vector<MenuModelData> &data, const Rect &display_r, const std::unique_ptr<WrapFont> &font);
            ~SubMenu();
            int getSelectedItemY();
            template <typename T>
                std::optional<T> get() {
                    if (index_ == invalid) {
                        return std::nullopt;
                    }
                    assert(index_ < item_list_.size());
                    return item_list_[index_]->get<T>();
                }
            ActionType getAction() const {
                if (index_ == invalid) {
                    return ActionType::None;
                }
                assert(index_ < item_list_.size());
                return item_list_[index_]->getAction();
            }
            Rect rect() {
                return r_;
            }
            bool highlight(int x, int y);
            void unhighlight();
            std::unique_ptr<WrapSurface> getSurface(std::unique_ptr<ImageCache> &image_cache) const override;
            std::unique_ptr<WrapTexture> getTexture(std::unique_ptr<ImageCache> &image_cache, renderer_t *renderer, std::unique_ptr<TextureCache> &texture_cache) const override;
            bool equals(const RenderInfo &rhs) const override {
                return false;
            }
    };
}

#endif // SORAKADO_AO_MENU_H_
