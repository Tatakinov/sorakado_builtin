#include "sorakado/ao/ao.h"

#include "sorakado/util.h"

namespace sorakado::ao {
    Ao::Ao(sorakado::Application *parent, std::filesystem::path dir) : sorakado::Sorakado(parent, dir) {
    }

    std::optional<lib_skeleton::sorakado::Response> Ao::sorakadoEventImmediately(const lib_skeleton::sorakado::Request &req) {
        if (req().value() == "IsPlayingAnimation" && req(0) && req(1)) {
            int side;
            util::to_x(req(0).value(), side);
            lib_skeleton::sorakado::Response res = {200, "OK"};
            bool playing = isPlayingAnimation(side, req(1).value());
            res() = static_cast<int>(playing);
        }
        if (req().value() == "GetActiveAnimationList" && req(0)) {
            int side;
            util::to_x(req(0).value(), side);
            lib_skeleton::sorakado::Response res = {200, "OK"};
            auto list = getActiveAnimationList(side);
            std::ostringstream oss;
            for (auto id : list) {
                oss << id << ",";
            }
            auto s = oss.str();
            res() = s.substr(0, s.length() - 1);
        }
        return std::nullopt;
    }

    void Ao::sorakadoEvent(const std::vector<std::string> &args) {
        if (args[0] == "Create") {
            int side;
            util::to_x(args[1], side);
            create(side);
        }
        else if (args[0] == "Show") {
            int side;
            util::to_x(args[1], side);
            show(side);
        }
        else if (args[0] == "SetSurfaceID") {
            int side;
            util::to_x(args[1], side);
            setSurfaceID(side, args[2]);
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
                    clearCache();
                }
            }
        }
        else if (args[0] == "NotifyMenuInfo" && args.size() == 2) {
            Json::Reader reader;
            Json::Value value;
            reader.parse(args[1], value);
            auto data = parseMenuInfo(value, getDressUpList());
            createMenu(data);
        }
        else if (args[0] == "InvokeAnimation" && args.size() >= 4) {
            int side, x = 0, y = 0;
            util::to_x(args[1], side);
            if (args.size() == 6) {
                util::to_x(args[4], x);
                util::to_x(args[5], y);
            }
            if (args[3] == "start") {
                startAnimation(side, args[2]);
            }
        }
        else if (args[0] == "Bind") {
            for (int i = 0; i + 5 < args.size(); i += 5) {
                int side;
                BindFlag flag = BindFlag::Toggle;
                auto arg = args[i + 5];
                if (arg == "true") {
                    flag = BindFlag::True;
                }
                else if (arg == "false") {
                    flag = BindFlag::False;
                }
                util::to_x(args[i + 1], side);
                bind(side, args[i + 2], args[i + 3], args[i + 4], flag);
            }
        }
        else if (args[0] == "OnScopeChange") {
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
