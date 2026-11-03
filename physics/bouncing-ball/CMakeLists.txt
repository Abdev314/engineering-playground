cmake_minimum_required(VERSION 3.20)

project(BouncingBall
    VERSION 1.0
    DESCRIPTION "A C++20 2D physics simulation"
    LANGUAGES CXX
)

# C++ configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find SFML 3
find_package(SFML 3 REQUIRED COMPONENTS
    Graphics
    Window
    System
)

# Executable
add_executable(bouncing-ball
    src/main.cpp
    src/core/Simulation.cpp
    src/physics/Vector2.cpp
    src/physics/Ball.cpp
    src/rendering/Renderer.cpp
)

# Header directories
target_include_directories(bouncing-ball PRIVATE
    include
)

# Link SFML libraries
target_link_libraries(bouncing-ball PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

# Compiler warnings
target_compile_options(bouncing-ball PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)