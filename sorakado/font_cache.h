#ifndef SORAKADO_FONT_CACHE_H_
#define SORAKADO_FONT_CACHE_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "sorakado/font.h"

namespace sorakado {
    class FontCache {
        private:
            std::unordered_map<std::string, std::unique_ptr<WrapFont>> cache_;
        public:
            FontCache();
            ~FontCache();
            void setDefaultFont(const fontlist::fontfamily &family);
            WrapFont *getDefaultFont();
            WrapFont *get(const std::filesystem::path &path);
    };
}

#endif // SORAKADO_FONT_CACHE_H_
