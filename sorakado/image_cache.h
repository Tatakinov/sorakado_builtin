#ifndef IMAGE_CACHE_H_
#define IMAGE_CACHE_H_

#include <condition_variable>
#include <filesystem>
#include <mutex>
#if defined(USE_ONNX)
#include <onnxruntime/onnxruntime_cxx_api.h>
#endif // USE_ONNX
#include <optional>
#include <queue>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include <SDL3/SDL_surface.h>

namespace sorakado {
    struct ImagePath {
        std::filesystem::path path;
        // FIXME variant<int, string>
        std::optional<int> index;
        bool operator==(const ImagePath &rhs) const {
            return path == rhs.path && index == rhs.index;
        }
    };
}

template<>
struct std::hash<sorakado::ImagePath> {
    size_t operator ()(const sorakado::ImagePath &p) const {
        size_t hash = std::hash<std::filesystem::path>()(p.path);
        if (p.index) {
            hash ^= std::hash<int>()(p.index.value());
        }
        return hash;
    }
};

namespace sorakado {
    struct RegionData {
        int x, y, len;
        bool operator==(const RegionData &rhs) const {
            return x == rhs.x && y == rhs.y && len == rhs.len;
        }
    };

    using Region = std::vector<RegionData>;

    Region merge(const Region &a, const Region &b);
    Region translate(const Region &r, int x, int y);
    Region subtract(const Region &r, int x, int y, int w, int h);

    class ImageInfo {
        private:
            std::vector<unsigned char> data_;
            int width_, height_;
            bool is_upconverted_;
            Region region_;
        public:
            ImageInfo(const std::vector<unsigned char> &data, int width, int height, bool is_upconverted);
            ~ImageInfo() {}
            std::vector<unsigned char> &get() {
                return data_;
            }
            int width() const {
                return width_;
            }
            int height() const {
                return height_;
            }
            bool isUpconverted() const {
                return is_upconverted_;
            }
            Region region() const {
                return region_;
            }
            bool empty() const {
                return region_.empty();
            }
    };

    class ImageCache {
        private:
            bool alive_;
            bool use_self_alpha_;
            int scale_;
            std::filesystem::path sorakado_dir_;
            std::mutex mutex_;
            std::condition_variable cond_;
            std::unique_ptr<std::thread> th_;
            std::queue<ImagePath> queue_;
            std::unordered_map<ImagePath, std::optional<ImageInfo>> cache_orig_;
            std::unordered_map<ImagePath, std::optional<ImageInfo>> cache_;
#if defined(USE_ONNX)
            Ort::Env env_;
            Ort::Session session_;
#endif // USE_ONNX

            std::optional<ImageInfo> load(const ImagePath &p, SDL_Surface *in);

        public:
#if defined(USE_ONNX)
            ImageCache(const std::filesystem::path &sorakado_dir, const std::filesystem::path &exe_dir, bool use_self_alpha);
#else
            ImageCache(const std::filesystem::path &sorakado_dir, const std::filesystem::path &exe_dir, bool use_self_alpha)
            : alive_(true), use_self_alpha_(use_self_alpha), scale_(100), sorakado_dir_(sorakado_dir) {}
#endif // USE_ONNX
            ~ImageCache();
            void setScale(int scale);
            std::optional<ImageInfo> &get(const std::filesystem::path &path, const std::optional<int> index = std::nullopt);
            std::optional<ImageInfo> &getOriginal(const std::filesystem::path &path, const std::optional<int> index = std::nullopt);
            void clearCache();
    };
}

#endif // IMAGE_CACHE_H_
