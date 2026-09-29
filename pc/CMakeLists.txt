cmake_minimum_required(VERSION 3.16)
project(WMW_PC VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Prefer static linking on Windows later; for now dynamic SDL2 on Linux CI
find_package(SDL2 REQUIRED)

add_executable(wmw_pc
    src/main.cpp
    src/platform/window.cpp
    src/core/engine.cpp
    src/core/assets.cpp
)

target_include_directories(wmw_pc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(wmw_pc PRIVATE
    ${SDL2_LIBRARIES}
)

# On some distros SDL2::SDL2 target exists; fall back to variables
if(TARGET SDL2::SDL2)
    target_link_libraries(wmw_pc PRIVATE SDL2::SDL2)
endif()

# Nice warnings
if(MSVC)
    target_compile_options(wmw_pc PRIVATE /W4)
else()
    target_compile_options(wmw_pc PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS wmw_pc RUNTIME DESTINATION bin)
