#ifndef SORAKADO_AI_MASTER_WINDOW_H_
#define SORAKADO_AI_MASTER_WINDOW_H_

#include "sorakado/misc.h"

#include <memory>

#include "sorakado/backend_window_factory.h"
#include "sorakado/window.h"
#include "sorakado/window_factory.h"

namespace sorakado {
    class BaseCharacter;
}

namespace sorakado::ai::master {
    class AiMasterWindowFactory : public WindowFactory {
        public:
            std::unique_ptr<sorakado::Window> create(sorakado::BaseCharacter *parent, display_t id, std::unique_ptr<BackendWindowFactory> &factory, const std::string &name) const override;
    };

    class AiMasterWindow : public Window {
        private:
        public:
            AiMasterWindow(sorakado::BaseCharacter *parent, display_t id, std::unique_ptr<BackendWindowFactory> &factory, const std::string &name);
            ~AiMasterWindow();

            bool input(sorakado::window_id_t id, const std::string &text) override;
            bool edit(sorakado::window_id_t id, const std::string &text) override;
            bool wheel(sorakado::window_id_t id, float x, float y) override;
    };
}

#endif // SORAKADO_AI_MASTER_WINDOW_H_
