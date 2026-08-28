#ifndef SORAKADO_AO_SERIKO_H_
#define SORAKADO_AO_SERIKO_H_

#include <queue>
#include <unordered_set>
#include <variant>
#include <vector>

#include "sorakado/ao/master/actor.h"
#include "sorakado/ao/master/surface.h"

namespace sorakado {
    class Character;
}

namespace sorakado::ao::master {
    class Actor;
    class ElementWithChildren;

    struct ActorWithPriority {
        int id;
        int remain;
    };

    struct Compare {
        bool operator()(const ActorWithPriority &a, const ActorWithPriority &b) const {
            return a.remain > b.remain;
        }
    };

    struct CollisionInfo {
        int x, y;
        std::vector<Collision> list;
    };

    class Seriko {
        private:
            int scale_;
            int current_id_;
            std::unordered_map<int, Surface> surfaces_;
            std::unordered_map<int, Actor> actors_;
            std::chrono::system_clock::time_point prev_time_;
            std::priority_queue<ActorWithPriority, std::vector<ActorWithPriority>, Compare> process_;
            Character *parent_;
            std::unordered_map<int, bool> binds_;
            std::unordered_map<int, std::unordered_set<int>> bind_addids_;

            void updateBind();
        public:
            Seriko(const std::unordered_map<int, Surface> &surfaces) : scale_(100), current_id_(-1), surfaces_(surfaces) {}
            ~Seriko() {}
            void setParent(Character *parent) {
                parent_ = parent;
            }
            void update(bool change = false);
            void push(int id, int elapsed);
            bool active(int id);
            bool active(const std::string &id);
            std::unordered_set<int> getActiveAnimationList();
            void activate(From from, const std::string &id, int elapsed);
            void activate(From from, int id, int elapsed);
            void inactivate(const std::string &id);
            void inactivate(int id);
            int getSurfaceID() const;
            bool setSurfaceID(const std::string &id);
            bool setSurfaceID(int id);
            bool getBindDefault(int id);
            std::unordered_set<int> getBindAddIDs(int id);
            ElementWithChildren get();
            std::vector<std::variant<Element, ElementWithChildren>> getElements(int id, std::unordered_set<int> &done);
            std::vector<CollisionInfo> getCollision();
            void bind(int id, bool enable);
            bool isBinding(int id);
            void setScale(int scale);
    };
}

#endif // SORAKADO_AO_SERIKO_H_
