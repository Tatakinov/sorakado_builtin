add_library(sorakado window.cc character.cc multiple_window_manager.cc single_window_manager.cc application.cc base_character.cc image_cache.cc texture.cc texture_cache.cc default_backend_window_factory.cc popup_backend_window_factory.cc window_manager.cc util.cc font.cc sorakado.cc font_cache.cc sorakado_factory.cc ai/base_inputbox.cc ai/inputbox_window.cc ai/inputbox_info.cc ai/inputbox.cc ai/script_inputbox.cc ai/ai.cc ai/master/character.cc ai/master/ai.cc ai/master/render_info.cc ai/master/window.cc ao/context_menu.cc ao/menu_window.cc ao/menu.cc ao/ao.cc ao/master/character.cc ao/master/ao.cc ao/master/window.cc ao/master/surface.cc ao/master/element.cc ao/master/surfaces.cc ao/master/seriko.cc ao/master/actor.cc)

target_compile_options(sorakado PUBLIC -O2 -Wall)
target_compile_features(sorakado PUBLIC cxx_std_20)

if (USE_ONNX)
    target_compile_definitions(sorakado PRIVATE USE_ONNX)
endif()

target_include_directories(sorakado PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(sorakado jsoncpp_lib)
