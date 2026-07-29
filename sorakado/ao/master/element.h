#ifndef SORAKADO_AO_MASTER_ELEMENT_H_
#define SORAKADO_AO_MASTER_ELEMENT_H_

#include <memory>
#include <variant>

#include "sorakado/render_info.h"
#include "sorakado/ao/misc.h"

namespace sorakado::ao::master {
    struct Element;
    struct ElementWithChildren;

    struct ElementWithChildren : public sorakado::RenderInfo {
        ElementWithChildren(Method _method, int _x, int _y, std::vector<std::variant<Element, ElementWithChildren>> _children) : method(_method), x(_x), y(_y), children(_children) {}
        Method method;
        int x, y;
        std::vector<std::variant<Element, ElementWithChildren>> children;
        bool equals(const RenderInfo &rhs) const override;
        std::unique_ptr<WrapSurface> getSurface(std::unique_ptr<ImageCache> &image_cache) const override;
        std::unique_ptr<WrapTexture> getTexture(std::unique_ptr<ImageCache> &image_cache, renderer_t *renderer, std::unique_ptr<TextureCache> &texture_cache) const override;
    };
}

#endif // ELEMENT_H_
