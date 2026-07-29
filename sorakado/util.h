#ifndef UTIL_H_
#define UTIL_H_

#include <filesystem>
#include <sstream>
#include <string>

#include <SDL3/SDL_video.h>

namespace util {
    template<typename T>
    void to_x(std::string s, T &value) {
        std::istringstream iss(s);
        iss >> value;
    }

    template<typename T>
    std::string to_s(T value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    double random();
    int random(int a, int b);

    std::string side2str(int side);
    int balloon2id(int balloon_id, bool direction);
    std::string balloonSide2str(int side, int balloon_id, bool direction);

    bool isWayland();
    bool isX11();

    SDL_DisplayID getNearestDisplay(int x, int y);

    std::string readDescript(std::filesystem::path path);

    std::vector<std::string> UTF8Split(const std::string str);
}

#endif // UTIL_H_
