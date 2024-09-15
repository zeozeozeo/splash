cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(splashgame LANGUAGES C CXX VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)

# packages
set(BOX2D_BUILD_UNIT_TESTS OFF)
set(BOX2D_BUILD_TESTBED OFF)
set(BOX2D_BUILD_DOCS OFF)
add_subdirectory(thirdparty/box2d)
add_subdirectory(thirdparty/raylib)
add_subdirectory(thirdparty/spdlog)

# add executable
set(SOURCES
    src/main.cpp
    src/world/World.cpp
    src/world/Entity.cpp
    src/world/Player.cpp
    src/world/ContactListener.cpp
    src/debug/b2DrawRayLib.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# link
target_link_libraries(${PROJECT_NAME} raylib box2d spdlog::spdlog)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
