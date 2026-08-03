#include <cstdlib>
#include <vector>

#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "sorakado/application.h"
#include "logger.h"

int main(int argc, char **argv) {
    SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
    SDL_SetHint("SDL_BORDERLESS_WINDOWED_STYLE", "0");
    SDL_SetHint(SDL_HINT_APP_ID, "io.github.tatakinov.ninix-kagari.sorakado.sorakado_builtin");
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, "io.github.tatakinov.ninix-kagari.sorakado.sorakado_builtin");
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return 1;
    }
    atexit(SDL_Quit);
    if (!TTF_Init()) {
        return 1;
    }
    atexit(TTF_Quit);

    sorakado::Application app(argc, argv);

    while (app) {
        app.run();
    }

	return 0;
}
