#include "coins.h"
#include <iostream>
#include <cmath>
#include <random>

// Static member definitions
sf::Texture Coin::coinTexture;
bool Coin::textureLoaded = false;

// Coins interface static members
sf::Font Coins::interfaceFont;
sf::Texture Coins::coinInterfaceTexture;

// Coin class implementation
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
    : isCollected(false), rotationAngle(0.f), floatOffset(0.f), baseY(y), originalPosition(x, y),
      lifeTime(0.f), maxLifeTime(lifetime), isExpired(false)
{
    // Load texture if not already loaded
    if (!loadTexture())
    {
        return;
    }

    // Set texture and properties
    setTexture(coinTexture);

    // Set origin to center for proper rotation
    sf::Vector2u textureSize = coinTexture.getSize();
    setOrigin(textureSize.x / 2.f, textureSize.y / 2.f);

    // Scale coin to much smaller size
    setScale(0.02f, 0.02f);

    // Set initial position
    setPosition(x, y);

    // Random initial rotation and float offset for variety
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 360.0f);
    rotationAngle = dis(gen);
    floatOffset = dis(gen) / 57.3f; // Convert to radians
}

Coin::~Coin()
{
}

void Coin::update()
{
    if (!isCollected && !isExpired)
    {
        // Update lifetime
        lifeTime += 0.016f; // Assuming ~60 FPS (1/60 = 0.016)

        // Check if coin has expired
        if (lifeTime >= maxLifeTime)
        {
            isExpired = true;
            return;
        }

        // Simple floating animation - up and down movement only
        floatOffset += 0.05f;
        float floatY = baseY + std::sin(floatOffset) * 5.0f;
        setPosition(originalPosition.x, floatY);

        // Keep consistent small size
        setScale(0.02f, 0.02f);

        // No rotation - keep sprite upright
        setRotation(0.0f);

        // Fade out effect as coin approaches expiration
        float fadeRatio = 1.0f - (lifeTime / maxLifeTime);
        if (fadeRatio < 0.3f)
        {
            sf::Uint8 alpha = static_cast<sf::Uint8>(255 * (fadeRatio / 0.3f));
            setColor(sf::Color(255, 255, 255, alpha));
        }
    }
}
void Coin::draw(sf::RenderWindow &window)
{
    if (!isCollected && !isExpired)
    {
        window.draw(*this);
    }
}

bool Coin::getIsCollected() const
{
    return isCollected;
}

void Coin::setCollected(bool collected)
{
    isCollected = collected;
}

bool Coin::getIsExpired() const
{
    return isExpired;
}

sf::FloatRect Coin::getGlobalBounds() const
{
    return sf::Sprite::getGlobalBounds();
}

// Coins container class implementation
Coins::Coins(float spawnInterval, float coinLifetime) : spawnTimer(0.0f), spawnInterval(spawnInterval), defaultCoinLifetime(coinLifetime), currentSpawnIndex(0), collectedCoinsCounter(0)
{
    // Pre-load the texture when creating coins manager
    Coin::loadTexture();
}

Coins::~Coins()
{
    clear();
}

void Coins::addCoinPosition(float x, float y)
{
    coinSpawnPositions.push_back(sf::Vector2f(x, y));
}

void Coins::update()
{
    // Update spawn timer
    spawnTimer += 0.016f; // Assuming ~60 FPS

    // Spawn new coin if enough time has passed and there are positions available
    if (spawnTimer >= spawnInterval && !coinSpawnPositions.empty())
    {
        // Pick a random position from available spawn positions
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dis(0, coinSpawnPositions.size() - 1);

        size_t randomIndex = dis(gen);
        sf::Vector2f spawnPos = coinSpawnPositions[randomIndex];
        coins.push_back(new Coin(spawnPos.x, spawnPos.y, defaultCoinLifetime));
        spawnTimer = 0.0f;

        std::cout << "New coin spawned randomly at (" << spawnPos.x << ", " << spawnPos.y << ")" << std::endl;
    }

    // Update all existing coins
    for (auto coin : coins)
    {
        coin->update();
    }

    // Clean up expired coins
    cleanupExpiredCoins();
}

void Coins::draw(sf::RenderWindow &window)
{
    for (auto coin : coins)
    {
        coin->draw(window);
    }
}

void Coins::checkCollision(sf::FloatRect characterBounds)
{
    for (auto coin : coins)
    {
        if (!coin->getIsCollected() && !coin->getIsExpired() &&
            characterBounds.intersects(coin->getGlobalBounds()))
        {
            coin->setCollected(true);
            collectedCoinsCounter++;
            std::cout << "Coin collected! Total collected: " << collectedCoinsCounter << std::endl;
        }
    }
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
        {
            ++it;
        }
    }
}

void Coins::clear()
{
    for (auto coin : coins)
    {
        delete coin;
    }
    coins.clear();
}

size_t Coins::getCoinsCount() const
{
    return coins.size();
}

size_t Coins::getCollectedCoinsCount() const
{
    return collectedCoinsCounter;
}

void Coins::getCoinInterface(sf::RenderWindow &window)
{
    // Load font and coin texture for interface (only once)
    static bool interfaceLoaded = false;
    if (!interfaceLoaded)
    {
        if (!interfaceFont.loadFromFile("assets/fonts/GothicA1-Regular.ttf"))
        {
            std::cerr << "Error loading interface font for coins" << std::endl;
        }
        if (!coinInterfaceTexture.loadFromFile("assets/coin (2).png"))
        {
            std::cerr << "Error loading coin interface texture" << std::endl;
        }
        interfaceLoaded = true;
    }

    sf::View currentView = window.getView();
    sf::Vector2f viewCenter = currentView.getCenter();
    sf::Vector2f viewSize = currentView.getSize();

    // Position under health bar (health bar is at top-left, coins go below)
    float startX = viewCenter.x - viewSize.x / 2.f + 10.f;
    float startY = viewCenter.y - viewSize.y / 2.f + 50.f; // 50px below health bar

    // Create coin sprite for interface
    sf::Sprite coinIcon;
    coinIcon.setTexture(coinInterfaceTexture);

    // Scale coin to appropriate size
    sf::Vector2u textureSize = coinInterfaceTexture.getSize();
    float iconSize = 25.f;
    coinIcon.setScale(iconSize / textureSize.x, iconSize / textureSize.y);
    coinIcon.setPosition(startX, startY);

    // Create text for coin count
    sf::Text coinText;
    coinText.setFont(interfaceFont);
    coinText.setString("x " + std::to_string(getCollectedCoinsCount()));
    coinText.setCharacterSize(20);
    coinText.setFillColor(sf::Color::White);
    coinText.setPosition(startX + iconSize + 5.f, startY + 2.f);

    // Draw coin interface
    window.draw(coinIcon);
    window.draw(coinText);
}

void Coins::setSpawnInterval(float interval)
{
    spawnInterval = interval;
}

float Coins::getSpawnInterval() const
{
    return spawnInterval;
}

void Coins::setDefaultCoinLifetime(float lifetime)
{
    defaultCoinLifetime = lifetime;
}

float Coins::getDefaultCoinLifetime() const
{
    return defaultCoinLifetime;
}
