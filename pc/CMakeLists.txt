cmake_minimum_required(VERSION 3.16)
project(WMW_PC VERSION 0.4.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(FATAL_ERROR "WMW PC port requires 64-bit (x64) build. Generator is ${CMAKE_GENERATOR}")
    endif()
endif()

find_package(SDL2 CONFIG QUIET)
if(NOT SDL2_FOUND)
    find_package(SDL2 REQUIRED)
endif()

find_package(SDL2_image CONFIG QUIET)
if(NOT SDL2_image_FOUND)
    find_package(SDL2_image REQUIRED)
endif()

set(WMW_SOURCES
    src/main.cpp
    src/platform/window.cpp
    src/platform/input.cpp
    src/platform/filesystem.cpp
    src/platform/timer.cpp
    src/platform/audio.cpp
    src/platform/renderer.cpp
    src/platform/splash.cpp
    src/core/engine.cpp
    src/core/assets.cpp
    src/core/level_loader.cpp
    src/core/physics_system.cpp
    src/core/water_system.cpp
    src/core/particle_description.cpp
    src/core/spout.cpp
    src/core/hs_loader.cpp
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

if(TARGET SDL2_image::SDL2_image)
    target_link_libraries(wmw_pc PRIVATE SDL2_image::SDL2_image)
elseif(TARGET SDL2::SDL2_image)
    target_link_libraries(wmw_pc PRIVATE SDL2::SDL2_image)
else()
    target_include_directories(wmw_pc PRIVATE ${SDL2_IMAGE_INCLUDE_DIRS})
    target_link_libraries(wmw_pc PRIVATE ${SDL2_IMAGE_LIBRARIES})
endif()

if(MSVC)
    target_compile_options(wmw_pc PRIVATE /W4 /permissive- /Zc:__cplusplus)
    target_link_options(wmw_pc PRIVATE /SUBSYSTEM:CONSOLE)
else()
    target_compile_options(wmw_pc PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(wmw_pc PROPERTIES OUTPUT_NAME "wmw_pc")

add_custom_command(TARGET wmw_pc POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/Drawing.png"
        "$<TARGET_FILE_DIR:wmw_pc>/Drawing.png"
    COMMENT "Copy Drawing.png (copyright splash) next to wmw_pc"
)

install(TARGETS wmw_pc RUNTIME DESTINATION bin)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/Drawing.png" DESTINATION bin)
