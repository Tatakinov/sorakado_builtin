#ifndef SURFACE_H_
#define SURFACE_H_

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "sorakado/render_info.h"
#include "sorakado/texture.h"
#include "sorakado/ao/master/misc.h"

namespace sorakado {
    class ImageCache;
}

namespace sorakado::ao::master {
    struct Element {
        Element() : Element(Method::Overlay, 0, 0, "", std::nullopt) {}
        Element(Method _method, int _x, int _y, std::filesystem::path _filename, std::optional<int> _index) : method(_method), x(_x), y(_y), filename(_filename), index(_index) {}
        Method method;
        int x, y;
        std::filesystem::path filename;
        std::optional<int> index;
        Rect getRect(std::unique_ptr<ImageCache> &image_cache, bool include_empty_image) const;
        std::unique_ptr<WrapSurface> getSurface(std::unique_ptr<ImageCache> &image_cache) const;
        std::unique_ptr<WrapTexture> getTexture(std::unique_ptr<ImageCache> &image_cache, renderer_t *renderer, std::unique_ptr<TextureCache> &texture_cache) const;
        bool operator==(const Element &rhs) const;
    };

    struct Pattern {
        Method method;
        int index, id, wait_min, wait_max, x, y;
        std::vector<int> ids;
    };

    struct Animation {
        std::unordered_set<Interval> interval;
        int interval_factor;
        std::vector<Pattern> pattern;
        std::optional<std::vector<int>> exclusive;
        bool background;
        bool shared_index;
    };

    struct Collision {
        int factor;
        CollisionType type;
        std::string id;
        std::vector<int> point;
    };

    struct Surface {
        std::unordered_map<int, Element> element;
        std::unordered_map<int, Animation> animation;
        std::unordered_map<int, Collision> collision;
        void merge(const Surface &other) {
            for (auto &[k, v] : other.element) {
                element.insert_or_assign(k, v);
            }
            for (auto &[k, v] : other.animation) {
                animation.insert_or_assign(k, v);
            }
            for (auto &[k, v] : other.collision) {
                collision.insert_or_assign(k, v);
            }
        }
    };
}

#endif // SURFACE_H_
