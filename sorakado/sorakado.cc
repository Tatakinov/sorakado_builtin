#include "sorakado/sorakado.h"

#include "lib_skeleton/sstp.h"
#include "sorakado/application.h"
#include "logger.h"
#include "sorakado/util.h"

namespace sorakado {
    Sorakado::Sorakado(Application *parent, std::filesystem::path dir) : parent_(parent) {
        std::string descript = util::readDescript(dir / "descript.txt");
        std::istringstream iss(descript);
        std::string value;
        while (std::getline(iss, value, '\n')) {
            auto pos = value.find(',');
            if (pos == std::string::npos) {
                continue;
            }
            auto key = value.substr(0, pos);
            value = value.substr(pos + 1);
            descript_info_[key] = value;
        }
    }

    std::string Sorakado::getInfo(std::string key, bool fallback, bool freeze) {
        if (descript_info_.contains(key)) {
            return descript_info_.at(key);
        }
        if (fallback) {
            auto res = lib_skeleton::sstp::Response::parse(sendDirectSSTP("EXECUTE", "GetDescript", {key}, {}, false));
            std::string content = res.getContent();
            if (freeze) {
                descript_info_[key] = content;
            }
            if (content.empty()) {
                Logger::log("info(", key, "): not found");
                return "";
            }
            return content;
        }
        return "";
    }

    void Sorakado::run() {
    }

    std::string Sorakado::sendDirectSSTP(std::string method, std::string command, std::vector<std::string> args, std::string script, bool hide_on_204) {
        return parent_->sendDirectSSTP(method, command, args, script, hide_on_204);
    }

    void Sorakado::enqueueDirectSSTP(std::vector<directsstp::Request> list) {
        parent_->enqueueDirectSSTP(list);
    }
}
