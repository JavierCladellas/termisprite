add_library( termisprite_lib termisprite.cpp
                             tools.cpp
                             editor.cpp
                             colorpicker.cpp
                             menu.cpp
                             sprite.cpp
                             statusbar.cpp
                             sprite_io.cpp )

target_link_libraries(termisprite_lib PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(termisprite_lib PUBLIC ftxui::screen ftxui::dom ftxui::component)

add_executable( termisprite main.cpp )


target_link_libraries(termisprite PRIVATE termisprite_lib)
