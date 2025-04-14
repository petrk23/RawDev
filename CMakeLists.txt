## RawDev project root CMake file

cmake_minimum_required(VERSION 3.20)
project(RawDev VERSION 2.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(NoInSourceBuilds)

find_package(OpenMP REQUIRED)
if(NOT OpenMP_FOUND)
    message(FATAL_ERROR "OpenMP support is required for this project.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

include(EnableIPO)
if(MSVC)
    add_compile_options(/permissive- /W4 /sdl /fp:precise
        "$<$<CONFIG:Release>:/Gy;/Oy;/Oi;/Ot>")
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

include_directories(src)
add_subdirectory(src)

## Main executable
add_executable(RawDev src/Main.cpp)
target_link_libraries(RawDev PRIVATE RawDevLib)
install(TARGETS RawDev)

enable_testing()
add_subdirectory(test)
