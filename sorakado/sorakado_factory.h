#ifndef SORAKADO_SORAKADO_FACTORY_H_
#define SORAKADO_SORAKADO_FACTORY_H_

#include <filesystem>
#include <memory>

namespace sorakado {
    class Sorakado;
    class Application;

    enum class SorakadoType {
        Unknown, Ao, Ai,
    };

    class SorakadoFactory {
        public:
            static std::unique_ptr<Sorakado> create(SorakadoType type, Application *parent, std::filesystem::path dir);
    };
}

#endif // SORAKADO_SORAKADO_FACTORY_H_
