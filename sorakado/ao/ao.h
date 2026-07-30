#ifndef SORAKADO_AO_AO_H_
#define SORAKADO_AO_AO_H_

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "sorakado/sorakado.h"
#include "sorakado/ao/misc.h"
#include "sorakado/ao/menu.h"

#include "sorakado/util.h"

namespace sorakado::ao {
    class Ao : public sorakado::Sorakado {
        private:
        protected:
        public:
            Ao(sorakado::Application *parent, std::filesystem::path dir);
            virtual ~Ao() {}

            std::optional<lib_skeleton::sorakado::Response> sorakadoEventImmediately(const lib_skeleton::sorakado::Request &req) override;
            void sorakadoEvent(const std::vector<std::string> &args) override;

            virtual bool isPlayingAnimation(int side, std::string id) const = 0;
            virtual std::unordered_set<int> getActiveAnimationList(int side) const = 0;
            virtual void create(int side) = 0;
            virtual void show(int side) = 0;
            virtual void raise(int side) = 0;
            virtual void hide(int side) = 0;
            virtual void setScale(int value) = 0;
            virtual void clearCache() = 0;
            virtual void createMenu(const std::vector<MenuModelData> &data) = 0;
            virtual std::vector<MenuModelData> getDressUpList() = 0;
            virtual void setSurfaceID(int side, std::string id) = 0;
            virtual void startAnimation(int side, std::string id) = 0;
            virtual void bind(int side, std::string category, std::string parts, std::string from, BindFlag flag) = 0;
            virtual std::optional<Position> getCharacterPosition(int side) const = 0;
    };
}

#endif // SORAKADO_AO_AO_H_
