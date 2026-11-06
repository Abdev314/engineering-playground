#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SFML/Graphics.hpp>
#include <string>

#include "core/Simulation.hpp"

class Renderer {
private:
    sf::RenderWindow window;

public:
    Renderer(unsigned int width, unsigned int height,
             const std::string& title);

    void HandleEvents();
    void Render(const Simulation& simulation);
    bool IsOpen() const;
};

#endif
