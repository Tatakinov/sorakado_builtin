#include "sorakado/default_backend_window_factory.h"

namespace sorakado {
    window_t DefaultBackendWindowFactory::create(const char *title, int w, int h) const {
        return SDL_CreateWindow(title, w, h, flag_);
    }
}
