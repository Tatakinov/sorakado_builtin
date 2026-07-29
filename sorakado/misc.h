#ifndef SORAKADO_MISC_H_
#define SORAKADO_MISC_H_

#include <string>
#include <vector>

namespace sorakado {
    struct Position {
        int x, y;
    };

    struct Rect : public Position {
        int w, h;
    };

    struct DragPosition {
        float x, y;
    };

    struct Color {
        int r, g, b, a;
    };
}

namespace sorakado::directsstp {
    struct Request {
        std::string method;
        std::string command;
        std::vector<std::string> args;
        std::string script;
    };
}

constexpr int BUFFER_SIZE = 1024;

#endif // SORAKADO_MISC_H_
