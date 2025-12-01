#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>

class Enemy : public sf::Sprite
{
private:
    sf::Texture idleTexture;
    sf::Texture walkTexture;
    sf::Texture jumpTexture;
    sf::Texture deadTexture;
    sf::Clock animationClock;
    int currentFrame;
    int idleFrames;
    int walkFrames;
    int jumpFrames;
    int deadFrames;
    sf::Vector2i frameSize;
    float animationSpeed;
    sf::Clock movementClock;
    sf::Clock physicsClock;
    float speed;
    std::vector<sf::FloatRect> collisionList;
    sf::Vector2f velocity;
    bool onGround;
    float jumpVelocity;
    bool isIdle;
    bool isWalking;
    bool isJumping;
    bool isDead;
    bool shouldRemove;

    // AI behavior variables
    float detectionRange;

public:
    sf::RectangleShape hitbox;

    Enemy();
    ~Enemy();
    void update();
    void enemyAI(sf::Vector2f playerPosition);
    void addToCollisionList(sf::FloatRect rect);
    void initializeHitbox();
    void enemyLogic();
    void takeDamage();
    bool getIsDead() const;
    bool shouldBeRemoved() const;
};

#endif
