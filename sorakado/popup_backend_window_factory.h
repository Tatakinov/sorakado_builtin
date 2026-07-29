#ifndef SORAKADO_POPUP_BACKEND_WINDOW_FACTORY_H_
#define SORAKADO_POPUP_BACKEND_WINDOW_FACTORY_H_

#include "sorakado/backend_window_factory.h"

namespace sorakado {
    class PopupBackendWindowFactory : public BackendWindowFactory {
        private:
            window_t parent_;
            int x_, y_;
            window_flag_t flag_;
        public:
            PopupBackendWindowFactory(window_t parent, int x, int y, window_flag_t flag) : BackendWindowFactory(), parent_(parent), x_(x), y_(y), flag_(flag) {}
            ~PopupBackendWindowFactory() {}
            window_t create(const char *title, int w, int h) const override;
    };
}

#endif // SORAKADO_POPUP_BACKEND_WINDOW_FACTORY_H_
