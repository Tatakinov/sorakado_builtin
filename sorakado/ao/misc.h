#ifndef SORAKADO_AO_MISC_H_
#define SORAKADO_AO_MISC_H_

namespace sorakado::ao {
    enum class BindFlag {
        True, False, Toggle,
    };

    enum class From {
        System, Seriko, User, YenE, Talk,
    };

    enum class CursorType {
        Default, Hand,
    };

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

    enum class Alignment {
        Bottom, Top, Free,
    };
}

#endif // SORAKADO_AO_MISC_H_
