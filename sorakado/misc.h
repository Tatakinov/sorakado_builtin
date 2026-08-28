#ifndef SORAKADO_MISC_H_
#define SORAKADO_MISC_H_

#include <string>
#include <vector>

#include "sorakado/compatible.h"

namespace sorakado {
    enum class SorakadoType {
        Unknown, Ao, Ai,
    };

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
        byte_t r, g, b, a;
        bool operator==(const Color &l) const {
            return r == l.r && g == l.g && b == l.b && a == l.a;
        }
    };

    namespace directsstp {
        struct Request {
            std::string method;
            std::string command;
            std::vector<std::string> args;
            std::string script;
            bool hide_on_204;
            // use only on EXECUTE
            int side;
        };

        struct RequestCache {
            int side;
            std::string command;
            std::vector<Request> req;
        };
    }
}

#endif // SORAKADO_MISC_H_
