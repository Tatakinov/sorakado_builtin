#include "sorakado/window_manager.h"

#include "sorakado/window_factory.h"

namespace sorakado {
    WindowManager::WindowManager(std::unique_ptr<WindowFactory> factory, std::unique_ptr<BackendWindowFactory> backend_factory) : factory_(std::move(factory)), backend_factory_(std::move(backend_factory)) {
    }
    WindowManager::~WindowManager() {
    }
}
