#include "seriko.h"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "logger.h"
#include "sorakado/util.h"
#include "sorakado/character.h"
#include "sorakado/ao/master/element.h"

namespace sorakado::ao::master {
    void Seriko::update(bool change) {
        auto now = std::chrono::system_clock::now();
        int elapsed = (change) ? (0) : (std::chrono::duration_cast<std::chrono::milliseconds>(now - prev_time_).count());
        while (!process_.empty()) {
            process_.pop();
        }
        for (auto &[k, v] : actors_) {
            if (change) {
                v.activate(From::System);
            }
            push(k, elapsed);
        }
        while (!process_.empty()) {
            auto [k, t] = process_.top();
            process_.pop();
            actors_.at(k).update(t);
        }
        prev_time_ = now;
    }

    void Seriko::push(int id, int elapsed) {
        if (!actors_.contains(id)) {
            return;
        }
        auto &actor = actors_.at(id);
        if (!actor.active()) {
            return;
        }
        process_.push({id, elapsed});
    }

    bool Seriko::active(const std::string &id) {
        int tmp;
        util::to_x(id, tmp);
        active(tmp);
    }

    bool Seriko::active(int id) {
        if (!actors_.contains(id)) {
            Logger::log("id: ", id, " not found");
            return false;
        }
        return actors_.at(id).active();
    }

    std::unordered_set<int> Seriko::getActiveAnimationList() {
        std::unordered_set<int> list;
        for (auto &[k, _]: actors_) {
            if (active(k)) {
                list.emplace(k);
            }
        }
        for (auto &[k, v]: binds_) {
            if (v) {
                list.emplace(k);
            }
        }
        Logger::log("seriko.active.num", list.size());
        return list;
    }

    void Seriko::activate(From from, const std::string &id, int elapsed) {
        int tmp;
        util::to_x(id, tmp);
        activate(from, tmp, elapsed);
    }

    void Seriko::activate(From from, int id, int elapsed) {
        if (!actors_.contains(id)) {
            Logger::log("animation id: ", id , " not found");
            return;
        }
        auto &actor = actors_.at(id);
        if (actor.active()) {
            Logger::log("animation id: ", id , " already active");
            return;
        }
        actor.activate(from);
        push(id, elapsed);
    }

    void Seriko::inactivate(const std::string &id) {
        int tmp;
        util::to_x(id, tmp);
        inactivate(tmp);
    }

    void Seriko::inactivate(int id) {
        if (!actors_.contains(id)) {
            return;
        }
        auto &actor = actors_.at(id);
        actor.inactivate();
    }

    int Seriko::getSurfaceID() const {
        return current_id_;
    }

    bool Seriko::setSurfaceID(const std::string &id) {
        int tmp;
        util::to_x(id, tmp);
        return setSurfaceID(tmp);
    }

    bool Seriko::setSurfaceID(int id) {
        if (current_id_ == id) {
            return false;
        }
        current_id_ = id;
        if (!surfaces_.contains(id)) {
            return true;
        }
        auto &surface = surfaces_.at(id);
        actors_.clear();
        for (auto &[k, v] : surface.animation) {
            Actor actor = {k, v, this};
            actors_.emplace(k, actor);
        }
        updateBind();
        update(true);
        return true;
    }

    bool Seriko::getBindDefault(int id) {
        std::string key = util::side2str(parent_->side()) + ".bindgroup" + util::to_s(id) + ".default";
        std::string value = parent_->getInfo(key, false);
        if (value.empty()) {
            return false;
        }
        int binding;
        util::to_x(value, binding);
        return binding == 1;
    }

    std::unordered_set<int> Seriko::getBindAddIDs(int id) {
        std::unordered_set<int> addids;
        std::string key = util::side2str(parent_->side()) + ".bindgroup" + util::to_s(id) + ".addid";
        std::string value = parent_->getInfo(key, false);
        if (value.empty()) {
            return addids;
        }
        std::istringstream iss(value);
        std::string tmp;
        while (std::getline(iss, tmp, ',')) {
            int id;
            util::to_x(tmp, id);
            addids.emplace(id);
        }
        return addids;
    }

    ElementWithChildren Seriko::get() {
        if (!surfaces_.contains(current_id_)) {
            return ElementWithChildren(Method::Overlay, 0, 0, {});
        }
        ElementWithChildren ret(Method::Overlay, 0, 0, {});
        update();
        int id = current_id_;
        auto &surface = surfaces_.at(id);
        std::vector<int> list;
        list.reserve(std::max(surface.element.size(), actors_.size()));
        ret.children.reserve(surface.element.size());
        // TODO background
        for (auto &[k, _] : surface.element) {
            list.emplace_back(k);
        }
        std::sort(list.begin(), list.end());
        for (auto i : list) {
            auto element = surface.element[i];
            element.x *= scale_ / 100.0;
            element.y *= scale_ / 100.0;
            ret.children.emplace_back(element);
        }
        list.clear();
        int allocate = ret.children.size();
        for (auto &[k, v] : actors_) {
            list.emplace_back(k);
            allocate += v.patterns().size();
        }
        ret.children.reserve(allocate);
        std::sort(list.begin(), list.end());
        std::unordered_set<int> done = {id};
        for (auto i : list) {
            auto &actor = actors_.at(i);
            auto &interval = actor.interval();
            if (interval.size() == 1 && interval.contains(Interval::Bind)) {
                if (isBinding(i)) {
                    auto ps = actor.patterns();
                    for (auto &p : ps) {
                        ElementWithChildren e(p.method, p.x * scale_ / 100.0, p.y * scale_ / 100.0, getElements(p.id, done));
                        ret.children.emplace_back(e);
                    }
                }
            }
#if 0
            else if (actor.active()) {
                auto p = actor.currentPattern();
                ElementWithChildren e = { p.method, p.x, p.y, getElements(p.id, done) };
                ret.children.emplace_back(e);
            }
#else
            auto p = actor.currentPattern();
            ElementWithChildren e(p.method, p.x * scale_ / 100.0, p.y * scale_ / 100.0, getElements(p.id, done));
            ret.children.emplace_back(e);
#endif
        }
        return ret;
    }

