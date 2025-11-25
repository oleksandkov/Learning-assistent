#ifndef CHARACTER_H
#define CHARACTER_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>

class Character : public sf::Sprite
{
private:
    sf::Texture idleTexture;
    sf::Texture walkTexture;
    sf::Texture jumpTexture;
    sf::Clock animationClock;
    int currentFrame;
    int totalFrames;
    sf::Vector2i frameSize;
    float animationSpeed; 
    sf::Clock clock;
    float speed;
    std::vector<sf::FloatRect> collisionList;
    sf::Vector2f oldpos;
    bool isWalking;
    bool isJumping;
    sf::Vector2f velocity;
    bool onGround;
    float jumpVelocity;
    float jumpAnimationSpeed;
    
    
    public:
    sf::RectangleShape hitbox;
    Character();
    ~Character();
    void update();
    void moveCharacter();
    void addToCollisionList(sf::FloatRect rect);
    void initializeHitbox();
    void characterLogic();
};

#endif