#include "rendering/Background.hpp"

Background::Background(unsigned width, unsigned height, Style style)
    : style(style) {
    Build(width, height);
}

void Background::Build(unsigned width, unsigned height) {
    const float w = static_cast<float>(width);
    const float h = static_cast<float>(height);

    auto addLine = [](sf::VertexArray& va, sf::Vector2f a, sf::Vector2f b, sf::Color c) {
        va.append({a, c});
        va.append({b, c});
    };

    // Grid lines
    int i = 0;
    for (float x = 0.0f; x <= w; x += style.cell, ++i) {
        const bool isMajor = (i % style.majorEvery == 0);
        addLine(isMajor ? major : minor, {x, 0.0f}, {x, h},
                isMajor ? style.majorLine : style.minorLine);
    }
    i = 0;
    for (float y = 0.0f; y <= h; y += style.cell, ++i) {
        const bool isMajor = (i % style.majorEvery == 0);
        addLine(isMajor ? major : minor, {0.0f, y}, {w, y},
                isMajor ? style.majorLine : style.minorLine);
    }

    // Crosshairs on major intersections
    const float step = style.cell * style.majorEvery;
    for (float x = step; x < w; x += step) {
        for (float y = step; y < h; y += step) {
            addLine(cross, {x - style.crossSize, y}, {x + style.crossSize, y}, style.crosshair);
            addLine(cross, {x, y - style.crossSize}, {x, y + style.crossSize}, style.crosshair);
        }
    }
}

void Background::Draw(sf::RenderTarget& target) const {
    target.clear(style.paper);
    target.draw(minor);
    target.draw(major);
    target.draw(cross);
}