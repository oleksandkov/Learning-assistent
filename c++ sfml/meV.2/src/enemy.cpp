#include <enemy.h>

// Static member definitions
size_t Enemy::totalKilledEnemies = 0;
sf::Font Enemy::killInterfaceFont;
sf::Texture Enemy::killInterfaceTexture;
bool Enemy::killInterfaceLoaded = false;

Enemy::Enemy()
{
    // Frame counts for different animations
    idleFrames = 8;
    walkFrames = 8;
    deadFrames = 3;
    attackFrames = 4;
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
    wasHitThisFrame = false;
    isAttacking = false;

    // AI behavior
    detectionRange = 400.f;
    isStaying = false;
    platformBounds = sf::FloatRect(0.f, 0.f, 0.f, 0.f);

    // Health
    health = 20.f; // Default health for enemy

    // Attack
    attackDamage = 10.f;
    attackCooldown = 1.0f; // 1 second cooldown between attacks
    canAttack = true;

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
    if (!attackTexture.loadFromFile("assets/enemy/Attack_1.png"))
    {
        std::cerr << "Error loading enemy attack texture" << std::endl;
    }

    frameSize.x = idleTexture.getSize().x / idleFrames;
    frameSize.y = idleTexture.getSize().y;

    setTexture(idleTexture);
    setTextureRect(sf::IntRect(0, 0, frameSize.x, frameSize.y));
    setPosition(300.f, 100.f);

    // Initialize attack hitbox
    attackHitbox.setSize(sf::Vector2f(20.f, 20.f));
    attackHitbox.setFillColor(sf::Color::Transparent);
    attackHitbox.setOutlineColor(sf::Color::Yellow);
    attackHitbox.setOutlineThickness(2.f);
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
    else if (isAttacking)
    {
        setTexture(attackTexture);
        if (animationClock.getElapsedTime().asSeconds() >= animationSpeed)
        {
            currentFrame++;
            if (currentFrame >= attackFrames)
            {
                currentFrame = 0;
                isAttacking = false;
                setTexture(idleTexture);
            }
            setTextureRect(sf::IntRect(currentFrame * frameSize.x, 0, frameSize.x, frameSize.y));
            animationClock.restart();
        }
    }
    else // Idle
    {
        setTexture(idleTexture);
        if (animationClock.getElapsedTime().asSeconds() >= animationSpeed)
        {
            currentFrame = (currentFrame + 1) % idleFrames;
            setTextureRect(sf::IntRect(currentFrame * frameSize.x, 0, frameSize.x, frameSize.y));
            animationClock.restart();
        }
    }

    // Update attack cooldown
    if (!canAttack && attackCooldownClock.getElapsedTime().asSeconds() >= attackCooldown)
    {
        canAttack = true;
    }

    // Update attack hitbox position and visibility
    if (isAttacking)
    {
        // Ensure attack hitbox has proper size during attack
        attackHitbox.setSize(sf::Vector2f(30.f, 30.f));

        // Position attack hitbox right next to the enemy's main hitbox
        initializeHitbox(); // Update hitbox first to get current position
        sf::FloatRect mainHitbox = hitbox.getGlobalBounds();
        float attackWidth = 30.f;
        float attackHeight = 30.f;

        if (getScale().x > 0)
        {
            // Facing right - place attack hitbox right after main hitbox
            attackHitbox.setPosition(mainHitbox.left + mainHitbox.width, mainHitbox.top + (mainHitbox.height - attackHeight) / 2);
        }
        else
        {
            // Facing left - place attack hitbox left before main hitbox
            attackHitbox.setPosition(mainHitbox.left - attackWidth, mainHitbox.top + (mainHitbox.height - attackHeight) / 2);
        }
    }
    else
    {
        // Hide attack hitbox when not attacking
        attackHitbox.setSize(sf::Vector2f(0.f, 0.f));
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
        // Check if close enough to attack (attack range)
        float attackRange = 80.f;

        // Check if character is at same height level (within reasonable vertical distance)
        float verticalDistance = std::abs(playerPosition.y - currentPos.y);
        float maxHeightDifference = 50.f; // Allow 50 pixels of height difference

        if (distanceToPlayer < attackRange && canAttack && !isAttacking && verticalDistance < maxHeightDifference)
        {
            // Start attack
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
        isAttacking = false;
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

void Enemy::takeDamage(float damageAmount)
{
    if (!isDead && !wasHitThisFrame)
    {
        health -= damageAmount;
        wasHitThisFrame = true; // Mark as hit this frame

        if (health <= 0.f)
        {
            health = 0.f;
            isDead = true;
            isIdle = false;
            isWalking = false;
            currentFrame = 0;
            animationClock.restart();
            totalKilledEnemies++; // Track the kill
            std::cout << "Enemy died! Total enemies killed: " << totalKilledEnemies << std::endl;
        }
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

float Enemy::getHealth() const
{
    return health;
}

void Enemy::setHealth(float newHealth)
{
    health = newHealth;
}

void Enemy::resetHitFlag()
{
    wasHitThisFrame = false;
}

float Enemy::getAttackDamage() const
{
    return attackDamage;
}

bool Enemy::getIsAttacking() const
{
    return isAttacking;
}

// Static kill tracking methods
size_t Enemy::getTotalKilledEnemies()
{
    return totalKilledEnemies;
}

void Enemy::resetKillCounter()
{
    totalKilledEnemies = 0;
}

bool Enemy::loadKillInterfaceResources()
{
    if (!killInterfaceLoaded)
    {
        if (!killInterfaceFont.loadFromFile("assets/fonts/GothicA1-Regular.ttf"))
        {
            std::cerr << "Error loading interface font for enemy kill counter" << std::endl;
        }
        if (!killInterfaceTexture.loadFromFile("assets/enemy/Dead.png"))
        {
            std::cerr << "Error loading enemy interface texture" << std::endl;
        }
        killInterfaceLoaded = true;
    }
    return killInterfaceLoaded;
}

void Enemy::getKillInterface(sf::RenderWindow &window)
{
    // Ensure resources are loaded
    loadKillInterfaceResources();

    sf::View currentView = window.getView();
    sf::Vector2f viewCenter = currentView.getCenter();
    sf::Vector2f viewSize = currentView.getSize();

    // Position under coin interface (coin interface is at ~50px from top, enemy kills go at ~80px)
    float startX = viewCenter.x - viewSize.x / 2.f + 10.f;
    float startY = viewCenter.y - viewSize.y / 2.f + 80.f; // 80px from top

    // Create enemy skull icon for interface
    sf::Sprite enemyIcon;
    enemyIcon.setTexture(killInterfaceTexture);

    // Scale enemy icon to appropriate size (small skull icon)
    sf::Vector2u textureSize = killInterfaceTexture.getSize();
    float iconSize = 25.f;

    // For enemy spritesheet, we might need to show just one frame
    sf::IntRect iconRect(0, 0, textureSize.x / 4, textureSize.y); // Assuming 4 frames, take first
    enemyIcon.setTextureRect(iconRect);

    enemyIcon.setScale(iconSize / (textureSize.x / 4), iconSize / textureSize.y);
    enemyIcon.setPosition(startX, startY);

    // Create text for enemy kill count
    sf::Text killText;
    killText.setFont(killInterfaceFont);
    killText.setString("x " + std::to_string(getTotalKilledEnemies()));
    killText.setCharacterSize(20);
    killText.setFillColor(sf::Color::Red); // Red color for enemy kills
    killText.setPosition(startX + iconSize + 5.f, startY + 2.f);

    // Draw enemy kill interface
    window.draw(enemyIcon);
    window.draw(killText);
}
