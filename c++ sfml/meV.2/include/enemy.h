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
    bool wasHitThisFrame;
    bool isAttacking;
    float attackDamage;
    sf::Clock attackCooldownClock;
    float attackCooldown;
    bool canAttack;

    // AI behavior variables
    float detectionRange;

    // Enemy spawning
    static sf::Clock spawnClock;
    static std::vector<sf::Vector2f> spawnPositions;
    static size_t currentEnemyCount;

    // Static kill tracking
    static size_t totalKilledEnemies;
    static sf::Font killInterfaceFont;
    static sf::Texture killInterfaceTexture;
    static bool killInterfaceLoaded;

public:
    sf::RectangleShape hitbox;
    bool isStaying;
    sf::FloatRect platformBounds;
    sf::RectangleShape attackHitbox;

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
    float getAttackDamage() const;
    bool getIsAttacking() const;

    // Static kill tracking methods
    static size_t getTotalKilledEnemies();
    static void resetKillCounter();
    static void getKillInterface(sf::RenderWindow &window);

    // Static spawning management
    static void initializeSpawner(const std::vector<sf::Vector2f> &positions, float spawnPeriod, size_t maxEnemies);
    static bool shouldSpawnNewEnemy();
    static sf::Vector2f getRandomSpawnPosition();
    static void updateSpawner();
    static void resetSpawner();
    static void decrementEnemyCount();
    static size_t getCurrentEnemyCount();

private:
    static float spawnPeriod;
    static size_t maxEnemyCount;
    static bool loadKillInterfaceResources();
};

#endif
