#ifndef OBJECTS_H
#define OBJECTS_H
#include <SFML/Graphics.hpp>

class GameObject : public sf::Sprite
{

    private:
    
public:
    GameObject();
    ~GameObject();

};

struct Floor {
    sf::RectangleShape shape;
    sf::Texture texture;
    Floor(float width, float height);
};

#endif // OBJECTS_H