#pragma once
#include <SFML/Graphics.hpp>

struct BackgroundStyle {
    sf::Color paper      = sf::Color(18, 38, 66);        
    sf::Color minorLine  = sf::Color(255, 255, 255, 28);
    sf::Color majorLine  = sf::Color(255, 255, 255, 70);
    sf::Color crosshair  = sf::Color(255, 255, 255, 140);
    float cell           = 30.0f;  
    int   majorEvery     = 5;      
    float crossSize      = 6.0f;   
};

class Background {
    public:
        using Style = BackgroundStyle;

        Background(unsigned width, unsigned height, Style style = {});
        void Draw(sf::RenderTarget& target) const;

    private:
        void Build(unsigned width, unsigned height);

        Style style;
        sf::VertexArray minor{sf::PrimitiveType::Lines};
        sf::VertexArray major{sf::PrimitiveType::Lines};
        sf::VertexArray cross{sf::PrimitiveType::Lines};
};