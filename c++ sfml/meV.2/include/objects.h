#ifndef OBJECTS_H
#define OBJECTS_H
#include <SFML/Graphics.hpp>
#include "coins.h"

class GameObject : public sf::Sprite
{

private:
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

#endif // OBJECTS_H