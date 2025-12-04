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
    sf::Texture deadTexture;
    sf::Texture attackTexture;
    sf::Clock animationClock;
    int currentFrame;
    int idleFrames;
    int walkFrames;
    int deadFrames;
    int attackFrames;
    sf::Vector2i frameSize;
    float animationSpeed;
    sf::Clock movementClock;
    sf::Clock physicsClock;
    float speed;
    float health;
    std::vector<sf::FloatRect> collisionList;
    sf::Vector2f velocity;
    bool onGround;
    bool isIdle;
    bool isWalking;
    bool isDead;
    bool shouldRemove;
    bool wasHitThisFrame; // Track if enemy was hit in current attack frame

    // AI behavior variables
    float detectionRange;

public:
    sf::RectangleShape hitbox;
    bool isStaying;
    sf::FloatRect platformBounds; // Bounds of the platform enemy stays on

    Enemy();
    ~Enemy();
    void update();
    void enemyAI(sf::Vector2f playerPosition, float playerHealth);
    void addToCollisionList(sf::FloatRect rect);
    void initializeHitbox();
    void enemyLogic();
    void takeDamage(float damageAmount);
    bool getIsDead() const;
    bool shouldBeRemoved() const;
    float getHealth() const;
    void setHealth(float newHealth);
    void resetHitFlag();
};

#endif
