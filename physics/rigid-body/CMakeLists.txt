cmake_minimum_required(VERSION 3.20)

project(rigid_body
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    src/main.cpp

    src/Physics/RigidBody.cpp
    src/Physics/Force.cpp
    src/Physics/Torque.cpp

    src/Math/Vector3.cpp
    src/Math/Matrix3.cpp
    src/Math/Quaternion.cpp
)

add_executable(rigid_body ${SOURCES})

target_include_directories(rigid_body
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)