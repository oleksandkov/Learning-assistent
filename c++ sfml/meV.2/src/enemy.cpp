#include <enemy.h>

size_t Enemy::totalKilledEnemies = 0;
sf::Font Enemy::killInterfaceFont;
sf::Texture Enemy::killInterfaceTexture;
bool Enemy::killInterfaceLoaded = false;
sf::Clock Enemy::spawnClock;
std::vector<sf::Vector2f> Enemy::spawnPositions;
size_t Enemy::currentEnemyCount = 0;
float Enemy::spawnPeriod = 2.0f;
size_t Enemy::maxEnemyCount = 5;

Enemy::Enemy()
{
    idleFrames = 8;
    walkFrames = 8;
    deadFrames = 3;
    attackFrames = 4;
    animationSpeed = 0.15f;
    currentFrame = 0;
    speed = 120.f;
    velocity = sf::Vector2f(0.f, 0.f);
    onGround = false;
    isIdle = true;
    isWalking = false;
    isDead = false;
    shouldRemove = false;
    wasHitThisFrame = false;
    isAttacking = false;
    detectionRange = 400.f;
    isStaying = false;
    platformBounds = sf::FloatRect(0.f, 0.f, 0.f, 0.f);
    health = 20.f;
    attackDamage = 10.f;
    attackCooldown = 2.5f;
    canAttack = true;

    idleTexture.loadFromFile("assets/enemy/Idle.png");
    walkTexture.loadFromFile("assets/enemy/Walk.png");
    deadTexture.loadFromFile("assets/enemy/Dead.png");
    attackTexture.loadFromFile("assets/enemy/Attack_1.png");

    frameSize.x = idleTexture.getSize().x / idleFrames;
    frameSize.y = idleTexture.getSize().y;
    setTexture(idleTexture);
    setTextureRect(sf::IntRect(0, 0, frameSize.x, frameSize.y));
    setPosition(300.f, 100.f);

    attackHitbox.setSize(sf::Vector2f(20.f, 20.f));
    attackHitbox.setFillColor(sf::Color::Transparent);
    attackHitbox.setOutlineColor(sf::Color::Transparent);
    attackHitbox.setOutlineThickness(2.f);
}

Enemy::~Enemy() {}

void Enemy::update()
{
    if (animationClock.getElapsedTime().asSeconds() < animationSpeed)
        return;

    if (isDead)
    {
        setTexture(deadTexture);
        if (currentFrame < deadFrames - 1)
            currentFrame++;
        else
            shouldRemove = true;
    }
    else if (isAttacking)
    {
        setTexture(attackTexture);
        currentFrame++;
        if (currentFrame >= attackFrames)
        {
            currentFrame = 0;
            isAttacking = false;
            setTexture(idleTexture);
        }
    }
    else if (isWalking)
    {
        setTexture(walkTexture);
        currentFrame = (currentFrame + 1) % walkFrames;
    }
    else
    {
        setTexture(idleTexture);
        currentFrame = (currentFrame + 1) % idleFrames;
    }

    setTextureRect(sf::IntRect(currentFrame * frameSize.x, 0, frameSize.x, frameSize.y));
    animationClock.restart();

    if (!canAttack && attackCooldownClock.getElapsedTime().asSeconds() >= attackCooldown)
        canAttack = true;

    if (!isAttacking)
    {
        attackHitbox.setSize(sf::Vector2f(0.f, 0.f));
        return;
    }

    const float attackSize = 30.f;
    attackHitbox.setSize(sf::Vector2f(attackSize, attackSize));
    initializeHitbox();

    sf::FloatRect mainHitbox = hitbox.getGlobalBounds();
    float xPos = (getScale().x > 0) ? mainHitbox.left + mainHitbox.width : mainHitbox.left - attackSize;
    float yPos = mainHitbox.top + (mainHitbox.height - attackSize) / 2;
    attackHitbox.setPosition(xPos, yPos);
}

