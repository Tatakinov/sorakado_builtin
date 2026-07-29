#include "sorakado/sorakado_factory.h"

#include <cassert>

#include "sorakado/ao/master/ao.h"
#include "sorakado/ai/master/ai.h"

namespace sorakado {
    std::unique_ptr<Sorakado> SorakadoFactory::create(SorakadoType type, Application *parent, std::filesystem::path dir) {
        switch (type) {
            case SorakadoType::Ao:
                return std::make_unique<ao::master::Ao>(parent, dir);
            case SorakadoType::Ai:
                return std::make_unique<ai::master::Ai>(parent, dir);
            default:
                assert(false);
                break;
        }
    }
}
