#include "objects.h"
#include <iostream>
#include <algorithm>

// test
Floor::Floor(float width, float height)
{
    shape.setSize(sf::Vector2f(width, height));
    if (!texture.loadFromFile("assets/425.jpg"))
    {
        std::cerr << "Error loading floor texture" << std::endl;
    }
    else
    {
        texture.setRepeated(true);
        shape.setTexture(&texture);
        shape.setTextureRect(sf::IntRect(0, 0, static_cast<int>(width), static_cast<int>(height)));
    }
    shape.setPosition(0.f, 0.f);
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(2.f);
}

Platform::Platform(float width, float height)
{
    shape.setSize(sf::Vector2f(width, height));
    if (!texture.loadFromFile("assets/platform.png"))
    {
        std::cerr << "Error loading platform texture" << std::endl;
    }
    else
    {
        texture.setRepeated(true);
        shape.setTexture(&texture);
        shape.setTextureRect(sf::IntRect(0, 0, static_cast<int>(width), static_cast<int>(height)));
    }
    shape.setPosition(0.f, 0.f);
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(2.f);
}

Wall::Wall(float width, float height)
{
    shape.setSize(sf::Vector2f(width, height));
    if (!texture.loadFromFile("assets/wall.jpg"))
    {
        std::cerr << "Error loading wall texture" << std::endl;
    }
    else
    {
        texture.setRepeated(true);
        shape.setTexture(&texture);
        shape.setTextureRect(sf::IntRect(0, 0, static_cast<int>(width), static_cast<int>(height)));
    }
    shape.setPosition(0.f, 0.f);
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(2.f);
}
