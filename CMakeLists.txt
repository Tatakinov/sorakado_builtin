cmake_minimum_required(VERSION 3.30)
project(sorakado_builtin CXX)

option(ENABLE_ONNXRUNTIME "Enable OnnxRuntime support if available" ON)

set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" "${PROJECT_SOURCE_DIR}/cmake")

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)

set(DEPENDENCIES SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf)

add_subdirectory(libfontlist)
add_subdirectory(sorakado)

add_executable(sorakado_builtin main.cc logger.cc libfontlist sorakado)

set_target_properties(sorakado_builtin PROPERTIES SUFFIX ".exe")

target_compile_options(sorakado_builtin PUBLIC -O2 -Wall)
target_compile_features(sorakado_builtin PUBLIC cxx_std_20)

target_include_directories(sorakado_builtin PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(sorakado_builtin PRIVATE sorakado fontlist ${DEPENDENCIES})
