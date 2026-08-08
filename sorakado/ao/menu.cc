#include "sorakado/ao/menu.h"

#include <cassert>

#include "logger.h"
#include "sorakado/character.h"
#include "sorakado/misc.h"
#include "sorakado/texture.h"

#define MOUSE_BUTTON_LEFT 1

namespace sorakado::ao {
    std::vector<std::string> toList(const Json::Value &value) {
        std::vector<std::string> list;
        for (int i = 0; !value[i].isNull(); i++) {
            list.push_back(value[i].asString());
        }
        return list;
    }

    std::vector<MenuModelData> parseMenuInfo(const Json::Value &value, const std::vector<MenuModelData> dress_up_list) {
        std::vector<MenuModelData> data;
        for (int i = 0; !value[i].isNull(); i++) {
            auto &v = value[i];
            auto type = v["type"].asString();
            if (type == "submenu") {
                MenuModelDataSubMenu submenu = {
                    .action = ActionType::None,
                    .caption = v["caption"].asString(),
                    .children = parseMenuInfo(v["list"], dress_up_list),
                };
                data.push_back(submenu);
                assert(std::holds_alternative<MenuModelDataSubMenu>(data.back()));
            }
            if (type == "site") {
                MenuModelDataActionWithArgs args = {
                    .action = ActionType::Site,
                    .valid = v["valid"].asBool(),
                    .caption = v["caption"].asString(),
                    .args = toList(v["list"]),
                };
                data.push_back(args);
            }
            if (type == "check") {
                MenuModelDataActionWithBoolean check = {
                    .action = ActionType::StayOnTop,
                    .valid = v["valid"].asBool(),
                    .caption = v["caption"].asString(),
                    .state = v["state"].asBool(),
                };
                data.push_back(check);
            }
            if (type == "preferences") {
                MenuModelDataAction action = {
                    .action = ActionType::Preferences,
                    .valid = v["valid"].asBool(),
                    .caption = v["caption"].asString(),
                };
                data.push_back(action);
            }
            if (type == "scriptinputbox") {
                MenuModelDataAction action = {
                    .action = ActionType::ScriptInputBox,
                    .valid = v["valid"].asBool(),
                    .caption = v["caption"].asString(),
                };
                data.push_back(action);
            }
            if (type == "scriptlog") {
                MenuModelDataAction action = {
                    .action = ActionType::ScriptLog,
                    .valid = v["valid"].asBool(),
                    .caption = v["caption"].asString(),
                };
                data.push_back(action);
            }
            if (type == "switch") {
                MenuModelDataActionWithArgs action = {
                    .action = ActionType::Switch,
                    .valid = v["valid"].asBool(),
                    .caption = v["caption"].asString(),
                    .args = toList(v["list"])
                };
                data.push_back(action);
            }
            if (type == "call") {
                MenuModelDataActionWithArgs action = {
                    .action = ActionType::Call,
                    .valid = v["valid"].asBool(),
                    .caption = v["caption"].asString(),
                    .args = toList(v["list"])
                };
                data.push_back(action);
            }
            if (type == "shell") {
                MenuModelDataActionWithArgs action = {
                    .action = ActionType::Shell,
                    .valid = v["valid"].asBool(),
                    .caption = v["caption"].asString(),
                    .args = toList(v["list"])
                };
                data.push_back(action);
            }
            if (type == "dressup") {
                MenuModelDataSubMenu dressup = {
                    .caption = v["caption"].asString(),
                    .children = dress_up_list,
                };
                data.push_back(dressup);
            }
            if (type == "balloon") {
                MenuModelDataActionWithArgs action = {
                    .action = ActionType::Balloon,
                    .valid = v["valid"].asBool(),
                    .caption = v["caption"].asString(),
                    .args = toList(v["list"])
                };
                data.push_back(action);
            }
            if (type == "basewareversion") {
                MenuModelDataAction action = {
                    .action = ActionType::BasewareVersion,
                    .valid = v["valid"].asBool(),
                    .caption = v["caption"].asString(),
                };
                data.push_back(action);
            }
            if (type == "close") {
                MenuModelDataAction action = {
                    .action = ActionType::Close,
                    .valid = v["valid"].asBool(),
                    .caption = v["caption"].asString(),
                };
                data.push_back(action);
            }
            if (type == "close_all") {
                MenuModelDataAction action = {
                    .action = ActionType::CloseAll,
                    .valid = v["valid"].asBool(),
                    .caption = v["caption"].asString(),
                };
                data.push_back(action);
            }
        }
        return data;
    }

    MenuItem::MenuItem(const MenuModelData &data, const std::unique_ptr<WrapFont> &font) : data_(data), font_(font), highlight_(false) {
        SDL_Color color = {0x00, 0x00, 0x00, 0xff};
        std::visit([&](const auto &d) {
            surface_ = TTF_RenderText_Blended(font_->font(), d.caption.data(), d.caption.length(), color);
        }, data_);
    }

