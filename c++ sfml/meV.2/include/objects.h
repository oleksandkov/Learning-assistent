#ifndef OBJECTS_H
#define OBJECTS_H

#include <SFML/Graphics.hpp>

class GameObject : public sf::Sprite
{
public:
    GameObject();
    ~GameObject();
};

struct Floor
{
    sf::RectangleShape shape;
    sf::Texture texture;
    Floor(float width, float height);
};

struct Platform
{
    sf::RectangleShape shape;
    sf::Texture texture;
    Platform(float width, float height, float textureScale = 1.0f);
};

struct Wall
{
    sf::RectangleShape shape;
    sf::Texture texture;
    Wall(float width, float height);
};

class Button : public sf::RectangleShape
{
private:
    sf::Text buttonText;
    sf::Font buttonfont;

public:
    Button();
    Button(const sf::Vector2f &size);
    ~Button();

    void setFont(const sf::Font &font);
    void setButtonText(const std::string &text, unsigned int charSize = 20, sf::Color color = sf::Color::Black);
    sf::Text &getText();
    void isHover(const sf::RenderWindow &window);
    bool isClicked(const sf::Event &event, const sf::RenderWindow &window);
};  


#endif