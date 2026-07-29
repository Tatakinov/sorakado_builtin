#include "sorakado/popup_backend_window_factory.h"

namespace sorakado {
    window_t PopupBackendWindowFactory::create(const char *title, int w, int h) const {
        return SDL_CreatePopupWindow(parent_, x_, y_, w, h, flag_);
    }
}
