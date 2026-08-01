#ifndef SORAKADO_AO_MASTER_WINDOW_H_
#define SORAKADO_AO_MASTER_WINDOW_H_

#include "sorakado/misc.h"

#include <memory>

#include "sorakado/backend_window_factory.h"
#include "sorakado/window.h"
#include "sorakado/window_factory.h"

namespace sorakado {
    class BaseCharacter;
}

namespace sorakado::ao::master {

    class AoMasterWindowFactory : public sorakado::WindowFactory {
        public:
            AoMasterWindowFactory() {}
            ~AoMasterWindowFactory() {}
            std::unique_ptr<Window> create(sorakado::BaseCharacter *parent, display_t id, std::unique_ptr<BackendWindowFactory> &factory, const std::string &name) const override;
    };

    class AoMasterWindow : public Window {
        private:
        public:
            AoMasterWindow(sorakado::BaseCharacter *parent, display_t id, std::unique_ptr<BackendWindowFactory> &factory, const std::string &name);
            ~AoMasterWindow();

            bool key(sorakado::window_id_t id, sorakado::key_t key, bool down) override;
            bool input(sorakado::window_id_t id, const std::string &text) override;
            bool edit(sorakado::window_id_t id, const std::string &text) override;
            bool wheel(sorakado::window_id_t id, float x, float y) override;
            bool drop(sorakado::window_id_t id, const std::vector<std::string> &list) override;
            bool maximized(window_id_t id) override;
    };
}

#endif // SORAKADO_AO_MASTER_WINDOW_H_
