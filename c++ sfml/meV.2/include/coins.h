#ifndef COINS_H
#define COINS_H

#include <SFML/Graphics.hpp>
#include <vector>

class Coin : public sf::Sprite
{
private:
    static sf::Texture coinTexture;
    static bool textureLoaded;
    bool isCollected;
    float rotationAngle;
    float floatOffset;
    float baseY;
    sf::Vector2f originalPosition;
    float lifeTime;
    float maxLifeTime;
    bool isExpired;

public:
    Coin(float x, float y, float lifetime = 10.0f);
    ~Coin();

    static bool loadTexture();
    void update();
    void draw(sf::RenderWindow &window);

    bool getIsCollected() const;
    void setCollected(bool collected);
    bool getIsExpired() const;
    sf::FloatRect getGlobalBounds() const;

    // Lifetime management
    void setMaxLifeTime(float lifetime);
    float getMaxLifeTime() const;
};

class Coins
{
private:
    std::vector<Coin *> coins;
    std::vector<sf::Vector2f> coinSpawnPositions;
    float spawnTimer;
    float spawnInterval;
    float defaultCoinLifetime;
    size_t currentSpawnIndex;
    size_t collectedCoinsCounter;

    // Interface elements
    static sf::Font interfaceFont;
    static sf::Texture coinInterfaceTexture;

public:
    Coins(float spawnInterval = 10.0f, float coinLifetime = 10.0f);
    ~Coins();

    void addCoinPosition(float x, float y);
    void update();
    void draw(sf::RenderWindow &window);
    void checkCollision(sf::FloatRect characterBounds);
    void clear();
    void cleanupExpiredCoins();

    size_t getCoinsCount() const;
    size_t getCollectedCoinsCount() const;

    // Interface
    void getCoinInterface(sf::RenderWindow &window);

    // Configuration management
    void setSpawnInterval(float interval);
    float getSpawnInterval() const;
    void setDefaultCoinLifetime(float lifetime);
    float getDefaultCoinLifetime() const;
};

#endif // COINS_H