void Enemy::enemyAI(sf::Vector2f playerPosition, float playerHealth)
{
    if (isDead || playerHealth <= 0.f)
    {
        if (isWalking)
        {
            currentFrame = 0;
            animationClock.restart();
        }
        isIdle = true;
        isWalking = false;
        return;
    }

    float deltaTime = movementClock.restart().asSeconds();
    sf::Vector2f currentPos = getPosition();
    sf::Vector2f oldPos = currentPos;

    const float attackRange = 80.f;
    const float maxHeightDifference = 50.f;
    float distanceToPlayer = std::abs(playerPosition.x - currentPos.x);
    float verticalDistance = std::abs(playerPosition.y - currentPos.y);

    if (distanceToPlayer >= detectionRange)
    {
        if (isWalking)
        {
            currentFrame = 0;
            animationClock.restart();
        }
        isIdle = true;
        isWalking = false;
        isAttacking = false;
        return;
    }

    if (distanceToPlayer < attackRange && canAttack && !isAttacking && verticalDistance < maxHeightDifference)
    {
        isAttacking = true;
        isWalking = false;
        isIdle = false;
        canAttack = false;
        currentFrame = 0;
        animationClock.restart();
        attackCooldownClock.restart();
    }
    else if (!isAttacking)
    {
        if (isIdle)
        {
            currentFrame = 0;
            animationClock.restart();
        }
        isIdle = false;
        isWalking = true;

        bool moveLeft = playerPosition.x < currentPos.x;
        setScale(moveLeft ? -1.f : 1.f, 1.f);
        setOrigin(moveLeft ? frameSize.x : 0, 0);
        move((moveLeft ? -speed : speed) * deltaTime, 0.f);

        if (isStaying)
        {
            sf::Vector2f newPos = getPosition();
            float minX = platformBounds.left;
            float maxX = platformBounds.left + platformBounds.width - getGlobalBounds().width;

            if (newPos.x < minX)
                setPosition(minX, newPos.y);
            else if (newPos.x > maxX)
                setPosition(maxX, newPos.y);
        }
    }

    initializeHitbox();
    for (const auto &rect : collisionList)
    {
        if (hitbox.getGlobalBounds().intersects(rect))
        {
            setPosition(oldPos);
            break;
        }
    }
}

void Enemy::addToCollisionList(sf::FloatRect rect)
{
    collisionList.push_back(rect);
}

void Enemy::initializeHitbox()
{
    const float scale = 0.38f;
    sf::FloatRect bounds = getGlobalBounds();

    float width = bounds.width * scale;
    float height = bounds.height * scale - 15.f;
    float xPos = bounds.left + bounds.width * (1 - scale) / 2;
    float yPos = bounds.top + (bounds.height * (1 - scale) + 107.f) / 2;

    hitbox.setSize(sf::Vector2f(width, height));
    hitbox.setPosition(xPos, yPos);
    hitbox.setFillColor(sf::Color::Transparent);
    hitbox.setOutlineColor(sf::Color::Transparent);
    hitbox.setOutlineThickness(1.f);
}

void Enemy::enemyLogic()
{
    if (isDead)
        return;

    float dt = physicsClock.restart().asSeconds();
    if (dt > 0.1f)
        dt = 0.016f;

    if (isStaying)
    {
        velocity.y = 0.f;
        sf::Vector2f currentPos = getPosition();
        float correctY = platformBounds.top - getGlobalBounds().height;
        setPosition(currentPos.x, correctY);
        initializeHitbox();
        onGround = true;
        return;
    }

    velocity.y += 980.f * dt;
    move(0.f, velocity.y * dt);
    initializeHitbox();
    onGround = false;

    for (const auto &rect : collisionList)
    {
        if (hitbox.getGlobalBounds().intersects(rect))
        {
            if (velocity.y > 0.f)
            {
                float correctY = rect.top - getGlobalBounds().height;
                setPosition(getPosition().x, correctY);
                velocity.y = 0.f;
                onGround = true;
                initializeHitbox();
                break;
            }
        }
    }
}

