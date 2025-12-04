#ifndef CHARACTER_H
#define CHARACTER_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>

class Character : public sf::Sprite
{
private:
    sf::Texture idleTexture, walkTexture, jumpTexture, attackTexture, hurtTexture, deadTexture, heartTexture;
    sf::Clock animationClock, clock, physicsClock, damageCooldownClock;
    sf::Vector2i frameSize;
    sf::Vector2f velocity, oldpos;
    std::vector<sf::FloatRect> collisionList;
    std::vector<sf::Sprite> hearts;

    int currentFrame, totalFrames, attackFrames, hurtFrames, deadFrames;
    float animationSpeed, jumpAnimationSpeed, speed, jumpVelocity, health, damageCooldown, damage;
    bool isWalking, isJumping, onGround, isAttacking, wasSpacePressed, isHurt, isDead, canTakeDamage;

public:
    sf::RectangleShape hitbox, attackHitbox;

    Character();
    ~Character();

    void update();
    void moveCharacter();
    void characterLogic();
    void takeDamage(float damage);
    void addToCollisionList(sf::FloatRect rect);
    void initializeHitbox();
    void updateAttackHitbox();
    void getHealthInterface(sf::RenderWindow &window);

    float getHealth() const;
    float getDamage() const;
    bool getIsAttacking() const;
    bool getIsDead() const;
};

#endif