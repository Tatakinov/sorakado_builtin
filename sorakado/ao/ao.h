#ifndef SORAKADO_AO_AO_H_
#define SORAKADO_AO_AO_H_

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "sorakado/sorakado.h"
#include "sorakado/util.h"
#include "sorakado/ao/menu.h"
#include "sorakado/ao/misc.h"

namespace sorakado::ao {
    using animation_key_t = std::pair<int, std::string>;
}

template<>
struct std::hash<sorakado::ao::animation_key_t> {
    size_t operator ()(const sorakado::ao::animation_key_t &key) const {
        return std::hash<int>()(key.first) ^ std::hash<std::string>()(key.second);
    }
};

namespace sorakado::ao {
    class Ao : public sorakado::Sorakado {
        private:
            std::unordered_set<animation_key_t> defer_animation_;
        protected:
        public:
            Ao(sorakado::Application *parent, std::filesystem::path dir);
            virtual ~Ao() {}

            std::optional<lib_skeleton::sorakado::Response> sorakadoEventImmediately(const lib_skeleton::sorakado::Request &req) override;
            void sorakadoEvent(const std::vector<std::string> &args) override;

            virtual bool isPlayingAnimation(int side, const std::string &id) const = 0;
            virtual std::unordered_set<int> getActiveAnimationList(int side) const = 0;
            virtual void create(int side) = 0;
            virtual void show(int side) = 0;
            virtual void raise(int side) = 0;
            virtual void hide(int side) = 0;
            virtual void setScale(int value) = 0;
            virtual void clearCache() = 0;
            virtual void createMenu(const std::vector<MenuModelData> &data) = 0;
            virtual std::vector<MenuModelData> getDressUpList() = 0;
            virtual void setSurfaceID(int side, const std::string &id) = 0;
            virtual void startAnimation(int side, const std::string &id) = 0;
            virtual void clearAnimation(int side, const std::string &id) = 0;
            virtual void pauseAnimation(int side, const std::string &id) = 0;
            virtual void resumeAnimation(int side, const std::string &id) = 0;
            virtual void offsetAnimation(int side, const std::string &id, int x, int y) = 0;
            virtual void bind(int side, const std::string &category, const std::string &parts, const std::string &from, BindFlag flag) = 0;
            virtual std::optional<Position> getCharacterPosition(int side) const = 0;
            virtual void move(int side, bool is_async, const std::string &x, const std::string &y, int time, const std::string &base, const std::string &base_offset, const std::string &move_offset, const std::vector<std::string> &options) = 0;
    };
}

#endif // SORAKADO_AO_AO_H_
