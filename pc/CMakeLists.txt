cmake_minimum_required(VERSION 3.16)
project(WMW_PC VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Force x64 on MSVC when possible
if(MSVC)
    if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(FATAL_ERROR "WMW PC port requires 64-bit (x64) build. Generator is ${CMAKE_GENERATOR}")
    endif()
endif()

# ---------------------------------------------------------------------------
# SDL2
# ---------------------------------------------------------------------------
# Prefer CMake package (vcpkg / system). Fallback to SDL2_DIR.
find_package(SDL2 CONFIG QUIET)
if(NOT SDL2_FOUND)
    find_package(SDL2 REQUIRED)
endif()

set(WMW_SOURCES
    src/main.cpp
    src/platform/window.cpp
    src/platform/input.cpp
    src/platform/filesystem.cpp
    src/platform/timer.cpp
    src/platform/audio.cpp
    src/platform/renderer.cpp
    src/core/engine.cpp
    src/core/assets.cpp
    src/core/level_loader.cpp
    src/core/physics_system.cpp
    src/core/water_system.cpp
)

add_executable(wmw_pc ${WMW_SOURCES})

target_include_directories(wmw_pc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(TARGET SDL2::SDL2)
    target_link_libraries(wmw_pc PRIVATE SDL2::SDL2)
    if(TARGET SDL2::SDL2main)
        target_link_libraries(wmw_pc PRIVATE SDL2::SDL2main)
    endif()
else()
    target_include_directories(wmw_pc PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(wmw_pc PRIVATE ${SDL2_LIBRARIES})
endif()

if(MSVC)
    target_compile_options(wmw_pc PRIVATE /W4 /permissive- /Zc:__cplusplus)
    # Windows subsystem console so we see logs during research builds
    target_link_options(wmw_pc PRIVATE /SUBSYSTEM:CONSOLE)
else()
    target_compile_options(wmw_pc PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Output name always wmw_pc(.exe on Windows)
set_target_properties(wmw_pc PROPERTIES OUTPUT_NAME "wmw_pc")

install(TARGETS wmw_pc RUNTIME DESTINATION bin)
