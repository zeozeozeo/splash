cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(splashgame LANGUAGES C CXX VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING INTERNAL FORCE)
if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

# enable LTO in release
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

# packages
set(BOX2D_BUILD_STATIC ON)
add_subdirectory(thirdparty/Box2D)
add_subdirectory(thirdparty/raylib)

# add executable
set(SOURCES
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE thirdparty)

# link
target_link_libraries(${PROJECT_NAME} raylib Box2D)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
