#ifndef SORAKADO_WATCHER_H_
#define SORAKADO_WATCHER_H_

namespace sorakado {
    class Watcher {
        private:
            bool changed_;
        public:
            Watcher() : changed_(false) {}
            virtual ~Watcher() {}
            void change() {
                changed_ = true;
            }
            void update() {
                changed_ = false;
            }
            bool changed() const {
                return changed_;
            }
    };
}

#endif // SORAKADO_WATCHER_H_
