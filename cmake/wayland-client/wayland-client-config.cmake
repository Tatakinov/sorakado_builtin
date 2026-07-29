find_package(PkgConfig)
pkg_check_modules(PKG_WAYLAND QUIET wayland-client)
find_path(WAYLAND_CLIENT_INCLUDE_DIR NAMES wayland-client.h HINTS ${PKG_WAYLAND_INCLUDE_DIRS})
find_library(WAYLAND_CLIENT_LIBRARIES NAMES wayland-client HINTS ${PKG_WAYLAND_LIBRARY_DIRS})

