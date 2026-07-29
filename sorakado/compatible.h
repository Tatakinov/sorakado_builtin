#ifndef SORAKADO_COMPATIBLE_H_
#define SORAKADO_COMPATIBLE_H_

#include <memory>

#include <SDL3/SDL.h>

namespace sorakado {
    class WrapSurface;
    using button_t = Uint8;
    using display_t = SDL_DisplayID;
    using key_t = SDL_Keycode;
    using window_id_t = SDL_WindowID;
    using region_t = std::unique_ptr<WrapSurface>;
    using renderer_t = SDL_Renderer;
    using window_flag_t = SDL_WindowFlags;
    using window_t = SDL_Window *;
}

#endif // SORAKADO_COMPATIBLE_H_
