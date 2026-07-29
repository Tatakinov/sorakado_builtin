#ifndef SORAKADO_BACKEND_WINDOW_FACTORY_H_
#define SORAKADO_BACKEND_WINDOW_FACTORY_H_

#include "sorakado/compatible.h"

namespace sorakado {
    class BackendWindowFactory {
        public:
            BackendWindowFactory() {}
            virtual ~BackendWindowFactory() {}
            virtual window_t create(const char *title, int w, int h) const = 0;
    };
}

#endif // SORAKADO_BACKEND_WINDOW_FACTORY_H_
