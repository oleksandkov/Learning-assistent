#ifndef CHARACTER_H
#define CHARACTER_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>

class Character : public sf::Sprite
{
private:
    sf::Texture characterTexture;
    sf::Clock animationClock;
    int currentFrame;
    int totalFrames;
    sf::Vector2i frameSize;
    float animationSpeed; 
    sf::Clock clock;
    float speed;

public:
    Character();
    ~Character();
    void update();
    void moveCharacter();
};

#endif