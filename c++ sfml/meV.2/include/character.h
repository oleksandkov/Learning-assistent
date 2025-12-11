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
    sf::Texture attackTexture;
    sf::Texture hurtTexture;
    sf::Texture deadTexture;
    sf::Clock animationClock;
    int currentFrame;
    int totalFrames;
    int attackFrames;
    int hurtFrames;
    int deadFrames;
    sf::Vector2i frameSize;
    float animationSpeed; 
    sf::Clock clock;
    sf::Clock physicsClock;
    float speed;
    std::vector<sf::FloatRect> collisionList;
    sf::Vector2f oldpos;
    bool isWalking;
    bool isJumping;
    sf::Vector2f velocity;
    bool onGround;
    float jumpVelocity;
    float jumpAnimationSpeed;
    bool isAttacking;
    bool wasSpacePressed;
    bool isHurt;
    bool isDead;
    float health;
    float damage;
    sf::Clock damageCooldownClock;
    float damageCooldown;
    bool canTakeDamage;
    sf::Texture heartTexture;
    std::vector<sf::Sprite> hearts;
    
    public:
    sf::RectangleShape hitbox;
    sf::RectangleShape attackHitbox;
    Character();
    ~Character();
    void update();
    void moveCharacter();
    void addToCollisionList(sf::FloatRect rect);
    void initializeHitbox();
    void updateAttackHitbox();
    void characterLogic();
    void takeDamage(float damage);
    float getHealth() const;
    float getDamage() const;
    bool getIsAttacking() const;
    bool getIsDead() const;
    void getHealthInterface(sf::RenderWindow &window);
};

#endif