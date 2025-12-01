#include <enemy.h>

Enemy::Enemy()
{
    // Frame counts for different animations
    idleFrames = 8;
    walkFrames = 8;
    jumpFrames = 12;
    deadFrames = 3;

    animationSpeed = 0.1f;
    currentFrame = 0;
    speed = 120.f;

    // Physics
    velocity = sf::Vector2f(0.f, 0.f);
    onGround = false;
    jumpVelocity = -400.f;

    // State flags
    isIdle = true;
    isWalking = false;
    isJumping = false;
    isDead = false;
    shouldRemove = false;

    // AI behavior
    detectionRange = 400.f;

    // Load textures
    if (!idleTexture.loadFromFile("assets/enemy/Idle.png"))
    {
        std::cerr << "Error loading enemy idle texture" << std::endl;
    }
    if (!walkTexture.loadFromFile("assets/enemy/Walk.png"))
    {
        std::cerr << "Error loading enemy walk texture" << std::endl;
    }
    if (!jumpTexture.loadFromFile("assets/enemy/Jump.png"))
    {
        std::cerr << "Error loading enemy jump texture" << std::endl;
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
    if (isJumping)
    {
        setTexture(jumpTexture);
        if (animationClock.getElapsedTime().asSeconds() >= animationSpeed)
        {
            if (currentFrame < jumpFrames - 1)
            {
                currentFrame++;
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
    else if (isDead)
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
    else
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

void Enemy::enemyAI(sf::Vector2f playerPosition)
{
    if (isDead)
        return;

    float deltaTime = movementClock.restart().asSeconds();
    sf::Vector2f currentPos = getPosition();
    sf::Vector2f oldPos = currentPos;
    float distanceToPlayer = std::abs(playerPosition.x - currentPos.x);

    // Check if player is in detection range
    if (distanceToPlayer < detectionRange)
    {
        // Chase player - walk towards them
        isIdle = false;
        isWalking = true;

        if (playerPosition.x < currentPos.x)
        {
            // Move left towards player
            setScale(-1.f, 1.f);
            setOrigin(frameSize.x, 0);
            move(-speed * deltaTime, 0.f);
        }
        else
        {
            // Move right towards player
            setScale(1.f, 1.f);
            setOrigin(0, 0);
            move(speed * deltaTime, 0.f);
        }
    }
    else
    {
        // Player not in range - stay idle
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
                isJumping = false;
                currentFrame = 0; // Reset jump animation
                initializeHitbox();
                break;
            }
            else if (velocity.y < 0.f) // Jumping up
            {
                move(0.f, 1.f);
                velocity.y = 0.f;
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
        isJumping = false;
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
