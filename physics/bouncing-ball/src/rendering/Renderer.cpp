#include "rendering/Renderer.hpp"
#include "core/Simulation.hpp"
#include "physics/Ball.hpp"
#include "physics/Vector2.hpp"
#include "rendering/Background.hpp"

#include <optional>
#include <string>

Renderer::Renderer(unsigned int width, unsigned int height,
                   const std::string& title)
    : window(sf::VideoMode({width, height}), title), background(width, height) {}

bool Renderer::IsOpen() const {
    return window.isOpen();
}

void Renderer::HandleEvents() {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
    }
}

void Renderer::Render(const Simulation& simulation) {

    background.Draw(window);   
    window.draw(groundHatch);
    window.draw(groundLine);

    for (const Ball& ball : simulation.GetBalls()) {
        float radius = ball.getRadius();

        sf::CircleShape circle(radius);

        Vector2 position = ball.getPosition();

        circle.setOrigin({radius, radius});

        circle.setPosition({
            position.getx(),
            position.gety()
        });

        window.draw(circle);
    }

    window.display();
}
