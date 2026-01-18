#include "coins.h"
#include <iostream>
#include <cmath>
#include <random>

sf::Texture Coin::coinTexture;
bool Coin::textureLoaded = false;
sf::Font Coins::interfaceFont;
sf::Texture Coins::coinInterfaceTexture;

bool Coin::loadTexture()
{
    if (!textureLoaded)
    {
        if (coinTexture.loadFromFile("assets/coin (2).png"))
        {
            coinTexture.setSmooth(true);
            textureLoaded = true;
            std::cout << "Coin texture loaded successfully!" << std::endl;
        }
        else
        {
            std::cerr << "Error loading coin texture: assets/coin (2).png" << std::endl;
            return false;
        }
    }
    return true;
}

Coin::Coin(float x, float y, float lifetime)
    : isCollected(false), isExpired(false), lifeTime(0.f), maxLifeTime(lifetime),
      originalPosition(x, y), baseY(y), rotationAngle(0.f), floatOffset(0.f)
{
    if (!loadTexture())
        return;

    setTexture(coinTexture);
    sf::Vector2u textureSize = coinTexture.getSize();
    setOrigin(textureSize.x / 2.f, textureSize.y / 2.f);
    setScale(0.02f, 0.02f);
    setPosition(x, y);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 360.0f);
    rotationAngle = dis(gen);
    floatOffset = rotationAngle / 57.3f;
}

Coin::~Coin() {}

void Coin::update()
{
    if (isCollected || isExpired)
        return;

    lifeTime += 0.016f;

    if (lifeTime >= maxLifeTime)
    {
        isExpired = true;
        return;
    }

    floatOffset += 0.05f;
    float floatY = baseY + std::sin(floatOffset) * 5.0f;
    setPosition(originalPosition.x, floatY);
    setScale(0.02f, 0.02f);
    setRotation(0.0f);

    float fadeRatio = 1.0f - (lifeTime / maxLifeTime);
    if (fadeRatio < 0.3f)
    {
        sf::Uint8 alpha = static_cast<sf::Uint8>(255 * (fadeRatio / 0.3f));
        setColor(sf::Color(255, 255, 255, alpha));
    }
}

void Coin::draw(sf::RenderWindow &window)
{
    if (!isCollected && !isExpired)
        window.draw(*this);
}

bool Coin::getIsCollected() const { return isCollected; }
void Coin::setCollected(bool collected) { isCollected = collected; }
bool Coin::getIsExpired() const { return isExpired; }
sf::FloatRect Coin::getGlobalBounds() const { return sf::Sprite::getGlobalBounds(); }
void Coin::setMaxLifeTime(float lifetime) { maxLifeTime = lifetime; }
float Coin::getMaxLifeTime() const { return maxLifeTime; }

Coins::Coins(float spawnInterval, float coinLifetime)
    : spawnTimer(0.0f), spawnInterval(spawnInterval), defaultCoinLifetime(coinLifetime),
      currentSpawnIndex(0), collectedCoinsCounter(0)
{
    Coin::loadTexture();
}

Coins::~Coins() { clear(); }

void Coins::addCoinPosition(float x, float y)
{
    coinSpawnPositions.push_back(sf::Vector2f(x, y));
}

void Coins::update()
{
    spawnTimer += 0.016f;

    if (spawnTimer >= spawnInterval && !coinSpawnPositions.empty())
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dis(0, coinSpawnPositions.size() - 1);

        sf::Vector2f spawnPos = coinSpawnPositions[dis(gen)];
        coins.push_back(new Coin(spawnPos.x, spawnPos.y, defaultCoinLifetime));
        spawnTimer = 0.0f;
    }

    for (auto coin : coins)
        coin->update();

    cleanupExpiredCoins();
}

void Coins::draw(sf::RenderWindow &window)
{
    for (auto coin : coins)
        coin->draw(window);
}

int Coins::checkCollision(sf::FloatRect characterBounds)
{
    int collectedCount = 0;
    for (auto coin : coins)
    {
        if (coin->getIsCollected() || coin->getIsExpired())
            continue;

        if (characterBounds.intersects(coin->getGlobalBounds()))
        {
            coin->setCollected(true);
            collectedCoinsCounter++;
            collectedCount++;
        }
    }
    return collectedCount;
}

void Coins::cleanupExpiredCoins()
{
    for (auto it = coins.begin(); it != coins.end();)
    {
        if ((*it)->getIsExpired() || (*it)->getIsCollected())
        {
            delete *it;
            it = coins.erase(it);
        }
        else
            ++it;
    }
}

void Coins::clear()
{
    for (auto coin : coins)
        delete coin;
    coins.clear();
}

size_t Coins::getCoinsCount() const { return coins.size(); }
size_t Coins::getCollectedCoinsCount() const { return collectedCoinsCounter; }

void Coins::getCoinInterface(sf::RenderWindow &window)
{
    static bool interfaceLoaded = false;
    if (!interfaceLoaded)
    {
        if (!interfaceFont.loadFromFile("assets/fonts/GothicA1-Regular.ttf"))
            std::cerr << "Error loading interface font for coins" << std::endl;
        if (!coinInterfaceTexture.loadFromFile("assets/coin (2).png"))
            std::cerr << "Error loading coin interface texture" << std::endl;
        interfaceLoaded = true;
    }

    sf::View currentView = window.getView();
    sf::Vector2f viewCenter = currentView.getCenter();
    sf::Vector2f viewSize = currentView.getSize();

    float startX = viewCenter.x - viewSize.x / 2.f + 10.f;
    float startY = viewCenter.y - viewSize.y / 2.f + 50.f;

    sf::Sprite coinIcon;
    coinIcon.setTexture(coinInterfaceTexture);
    sf::Vector2u textureSize = coinInterfaceTexture.getSize();
    float iconSize = 25.f;
    coinIcon.setScale(iconSize / textureSize.x, iconSize / textureSize.y);
    coinIcon.setPosition(startX, startY);

    sf::Text coinText;
    coinText.setFont(interfaceFont);
    coinText.setString("x " + std::to_string(getCollectedCoinsCount()));
    coinText.setCharacterSize(20);
    coinText.setFillColor(sf::Color::White);
    coinText.setPosition(startX + iconSize + 5.f, startY + 2.f);

    window.draw(coinIcon);
    window.draw(coinText);
}

void Coins::setSpawnInterval(float interval) { spawnInterval = interval; }
float Coins::getSpawnInterval() const { return spawnInterval; }
void Coins::setDefaultCoinLifetime(float lifetime) { defaultCoinLifetime = lifetime; }
float Coins::getDefaultCoinLifetime() const { return defaultCoinLifetime; }
