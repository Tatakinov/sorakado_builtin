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

    enum class Alignment {
        Bottom, Top, Free,
    };
}

#endif // SORAKADO_AO_MISC_H_
