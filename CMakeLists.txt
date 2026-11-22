cmake_minimum_required(VERSION 3.20)

project(My3DGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

FetchContent_MakeAvailable(raylib)

add_executable(My3DGame
    src/main.cpp
)

target_link_libraries(My3DGame PRIVATE raylib)

if(WIN32)
    target_link_libraries(My3DGame PRIVATE winmm)
endif()
