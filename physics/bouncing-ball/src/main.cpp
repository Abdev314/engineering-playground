#include "physics/Ball.hpp"
#include "core/Simulation.hpp"
#include "rendering/Renderer.hpp"

#include <SFML/System/Clock.hpp>
#include <algorithm>
#include <vector>

namespace {

// ---------------------------------------------------------------- config
struct Config {
    static constexpr unsigned WindowWidth  = 800;
    static constexpr unsigned WindowHeight = 600;
    static constexpr const char* Title     = "Bouncing Ball Simulation";

    static constexpr float FixedStep   = 1.0f / 120.0f; // physics step (s)
    static constexpr float MaxFrameTime = 0.25f;        // avoids spiral of death
};

// ---------------------------------------------------------------- scene
std::vector<Ball> CreateBalls() {
    std::vector<Ball> balls;
    balls.emplace_back(
        Vector2(200.0f, 100.0f),  // position
        Vector2(0.0f, 0.0f),      // velocity
        Vector2(0.0f, 0.0f),      // acceleration
        2.0f,                     // mass
        50.0f                     // radius
    );
    return balls;
}

} // namespace

// ---------------------------------------------------------------- main
int main() {
    const Vector2 gravity(0.0f, 9.81f);

    Simulation simulation(CreateBalls(), gravity);
    Renderer   renderer(Config::WindowWidth, Config::WindowHeight, Config::Title);

    sf::Clock clock;
    float accumulator = 0.0f;

    while (renderer.IsOpen()) {
        renderer.HandleEvents();

        // Fixed-timestep update: physics stays stable and deterministic
        accumulator += std::min(clock.restart().asSeconds(), Config::MaxFrameTime);
        while (accumulator >= Config::FixedStep) {
            simulation.Update(Config::FixedStep);
            accumulator -= Config::FixedStep;
        }

        renderer.Render(simulation);
    }

    return 0;
}