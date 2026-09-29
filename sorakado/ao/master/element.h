#ifndef SORAKADO_AO_MASTER_ELEMENT_H_
#define SORAKADO_AO_MASTER_ELEMENT_H_

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
}

#endif // ELEMENT_H_
