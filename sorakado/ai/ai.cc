#include "sorakado/ai/ai.h"

#include "sorakado/util.h"

namespace sorakado::ai {
    Ai::Ai(sorakado::Application *parent, std::filesystem::path dir) : sorakado::Sorakado(parent, dir) {
    }
    
    std::optional<lib_skeleton::sorakado::Response> Ai::sorakadoEventImmediately(const lib_skeleton::sorakado::Request &req) {
        return std::nullopt;
    }

    void Ai::sorakadoEvent(const std::vector<std::string> &args) {
        if (args[0] == "Create" && args.size() == 2) {
            int side;
            util::to_x(args[1], side);
            create(side);
        }
        else if (args[0] == "Show" && args.size() == 2) {
            int side;
            util::to_x(args[1], side);
            show(side);
        }
        else if (args[0] == "SetBalloonID" && args.size() == 3) {
            int side, id;
            util::to_x(args[1], side);
            util::to_x(args[2], id);
            setBalloonID(side, id);
        }
        else if (args[0] == "ResetBalloonID") {
            int side = -1;
            if (args.size() == 2) {
                util::to_x(args[1], side);
            }
            resetBalloonID(side);
        }
        else if (args[0] == "ConfigurationChanged") {
            for (int i = 1; i < args.size(); i++) {
                auto value = args[i];
                auto pos = value.find(',');
                if (pos == std::string::npos) {
                    continue;
                }
                auto key = value.substr(0, pos);
                value = value.substr(pos + 1);
                if (key == "scale") {
                    int v;
                    util::to_x(value, v);
                    setScale(v);
                }
                if (key == "font") {
                    setFont(value);
                }
            }
        }
        else if (args[0] == "SetPosition" && args.size() == 6) {
            int side, x, y, mx, my;
            util::to_x(args[1], side);
            util::to_x(args[2], x);
            util::to_x(args[3], y);
            util::to_x(args[4], mx);
            util::to_x(args[5], my);
            if (!util::isWayland() || getenv("NINIX_ENABLE_MULTI_MONITOR")) {
                x += mx;
                y += my;
            }
            setBalloonPosition(side, x, y);
        }
        else if (args[0] == "SetDirection" && args.size() == 3) {
            int side, direction;
            util::to_x(args[1], side);
            util::to_x(args[2], direction);
            setBalloonDirection(side, direction);
        }
        else if (args[0] == "AppendText" && args.size() == 3) {
            int side;
            util::to_x(args[1], side);
            appendText(side, args[2]);
        }
        else if (args[0] == "AppendLinkBegin" && args.size() >= 4) {
            int side;
            util::to_x(args[1], side);
            bool is_anchor = args[2] == "true";
            std::string event = args[3];
            std::vector<std::string> a;
            for (int i = 4; i < args.size(); i++) {
                a.push_back(args[i]);
            }
            appendLinkBegin(side, is_anchor, event, a);
        }
        else if (args[0] == "AppendLinkEnd" && args.size() == 2) {
            int side;
            util::to_x(args[1], side);
            appendLinkEnd(side);
        }
        else if (args[0] == "SetCursorPosition" && args.size() == 6) {
            int side;
            std::string axis;
            double value;
            bool is_absolute;
            MoveUnit unit = MoveUnit::Px;
            util::to_x(args[1], side);
            axis = args[2];
            util::to_x(args[3], value);
            is_absolute = args[4] == "true";
            if (args[5] == "px") {
                unit = MoveUnit::Px;
            }
            else if (args[5] == "em") {
                unit = MoveUnit::Em;
            }
            else if (args[5] == "lh") {
                unit = MoveUnit::Lh;
            }
            setCursorPosition(side, axis, value, is_absolute, unit);
        }
        else if (args[0] == "NewLine" && args.size() == 2) {
            int side;
            util::to_x(args[1], side);
            newLine(side);
        }
        else if (args[0] == "HideAll") {
            hideAll();
        }
        else if (args[0] == "Hide" && args.size() == 2) {
            int side;
            util::to_x(args[1], side);
            hide(side);
        }
        else if (args[0] == "ClearTextAll") {
            clearTextAll();
        }
        else if (args[0] == "ClearText" && args.size() == 2) {
            int side;
            util::to_x(args[1], side);
            clearText(side, false);
        }
        else if (args[0] == "OpenInputBox" && args.size() >= 2) {
            // TODO timeout, text, etc
            openInputBox(args[1]);
        }
        else if (args[0] == "OpenScriptInputBox" && args.size() == 1) {
            openScriptInputBox();
        }
        else if (args[0] == "OnScopeChange") {
            int side;
            util::to_x(args[1], side);
            raiseOnTalk(side);
        }
        else if (args[0] == "OnScriptBegin") {
        }
        else if (args[0] == "OnScriptEnd") {
        }
        else if (args[0] == "Raise") {
            int side;
            util::to_x(args[1], side);
            raise(side);
        }
    }
}
