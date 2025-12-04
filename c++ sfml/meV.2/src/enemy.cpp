#include <enemy.h>

Enemy::Enemy()
{
    // Frame counts for different animations
    idleFrames = 8;
    walkFrames = 8;
    deadFrames = 3;
    animationSpeed = 0.15f; // Slower animation for better visibility
    currentFrame = 0;
    speed = 120.f;

    // Physics
    velocity = sf::Vector2f(0.f, 0.f);
    onGround = false;

    // State flags
    isIdle = true;
    isWalking = false;
    isDead = false;
    shouldRemove = false;

    // AI behavior
    detectionRange = 400.f;
    isStaying = false;
    platformBounds = sf::FloatRect(0.f, 0.f, 0.f, 0.f);

    // Load textures
    if (!idleTexture.loadFromFile("assets/enemy/Idle.png"))
    {
        std::cerr << "Error loading enemy idle texture" << std::endl;
    }
    if (!walkTexture.loadFromFile("assets/enemy/Walk.png"))
    {
        std::cerr << "Error loading enemy walk texture" << std::endl;
    }
    if (!deadTexture.loadFromFile("assets/enemy/Dead.png"))
    {
        std::cerr << "Error loading enemy dead texture" << std::endl;
    }

    frameSize.x = idleTexture.getSize().x / idleFrames;
    frameSize.y = idleTexture.getSize().y;

    setTexture(idleTexture);
    setTextureRect(sf::IntRect(0, 0, frameSize.x, frameSize.y));
    setPosition(300.f, 100.f);
}

Enemy::~Enemy()
{
}

void Enemy::update()
{
    if (isDead)
    {
        setTexture(deadTexture);
        if (animationClock.getElapsedTime().asSeconds() >= animationSpeed)
        {
            if (currentFrame < deadFrames - 1)
            {
                currentFrame++;
            }
            else
            {
                shouldRemove = true; // Mark for removal when animation finishes
            }
            setTextureRect(sf::IntRect(currentFrame * frameSize.x, 0, frameSize.x, frameSize.y));
            animationClock.restart();
        }
    }
    else if (isWalking)
    {
        setTexture(walkTexture);
        if (animationClock.getElapsedTime().asSeconds() >= animationSpeed)
        {
            currentFrame = (currentFrame + 1) % walkFrames;
            setTextureRect(sf::IntRect(currentFrame * frameSize.x, 0, frameSize.x, frameSize.y));
            animationClock.restart();
        }
    }
    else // isIdle
    {
        setTexture(idleTexture);
        if (animationClock.getElapsedTime().asSeconds() >= animationSpeed)
        {
            currentFrame = (currentFrame + 1) % idleFrames;
            setTextureRect(sf::IntRect(currentFrame * frameSize.x, 0, frameSize.x, frameSize.y));
            animationClock.restart();
        }
    }
}

void Enemy::enemyAI(sf::Vector2f playerPosition, float playerHealth)
{
    if (isDead)
        return;

    // Stop enemy AI if player is dead
    if (playerHealth <= 0.f)
    {
        if (isWalking) // Transitioning from walking to idle
        {
            currentFrame = 0;
            animationClock.restart();
        }
        isIdle = true;
        isWalking = false;
        return; // Exit early, no need to process AI
    }

    float deltaTime = movementClock.restart().asSeconds();
    sf::Vector2f currentPos = getPosition();
    sf::Vector2f oldPos = currentPos;
    float distanceToPlayer = std::abs(playerPosition.x - currentPos.x);

    // Check if player is in detection range
    if (distanceToPlayer < detectionRange)
    {
        if (isIdle) // Transitioning from idle to walking
        {
            currentFrame = 0; // Reset animation frame
            animationClock.restart();
        }
        isIdle = false;
        isWalking = true;

        if (playerPosition.x < currentPos.x)
        {
            setScale(-1.f, 1.f);
            setOrigin(frameSize.x, 0);
            move(-speed * deltaTime, 0.f);
        }
        else
        {
            setScale(1.f, 1.f);
            setOrigin(0, 0);
            move(speed * deltaTime, 0.f);
        }

        // If isStaying, keep enemy within platform bounds
        if (isStaying)
        {
            sf::Vector2f newPos = getPosition();
            if (newPos.x < platformBounds.left)
            {
                setPosition(platformBounds.left, newPos.y);
            }
            else if (newPos.x + getGlobalBounds().width > platformBounds.left + platformBounds.width)
            {
                setPosition(platformBounds.left + platformBounds.width - getGlobalBounds().width, newPos.y);
            }
        }
    }
    else
    {
        if (isWalking) // Transitioning from walking to idle
        {
            currentFrame = 0; // Reset animation frame
            animationClock.restart();
        }
        isIdle = true;
        isWalking = false;
    }

    // Check collision and revert if needed
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
    float scale = 0.38f;
    sf::FloatRect bounds = getGlobalBounds();
    hitbox.setSize(sf::Vector2f(bounds.width * scale, bounds.height * scale - 15.f));
    hitbox.setPosition(bounds.left + (bounds.width * (1 - scale) / 2), bounds.top + (((bounds.height * (1 - scale)) + 107.f) / 2));
    hitbox.setFillColor(sf::Color::Transparent);
    hitbox.setOutlineColor(sf::Color::Green);
    hitbox.setOutlineThickness(1.f);
}

void Enemy::enemyLogic()
{
    if (isDead)
        return;

    float dt = physicsClock.restart().asSeconds();
    if (dt > 0.1f)
        dt = 0.016f;

    // If isStaying, don't apply gravity - keep enemy on platform
    if (isStaying)
    {
        velocity.y = 0.f;

        // Keep enemy at the platform level
        sf::Vector2f currentPos = getPosition();
        float correctY = platformBounds.top - getGlobalBounds().height;
        setPosition(currentPos.x, correctY);

        // Update hitbox after movement
        initializeHitbox();
        onGround = true;
        return;
    }

    // Apply gravity
    velocity.y += 980.f * dt;

    // Move by velocity
    move(0.f, velocity.y * dt);

    // Update hitbox after movement
    initializeHitbox();

    // Reset ground status
    onGround = false;

    // Check for collisions after vertical movement
    for (const auto &rect : collisionList)
    {
        if (hitbox.getGlobalBounds().intersects(rect))
        {
            if (velocity.y > 0.f) // Falling down
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

void Enemy::takeDamage()
{
    if (!isDead)
    {
        isDead = true;
        isIdle = false;
        isWalking = false;
        currentFrame = 0;
        animationClock.restart();
    }
}

bool Enemy::getIsDead() const
{
    return isDead;
}

bool Enemy::shouldBeRemoved() const
{
    return shouldRemove;
}
