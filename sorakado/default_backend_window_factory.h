#ifndef SORAKADO_DEFAULT_BACKEND_WINDOW_FACTORY_H_
#define SORAKADO_DEFAULT_BACKEND_WINDOW_FACTORY_H_

#include "sorakado/backend_window_factory.h"

namespace sorakado {
    class DefaultBackendWindowFactory  : public BackendWindowFactory{
        private:
            window_flag_t flag_;
        public:
            DefaultBackendWindowFactory(window_flag_t flag) : flag_(flag) {}
            ~DefaultBackendWindowFactory() {}
            window_t create(const char *title, int w, int h) const override;
    };
}

#endif // SORAKADO_DEFAULT_BACKEND_WINDOW_FACTORY_H_
