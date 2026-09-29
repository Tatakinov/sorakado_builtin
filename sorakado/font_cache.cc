#include "sorakado/font_cache.h"

namespace sorakado {
    FontCache::FontCache() {
        std::unique_ptr<WrapFont> invalid;
        cache_["invalid"] = std::move(invalid);
    }

    FontCache::~FontCache() {
        cache_.clear();
    }

    void FontCache::setDefaultFont(const fontlist::fontfamily &family) {
        cache_["default"] = std::make_unique<WrapFont>(family);
    }

    WrapFont *FontCache::getDefaultFont() {
        if (!cache_.contains("default")) {
            return cache_.at("invalid").get();
        }
        return cache_.at("default").get();
    }

    WrapFont *FontCache::get(const std::filesystem::path &path) {
        if (cache_.contains(path.string())) {
            return cache_.at(path.string()).get();
        }
        cache_[path.string()] = std::make_unique<WrapFont>(path);
        return cache_.at(path.string()).get();
    }
}
