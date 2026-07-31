#ifndef SORAKADO_AO_MASTER_MISC_H_
#define SORAKADO_AO_MASTER_MISC_H_

namespace sorakado::ao {
    enum class Method {
        Base, Overlay, OverlayFast, OverlayMultiply, Replace,
        Interpolate, Asis, Move, Bind, Add, Reduce, Insert,
        Start, Stop, AlternativeStart, AlternativeStop,
        ParallelStart, ParallelStop, Import,
    };

    enum class Interval {
        Sometimes, Rarely, Random, Periodic, Always, Runonce,
        Never, YenE, Talk, Bind,
    };

    enum class CollisionType {
        Rect, Ellipse, Circle, Polygon, Region,
    };
}

#endif // SORAKADO_AO_MASTER_MISC_H_
