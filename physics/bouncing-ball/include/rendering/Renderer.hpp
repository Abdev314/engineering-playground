#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include "core/Simulation.hpp"
#include "rendering/Background.hpp"

class Ball;

class Renderer {
    private:
        void BuildGround(float width);
        void DrawBall(const Ball& ball);

        sf::RenderWindow  window;
        Background background;
        float groundY;
        sf::VertexArray groundHatch{sf::PrimitiveType::Lines};
        sf::RectangleShape groundLine;

    public:
        Renderer(unsigned int width, unsigned int height, const std::string& title);

        void HandleEvents();
        void Render(const Simulation& simulation);
        bool IsOpen() const;
};

#endif