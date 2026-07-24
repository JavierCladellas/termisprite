cmake_minimum_required(VERSION 3.21.0)

project( termisprite VERSION 0.0.0 )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#DEPS
include(FetchContent)

FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
    GIT_TAG v7.0.0
    EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(ftxui)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(json)

add_subdirectory( src )

set(CPACK_PACKAGE_FILE_NAME "termisprite-${PROJECT_VERSION}-Linux")
include(CPack)