    MenuItem::~MenuItem() {
        if (surface_ != nullptr) {
            SDL_DestroySurface(surface_);
        }
    }

    int MenuItem::width() const {
        if (surface_ == nullptr) {
            return 0;
        }
        return surface_->w;
    }

    int MenuItem::height() const {
        if (surface_ == nullptr) {
            return 0;
        }
        return surface_->h;
    }

    SDL_Surface *MenuItem::surface() {
        return surface_;
    }

    bool MenuItem::highlight(int y) {
        if (y >= 0 && y < height()) {
            highlight_ = true;
            return true;
        }
        return false;
    }

    void MenuItem::unhighlight() {
        highlight_ = false;
    }

    SubMenu::SubMenu(const std::vector<MenuModelData> &data, const Rect &display_r, const std::unique_ptr<WrapFont> &font) : RenderInfo(), r_({0, 0, 0, 0}), scroll_(0), prev_index_(invalid), index_(invalid) {
        for (auto &v : data) {
            item_list_.push_back(std::make_unique<MenuItem>(v, font));
            auto &last = item_list_.back();
            if (r_.w < last->width()) {
                r_.w = last->width();
            }
            r_.h += last->height();
        }
        if (r_.w > display_r.w) {
            r_.w = display_r.w;
        }
        if (r_.h > display_r.h) {
            r_.h = display_r.h;
        }
        change();
    }

    SubMenu::~SubMenu() {
    }

    int SubMenu::getSelectedItemY() {
        if (index_ == invalid) {
            return 0;
        }
        int height = 0;
        for (int i = 0; i < index_; i++) {
            height += item_list_[index_]->height();
        }
        return height;
    }

    bool SubMenu::highlight(int x, int y) {
        x -= r_.x;
        y -= r_.y;
        if (x < 0 || x > r_.w) {
            return false;
        }
        for (auto &item : item_list_) {
            index_++;
            if (item->highlight(y)) {
                if (prev_index_ != index_) {
                    prev_index_ = index_;
                    change();
                    return true;
                }
                return false;
            }
            y -= item->height();
        }
        index_ = invalid;
        if (prev_index_ != index_) {
            prev_index_ = index_;
            change();
            return true;
        }
        return false;
    }

    void SubMenu::unhighlight() {
        index_ = invalid;
        for (auto &item : item_list_) {
            item->unhighlight();
        }
    }

    Region SubMenu::getRegion(std::unique_ptr<ImageCache> &image_cache) const {
        Region r;
        for (int y = 0; y < r_.h; y++) {
            r.push_back({0, y, r_.w});
        }
        return r;
    }

    std::unique_ptr<WrapSurface> SubMenu::getSurface(std::unique_ptr<ImageCache> &cache) const {
        auto s = std::make_unique<WrapSurface>(r_.w, r_.h);
        SDL_ClearSurface(s->surface(), 1, 1, 1, 1); // とりあえず白背景
        int height = 0;
        for (auto &item : item_list_) {
            SDL_Rect r = {0, height, item->width(), item->height()};
            SDL_BlitSurface(item->surface(), nullptr, s->surface(), &r);
            height += item->height();
        }
        return s;
    }

    std::unique_ptr<WrapTexture> SubMenu::getTexture(std::unique_ptr<ImageCache> &image_cache, renderer_t *renderer, std::unique_ptr<TextureCache> &texture_cache) const {
        auto s = getSurface(image_cache);
        if (!s) {
            std::unique_ptr<WrapTexture> invalid;
            return invalid;
        }
        auto t = std::make_unique<WrapTexture>(renderer, s->surface(), true);
        if (index_ == invalid) {
            return t;
        }
        auto flip = std::make_unique<WrapTexture>(renderer, 1, 1, true);
        SDL_SetRenderTarget(renderer, flip->texture());
        SDL_SetRenderDrawColor(renderer, 0x7f, 0x7f, 0x7f, 0x7f);
        SDL_RenderClear(renderer);
        auto dst = std::make_unique<WrapTexture>(renderer, t->width(), t->height(), true);
        SDL_SetRenderTarget(renderer, dst->texture());
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, t->texture(), nullptr, nullptr);
        SDL_BlendMode old;
        SDL_GetRenderDrawBlendMode(renderer, &old);
        SDL_BlendMode m = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE_MINUS_DST_COLOR, SDL_BLENDFACTOR_ZERO, SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD);
        SDL_SetRenderDrawBlendMode(renderer, m);
        SDL_FRect r = {0, 0, dst->width(), item_list_[index_]->height()};
        for (int i = 0; i < index_; i++) {
            r.y += item_list_[i]->height();
        }
        SDL_RenderTexture(renderer, flip->texture(), nullptr, &r);
        SDL_SetRenderDrawBlendMode(renderer, old);

        SDL_SetRenderTarget(renderer, nullptr);
        return dst;
    }
}
