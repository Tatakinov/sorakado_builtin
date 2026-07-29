#ifndef SORAKADO_WINDOW_FACTORY_H_
#define SORAKADO_WINDOW_FACTORY_H_

#include <memory>
#include <string>

#include "backend_window_factory.h"

namespace sorakado {
    class BaseCharacter;
    class Window;
    class WindowFactory {
        public:
            WindowFactory() {}
            virtual ~WindowFactory() {}
            virtual std::unique_ptr<Window> create(BaseCharacter *parent, display_t id, std::unique_ptr<BackendWindowFactory> &factory, const std::string &name) const = 0;
    };
}

#endif // SORAKADO_WINDOW_FACTORY_H_
