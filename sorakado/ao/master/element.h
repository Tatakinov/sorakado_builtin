#ifndef SORAKADO_AO_MASTER_ELEMENT_H_
#define SORAKADO_AO_MASTER_ELEMENT_H_

#include <bit>
#include <memory>
#include <variant>

#include "sorakado/render_info.h"
#include "sorakado/ao/master/misc.h"
#include "sorakado/ao/master/surface.h"

namespace sorakado::ao::master {
    struct ElementWithNoChildren;
    struct ElementWithChildren;

    struct ElementWithNoChildren : public Element {
        private:
            ImageCache &image_cache_;
        public:
            ElementWithNoChildren(ImageCache &image_cache, Element e) : Element(e), image_cache_(image_cache) {}
            Rect getRect(bool include_empty_image) const;
            Region getRegion() const;
            std::unique_ptr<WrapSurface> getSurface() const;
            WrapTexture *getTexture(renderer_t *renderer, TextureCache &texture_cache) const;
            bool operator==(const ElementWithNoChildren &rhs) const;
    };

    struct ElementWithChildren : public sorakado::RenderInfo {
        ElementWithChildren(ImageCache &image_cache, Method _method, int _x, int _y, std::vector<std::variant<ElementWithNoChildren, ElementWithChildren>> _children) : RenderInfo(image_cache), method(_method), x(_x), y(_y), children(_children) {}
        Method method;
        int x, y;
        std::vector<std::variant<ElementWithNoChildren, ElementWithChildren>> children;
        bool equals(const RenderInfo &rhs) const override;
        Rect getRect(bool include_empty_image) const;
        Rect getRect() const override;
        Region getRegion() const override;
        std::unique_ptr<WrapSurface> getSurface() const override;
        std::unique_ptr<WrapTexture> getTexture(renderer_t *renderer, TextureCache &texture_cache) const override;
    };

    using VElement = std::variant<ElementWithNoChildren, ElementWithChildren>;
}

template<>
struct std::hash<sorakado::ao::master::ElementWithNoChildren> {
    size_t operator ()(const sorakado::ao::master::ElementWithNoChildren &e) const {
        size_t hash = std::hash<std::filesystem::path>()(e.filename);
        if (e.index) {
            hash ^= std::hash<int>()(*e.index + 1);
        }
        return hash;
    }
};

template<>
struct std::equal_to<sorakado::ao::master::ElementWithNoChildren> {
    bool operator()(const sorakado::ao::master::ElementWithNoChildren &x, const sorakado::ao::master::ElementWithNoChildren &y) const {
        return x.filename == y.filename && x.index == y.index;
    }
};

template<>
struct std::hash<sorakado::ao::master::ElementWithChildren> {
    size_t operator ()(const sorakado::ao::master::ElementWithChildren &e) const {
        size_t hash = 0;
        for (int i = 0; i < e.children.size(); i++) {
            hash ^= std::rotl(std::hash<sorakado::ao::master::VElement>()(e.children[i]), i);
            std::visit([&](const auto &v) {
                hash ^= std::hash<int>()((i + 1) * v.x) ^ std::rotl(std::hash<int>()((i + 1) * v.y), 1);
            }, e.children[i]);
        }
        return hash;
    }
};

template<>
struct std::equal_to<sorakado::ao::master::ElementWithChildren> {
    bool operator()(const sorakado::ao::master::ElementWithChildren &x, const sorakado::ao::master::ElementWithChildren &y) const {
        if (x.children.size() != y.children.size()) {
            return false;
        }
        for (int i = 0; i < x.children.size(); i++) {
            if (x.children[i] != y.children[i]) {
                return false;
            }
        }
        return true;
    }
};

template<>
struct std::equal_to<sorakado::ao::master::VElement> {
    bool operator()(const sorakado::ao::master::VElement &x, const sorakado::ao::master::VElement &y) const {
        if (x.index() != y.index()) {
            return false;
        }
        if (std::holds_alternative<sorakado::ao::master::ElementWithNoChildren>(x)) {
            auto a = std::get<sorakado::ao::master::ElementWithNoChildren>(x);
            auto b = std::get<sorakado::ao::master::ElementWithNoChildren>(y);
            return std::equal_to<sorakado::ao::master::ElementWithNoChildren>()(a, b);
        }
        else if (std::holds_alternative<sorakado::ao::master::ElementWithChildren>(x)) {
            auto a = std::get<sorakado::ao::master::ElementWithChildren>(x);
            auto b = std::get<sorakado::ao::master::ElementWithChildren>(y);
            return std::equal_to<sorakado::ao::master::ElementWithChildren>()(a, b);
        }
        return false;
    }
};

#endif // ELEMENT_H_
