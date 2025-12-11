#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>

class Enemy : public sf::Sprite
{
private:
    sf::Texture idleTexture, walkTexture, deadTexture, attackTexture;
    sf::Clock animationClock, movementClock, physicsClock, attackCooldownClock;
    sf::Vector2i frameSize;
    sf::Vector2f velocity;
    std::vector<sf::FloatRect> collisionList;

    int currentFrame, idleFrames, walkFrames, deadFrames, attackFrames;
    float animationSpeed, speed, health, detectionRange, attackDamage, attackCooldown;
    bool onGround, isIdle, isWalking, isDead, shouldRemove;
    bool wasHitThisFrame, isAttacking, canAttack;

    static sf::Clock spawnClock;
    static std::vector<sf::Vector2f> spawnPositions;
    static size_t currentEnemyCount, totalKilledEnemies, maxEnemyCount;
    static float spawnPeriod;
    static sf::Font killInterfaceFont;
    static sf::Texture killInterfaceTexture;
    static bool killInterfaceLoaded;

    static bool loadKillInterfaceResources();

public:
    sf::RectangleShape hitbox, attackHitbox;
    sf::FloatRect platformBounds;
    bool isStaying;

    Enemy();
    ~Enemy();

    void update();
    void enemyAI(sf::Vector2f playerPosition, float playerHealth);
    void enemyLogic();
    bool takeDamage(float damageAmount);
    void addToCollisionList(sf::FloatRect rect);
    void initializeHitbox();
    void resetHitFlag();

    bool getIsDead() const;
    bool shouldBeRemoved() const;
    bool getIsAttacking() const;
    float getHealth() const;
    float getAttackDamage() const;
    void setHealth(float newHealth);

    static size_t getTotalKilledEnemies();
    static size_t getCurrentEnemyCount();
    static void resetKillCounter();
    static void getKillInterface(sf::RenderWindow &window);
    static void initializeSpawner(const std::vector<sf::Vector2f> &positions, float spawnPeriod, size_t maxEnemies);
    static bool shouldSpawnNewEnemy();
    static sf::Vector2f getRandomSpawnPosition();
    static void updateSpawner();
    static void resetSpawner();
    static void decrementEnemyCount();
};

#endif
