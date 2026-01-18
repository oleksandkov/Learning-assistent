#include "objects.h"
#include <iostream>

Floor::Floor(float width, float height)
{
    shape.setSize(sf::Vector2f(width, height));
    if (!texture.loadFromFile("assets/425.jpg"))
        std::cerr << "Error loading floor texture" << std::endl;
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

Platform::Platform(float width, float height, float textureScale)
{
    shape.setSize(sf::Vector2f(width, height));
    if (!texture.loadFromFile("assets/platform.png"))
        std::cerr << "Error loading platform texture" << std::endl;
    else
    {
        texture.setSmooth(false);
        texture.setRepeated(true);
        shape.setTexture(&texture);
        sf::Vector2u textureSize = texture.getSize();
        int tileWidth = static_cast<int>(textureSize.x * textureScale);
        int tileHeight = static_cast<int>(textureSize.y * textureScale);
        shape.setTextureRect(sf::IntRect(0, 0, tileWidth, tileHeight));
    }
    shape.setPosition(0.f, 0.f);
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(2.f);
}

Wall::Wall(float width, float height)
{
    shape.setSize(sf::Vector2f(width, height));
    if (!texture.loadFromFile("assets/wall.jpg"))
        std::cerr << "Error loading wall texture" << std::endl;
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


Button::Button()
{
    setSize(sf::Vector2f(200.f, 100.f));
    setFillColor(sf::Color(200, 200, 200, 200));
    setOutlineThickness(2.f);
    setOutlineColor(sf::Color::Black);
}

Button::Button(const sf::Vector2f &size) : sf::RectangleShape(size)
{
    setFillColor(sf::Color(200, 200, 200, 200));
    setOutlineThickness(2.f);
    setOutlineColor(sf::Color::Black);
}

Button::~Button() {}

void Button::setFont(const sf::Font &font)
{
    buttonfont = font;
    buttonText.setFont(buttonfont);
}

void Button::setButtonText(const std::string &text, unsigned int charSize, sf::Color color)
{
    buttonText.setString(text);
    buttonText.setCharacterSize(charSize);
    buttonText.setFillColor(color);

    // Center text on button
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    sf::FloatRect buttonBounds = getGlobalBounds();
    buttonText.setPosition(
        buttonBounds.left + (buttonBounds.width - textBounds.width) / 2.f - textBounds.left,
        buttonBounds.top + (buttonBounds.height - textBounds.height) / 2.f - textBounds.top);
}

sf::Text &Button::getText()
{
    return buttonText;
}

void Button::isHover(const sf::RenderWindow &window)
{
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    if (getGlobalBounds().contains(mousePos))
    {
        setFillColor(sf::Color(255, 255, 255, 255));
    }
    else
    {
        setFillColor(sf::Color(200, 200, 200, 200));
    }
}

bool Button::isClicked(const sf::Event &event, const sf::RenderWindow &window)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if (getGlobalBounds().contains(mousePos))
        {
            return true;
        }
    }
    return false;
}