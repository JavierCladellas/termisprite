add_library( termisprite_lib termisprite.cpp
                             tool.cpp
                             tools_section.cpp
                             editor.cpp
                             colorpicker.cpp
                             menu.cpp
                             sprite.cpp
                             statusbar.cpp
                             sprite_io.cpp
                             shortcuts.cpp
                             tool_settings.cpp
                             colorpalettes.cpp
                             layers.cpp
                             grid.cpp
                             cursor.cpp
                             selection_tool.cpp
                             eyedropper_tool.cpp
                             paint_tool.cpp
                             clipboard.cpp
                             brush_tool.cpp
                             shape_tool.cpp
                             geometry.cpp
                             modals.cpp )

target_link_libraries(termisprite_lib PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(termisprite_lib PUBLIC ftxui::screen ftxui::dom ftxui::component)

add_executable( termisprite main.cpp )

target_link_libraries(termisprite PRIVATE termisprite_lib)

#Install
include(GNUInstallDirs)
install(TARGETS termisprite
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )
