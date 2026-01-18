#ifndef COINS_H
#define COINS_H

#include <SFML/Graphics.hpp>
#include <vector>

class Coin : public sf::Sprite
{
private:
    static sf::Texture coinTexture;
    static bool textureLoaded;

    sf::Vector2f originalPosition;
    float rotationAngle, floatOffset, baseY, lifeTime, maxLifeTime;
    bool isCollected, isExpired;

public:
    Coin(float x, float y, float lifetime = 10.0f);
    ~Coin();

    static bool loadTexture();
    void update();
    void draw(sf::RenderWindow &window);

    bool getIsCollected() const;
    bool getIsExpired() const;
    void setCollected(bool collected);
    void setMaxLifeTime(float lifetime);
    float getMaxLifeTime() const;
    sf::FloatRect getGlobalBounds() const;
};

class Coins
{
private:
    std::vector<Coin *> coins;
    std::vector<sf::Vector2f> coinSpawnPositions;
    static sf::Font interfaceFont;
    static sf::Texture coinInterfaceTexture;

    float spawnTimer, spawnInterval, defaultCoinLifetime;
    size_t currentSpawnIndex, collectedCoinsCounter;

public:
    Coins(float spawnInterval = 10.0f, float coinLifetime = 10.0f);
    ~Coins();

    void addCoinPosition(float x, float y);
    void update();
    void draw(sf::RenderWindow &window);
    int checkCollision(sf::FloatRect characterBounds);
    void clear();
    void cleanupExpiredCoins();
    void getCoinInterface(sf::RenderWindow &window);

    size_t getCoinsCount() const;
    size_t getCollectedCoinsCount() const;
    void setSpawnInterval(float interval);
    float getSpawnInterval() const;
    void setDefaultCoinLifetime(float lifetime);
    float getDefaultCoinLifetime() const;
};

#endif