    std::vector<std::variant<Element, ElementWithChildren>> Seriko::getElements(int id, std::unordered_set<int> &done) {
        if (!surfaces_.contains(id)) {
            return {};
        }
        std::vector<std::variant<Element, ElementWithChildren>> ret;
        auto &surface = surfaces_.at(id);
        done.emplace(id);
        // TODO background
        for (auto &[_, v] : surface.element) {
            auto element = v;
            element.x *= scale_ / 100.0;
            element.y *= scale_ / 100.0;
            ret.push_back(v);
        }
        std::vector<int> list;
        for (auto &[k, _] : surface.animation) {
            list.push_back(k);
        }
        std::sort(list.begin(), list.end());
        for (auto i : list) {
            auto &interval = surface.animation[i].interval;
            if (interval.size() == 1 && interval.contains(Interval::Bind)) {
                auto ps = surface.animation[i].pattern;
                for (auto &p : ps) {
                    if (!done.contains(p.id)) {
                        ElementWithChildren e = { p.method, p.x * scale_ / 100.0, p.y * scale_ / 100.0, getElements(p.id, done) };
                        ret.emplace_back(e);
                    }
                }
            }
        }
        return ret;
    }

    std::vector<CollisionInfo> Seriko::getCollision() {
        int id = current_id_;
        if (!surfaces_.contains(id)) {
            return {};
        }
        update();
        std::vector<CollisionInfo> ret;
        // TODO order
        // 当たり判定は定義順の逆順で走査するので判定式も逆
        auto comp = [](const Collision &a, const Collision &b) {
            return a.factor > b.factor;
        };
        auto &surface = surfaces_.at(id);
        // TODO background
        {
            CollisionInfo info = {0, 0, {}};
            // scale変換を行うので参照はしない
            for (auto [_, v] : surface.collision) {
                for (auto &p : v.point) {
                    p *= scale_ / 100.0;
                }
                info.list.push_back(v);
            }
            if (info.list.size() > 0) {
                std::sort(info.list.begin(), info.list.end(), comp);
                ret.push_back(info);
            }
        }
        std::vector<int> keys;
        for (auto &[k, _] : actors_) {
            keys.push_back(k);
        }
        std::sort(keys.begin(), keys.end());
        for (auto i : keys) {
            auto &actor = actors_.at(i);
            auto p = actor.currentPattern();
            int id = p.id;
            if (!surfaces_.contains(id)) {
                continue;
            }
            auto &s = surfaces_.at(id);
            CollisionInfo info = {p.x * scale_ / 100.0, p.y * scale_ / 100.0, {}};
            // scale変換を行うので参照はしない
            for (auto [_, v] : s.collision) {
                for (auto &p : v.point) {
                    p = p * scale_ / 100.0;
                }
                info.list.push_back(v);
            }
            if (info.list.size() > 0) {
                std::sort(info.list.begin(), info.list.end(), comp);
                ret.push_back(info);
            }
        }
        // ここも逆順にする
        std::reverse(ret.begin(), ret.end());
        return ret;
    }


    void Seriko::bind(int id, bool enable) {
        if (!actors_.contains(id)) {
            return;
        }
        binds_[id] = enable;
        if (enable) {
            Logger::log("bind enable", id);
            actors_.at(id).activate(From::System);
        }
        else {
            actors_.at(id).inactivate();
        }
        auto addids = getBindAddIDs(id);
        for (auto e : addids) {
            if (enable) {
                bind_addids_[e].emplace(id);
                bind(e, true);
            }
            else if (bind_addids_.contains(id)) {
                bind_addids_.at(e).erase(id);
                if (bind_addids_.at(e).size() == 0) {
                    bind_addids_.erase(e);
                    bind(e, false);
                }
            }
        }
    }

    bool Seriko::isBinding(int id) {
        if (bind_addids_.contains(id)) {
            return true;
        }
        return binds_[id];
    }

    void Seriko::updateBind() {
        for (auto &[k, _] : actors_) {
            if (!binds_.contains(k)) {
                binds_[k] = getBindDefault(k);
                Logger::log("updateBind: ", k, binds_[k]);
                assert(binds_.contains(k));
                bind(k, binds_.at(k));
            }
        }
    }

    void Seriko::setScale(int scale) {
        if (scale_ != scale) {
            scale_ = scale;
        }
    }
}