bool Enemy::takeDamage(float damageAmount)
{
    if (!isDead && !wasHitThisFrame)
    {
        health -= damageAmount;
        wasHitThisFrame = true;

        if (health <= 0.f)
        {
            health = 0.f;
            isDead = true;
            isIdle = false;
            isWalking = false;
            currentFrame = 0;
            animationClock.restart();
            totalKilledEnemies++;
            std::cout << "Enemy died! Total enemies killed: " << totalKilledEnemies << std::endl;
        }
        return true;
    }
    return false;
}

bool Enemy::getIsDead() const { return isDead; }
bool Enemy::shouldBeRemoved() const { return shouldRemove; }
float Enemy::getHealth() const { return health; }
void Enemy::setHealth(float newHealth) { health = newHealth; }
void Enemy::resetHitFlag() { wasHitThisFrame = false; }
float Enemy::getAttackDamage() const { return attackDamage; }
bool Enemy::getIsAttacking() const { return isAttacking; }
size_t Enemy::getTotalKilledEnemies() { return totalKilledEnemies; }
void Enemy::resetKillCounter() { totalKilledEnemies = 0; }

bool Enemy::loadKillInterfaceResources()
{
    if (!killInterfaceLoaded)
    {
        if (!killInterfaceFont.loadFromFile("assets/fonts/GothicA1-Regular.ttf"))
            std::cerr << "Error loading interface font for enemy kill counter" << std::endl;
        if (!killInterfaceTexture.loadFromFile("assets/enemy/Dead.png"))
            std::cerr << "Error loading enemy interface texture" << std::endl;
        killInterfaceLoaded = true;
    }
    return killInterfaceLoaded;
}

void Enemy::getKillInterface(sf::RenderWindow &window)
{
    loadKillInterfaceResources();

    sf::View currentView = window.getView();
    sf::Vector2f viewCenter = currentView.getCenter();
    sf::Vector2f viewSize = currentView.getSize();

    float startX = viewCenter.x - viewSize.x / 2.f + 10.f;
    float startY = viewCenter.y - viewSize.y / 2.f + 80.f;

    sf::Sprite enemyIcon;
    enemyIcon.setTexture(killInterfaceTexture);
    sf::Vector2u textureSize = killInterfaceTexture.getSize();
    float iconSize = 25.f;
    sf::IntRect iconRect(0, 0, textureSize.x / 4, textureSize.y);
    enemyIcon.setTextureRect(iconRect);
    enemyIcon.setScale(iconSize / (textureSize.x / 4), iconSize / textureSize.y);
    enemyIcon.setPosition(startX, startY);

    sf::Text killText;
    killText.setFont(killInterfaceFont);
    killText.setString("x " + std::to_string(getTotalKilledEnemies()));
    killText.setCharacterSize(20);
    killText.setFillColor(sf::Color::Red);
    killText.setPosition(startX + iconSize + 5.f, startY + 2.f);

    window.draw(enemyIcon);
    window.draw(killText);
}

void Enemy::initializeSpawner(const std::vector<sf::Vector2f> &positions, float period, size_t maxEnemies)
{
    spawnPositions = positions;
    spawnPeriod = period;
    maxEnemyCount = maxEnemies;
    currentEnemyCount = 0;
    spawnClock.restart();
}

bool Enemy::shouldSpawnNewEnemy()
{
    if (currentEnemyCount >= maxEnemyCount)
        return false;

    if (spawnClock.getElapsedTime().asSeconds() >= spawnPeriod)
    {
        spawnClock.restart();
        currentEnemyCount++;
        return true;
    }

    return false;
}

sf::Vector2f Enemy::getRandomSpawnPosition()
{
    if (spawnPositions.empty())
        return sf::Vector2f(400.f, 400.f);

    size_t randomIndex = std::rand() % spawnPositions.size();
    return spawnPositions[randomIndex];
}

void Enemy::updateSpawner() {}

void Enemy::resetSpawner()
{
    spawnClock.restart();
    currentEnemyCount = 0;
}

void Enemy::decrementEnemyCount()
{
    if (currentEnemyCount > 0)
        currentEnemyCount--;
}

size_t Enemy::getCurrentEnemyCount()
{
    return currentEnemyCount;
}
