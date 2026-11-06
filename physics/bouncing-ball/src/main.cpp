#include "physics/Ball.hpp"
#include "core/Simulation.hpp"
#include "rendering/Renderer.hpp"

#include <SFML/System/Clock.hpp>
#include <vector>
#include <iostream>

int main() {

    std::cout << "Starting program\n";

    // Create balls
    std::vector<Ball> balls;

    balls.emplace_back(
        Vector2(200.0f, 100.0f),
        Vector2(0.0f, 0.0f),
        Vector2(0.0f, 0.0f),
        2.0f,
        50.0f
    );


    std::cout << "Balls created\n";

    // Create simulation
    Vector2 gravity(0.0f, 9.81f);
    Simulation simulation(balls, gravity);

    std::cout << "Simulation created\n";

    // Create renderer
    Renderer renderer(800, 600, "Bouncing Ball Simulation");

    std::cout << "Renderer created\n";
    std::cout << "Window open: " << renderer.IsOpen() << '\n';

    // Create clock
    sf::Clock clock;

    // Main application loop
    while (renderer.IsOpen()) {

        renderer.HandleEvents();

        float deltaTime = clock.restart().asSeconds();

        simulation.Update(deltaTime);

        renderer.Render(simulation);
    }

    std::cout << "Window closed\n";

    return 0;
}
