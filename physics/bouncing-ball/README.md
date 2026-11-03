# Bouncing Ball

A 2D physics simulation built from scratch using C++20.

This project explores object-oriented programming, classical mechanics, numerical integration, and real-time computer graphics through practical experimentation.

The objective is to build a physical simulation, understand the mathematics behind it, and visualize how objects behave under different physical conditions.

## Goals

Build a ball simulation that:

- Falls under gravity.
- Accelerates naturally.
- Collides with a flat surface.
- Bounces with configurable energy loss.
- Responds to changes in physical parameters.
- Displays its movement in real time.

All physics calculations will be implemented manually without relying on an existing physics engine.

## Technology Stack

| Technology | Purpose |
|---|---|
| C++20 | Core programming language |
| SFML 3 | Graphics and rendering |
| CMake | Build system |
| GCC/G++ | Compiler |

## Project Architecture

```text
bouncing-ball/
├── CMakeLists.txt
├── README.md
├── .gitignore
│
├── include/
│   ├── core/
│   │   └── Simulation.hpp
│   ├── physics/
│   │   ├── Vector2.hpp
│   │   └── Ball.hpp
│   └── rendering/
│       └── Renderer.hpp
│
└── src/
    ├── main.cpp
    ├── core/
    │   └── Simulation.cpp
    ├── physics/
    │   ├── Vector2.cpp
    │   └── Ball.cpp
    └── rendering/
        └── Renderer.cpp
```

## Physics Concepts

The project explores:

- Newton's laws of motion.
- Gravitational force.
- Position, velocity and acceleration.
- Two-dimensional vector mathematics.
- Numerical integration.
- Collision detection.
- Coefficient of restitution.
- Conservation and loss of mechanical energy.

## Development Roadmap

### Phase 1 — Project Foundation
- [ ] Configure C++20 and CMake.
- [ ] Set up SFML 3.
- [ ] Create the application window.
- [ ] Establish the simulation loop.

### Phase 2 — Mathematics
- [ ] Implement a 2D vector type.
- [ ] Implement vector addition and subtraction.
- [ ] Implement scalar multiplication and division.
- [ ] Implement magnitude and normalization.
- [ ] Implement dot product.

### Phase 3 — Physics
- [ ] Create the Ball class.
- [ ] Implement force application.
- [ ] Calculate acceleration.
- [ ] Implement numerical integration.
- [ ] Apply gravity.

### Phase 4 — Collision Mechanics
- [ ] Detect ground collision.
- [ ] Implement bouncing.
- [ ] Introduce energy loss.
- [ ] Experiment with different restitution values.

### Phase 5 — Interaction and Visualization
- [ ] Add pause and resume.
- [ ] Add reset functionality.
- [ ] Allow physical parameters to be modified.
- [ ] Display velocity and acceleration.
- [ ] Add real-time physics visualization.

## Design Principles

- Implement the underlying mathematics yourself.
- Keep physics independent from rendering.
- Use object-oriented design where appropriate.
- Prefer simple and understandable implementations.
- Experiment with physical parameters.
- Document discoveries, problems and solutions.

## Build Requirements

- C++20-compatible compiler.
- CMake 3.20 or newer.
- SFML 3.

## Building

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Run:

```bash
./build/bouncing-ball
```
