#ifndef SORAKADO_AI_AI_H_
#define SORAKADO_AI_AI_H_

#include <memory>
#include <string>
#include <vector>

#include "sorakado/sorakado.h"
#include "sorakado/ai/misc.h"

namespace sorakado::ai {

    class Ai : public sorakado::Sorakado {
        private:
        protected:
        public:
            Ai(sorakado::Application *parent, std::filesystem::path dir);
            virtual ~Ai() {}

            std::optional<lib_skeleton::sorakado::Response> sorakadoEventImmediately(const lib_skeleton::sorakado::Request &req) override;
            void sorakadoEvent(const std::vector<std::string> &args) override;

            virtual void setBalloonID(int side, int id) = 0;
            virtual void resetBalloonID(int side = -1) = 0;
            virtual void setBalloonPosition(int side, int x, int y) = 0;
            virtual void setBalloonDirection(int side, int direction) = 0;
            virtual void create(int side) = 0;
            virtual void show(int side) = 0;
            virtual void raise(int side) = 0;
            virtual void raiseOnTalk(int side) = 0;
            virtual void hide(int side) = 0;
            virtual void hideAll() = 0;
            virtual void setScale(int value) = 0;
            virtual void setFont(const std::string &name) = 0;
            virtual void openInputBox(const std::string &id) = 0;
            virtual void openScriptInputBox() = 0;
            virtual void clearText(int side, bool initialize) = 0;
            virtual void clearTextAll() = 0;
            virtual void clearCache() = 0;
            virtual void appendText(int side, const std::string &text) = 0;
            virtual void appendLinkBegin(int side, bool is_anchor, const std::string &event, const std::vector<std::string> &args) = 0;
            virtual void appendLinkEnd(int side) = 0;
            virtual void setCursorPosition(int side, std::string axis, double value, bool is_absolute, MoveUnit unit) = 0;
            virtual void newLine(int side) = 0;
    };
}

#endif // SORAKADO_AI_AI_H_
