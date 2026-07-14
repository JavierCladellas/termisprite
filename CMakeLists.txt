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
)

FetchContent_MakeAvailable(ftxui)


add_subdirectory( src )
