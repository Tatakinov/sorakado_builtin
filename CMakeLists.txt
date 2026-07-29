cmake_minimum_required(VERSION 3.30)
project(sorakado_builtin CXX)

set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" "${PROJECT_SOURCE_DIR}/cmake")

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(wayland-client REQUIRED)
find_package(jsoncpp REQUIRED)

add_subdirectory(libfontlist)
add_subdirectory(sorakado)

add_executable(sorakado_builtin.exe main.cc logger.cc libfontlist sorakado)

target_compile_options(sorakado_builtin.exe PUBLIC -O2 -Wall)
target_compile_features(sorakado_builtin.exe PUBLIC cxx_std_20)

target_include_directories(sorakado_builtin.exe PUBLIC ${PROJECT_SOURCE_DIR})

set(DEPENDENCIES SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf jsoncpp_lib)

if (UNIX)
    set(DEPENDENCIES ${DEPENDENCIES} ${WAYLAND_CLIENT_LIBRARIES})
endif()

if (USE_ONNX)
    find_package(onnxruntime REQUIRED)
    set(DEPENDENCIES ${DEPENDENCIES} onnxruntime::onnxruntime)
endif()

target_link_libraries(sorakado_builtin.exe PRIVATE sorakado fontlist ${DEPENDENCIES})
