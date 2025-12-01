#include <character.h>

Character::Character()
{
    totalFrames = 8;
    attackFrames = 4;
    hurtFrames = 6;
    animationSpeed = 0.1f;
    currentFrame = 0;
    speed = 250.f;
    isWalking = false;
    velocity = sf::Vector2f(0.f, 0.f);
    onGround = false;
    jumpVelocity = -500.f;
    jumpAnimationSpeed = 0.15f;
    isAttacking = false;
    wasSpacePressed = false;
    isHurt = false;
    health = 100.f;
    damageCooldown = 1.0f; // 1 second cooldown between damage
    canTakeDamage = true;

    if (!idleTexture.loadFromFile("assets/Idle.png"))
    {
        std::cerr << "Error loading idle texture" << std::endl;
    }
    if (!walkTexture.loadFromFile("assets/Walk.png"))
    {
        std::cerr << "Error loading walk texture" << std::endl;
    }
    if (!attackTexture.loadFromFile("assets/Attack_1.png"))
    {
        std::cerr << "Error loading attack texture" << std::endl;
    }
    if (!hurtTexture.loadFromFile("assets/Hurt.png"))
    {
        std::cerr << "Error loading hurt texture" << std::endl;
    }
    // if (!jumpTexture.loadFromFile("assets/Jump.png"))
    //     std::cerr << "Error loading jump texture" << std::endl;

    frameSize.x = idleTexture.getSize().x / totalFrames;
    frameSize.y = idleTexture.getSize().y;

    setTexture(idleTexture);

    setTextureRect(sf::IntRect(0, 0, frameSize.x, frameSize.y));
    setPosition(100.f, 100.f);

    // Initialize attack hitbox
    attackHitbox.setSize(sf::Vector2f(20.f, 20.f));
    attackHitbox.setFillColor(sf::Color::Transparent);
    attackHitbox.setOutlineColor(sf::Color::Blue);
    attackHitbox.setOutlineThickness(2.f);
}

Character::~Character()
{
}

void Character::update()
{
    if (isHurt)
    {
        setTexture(hurtTexture);
        if (animationClock.getElapsedTime().asSeconds() >= animationSpeed)
        {
            currentFrame++;
            if (currentFrame >= hurtFrames)
            {
                currentFrame = 0;
                isHurt = false;
                setTexture(idleTexture);
            }
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
    else if (isWalking)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            setScale(-1.f, 1.f);
            setOrigin(frameSize.x, 0);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            setScale(1.f, 1.f);
            setOrigin(0, 0);
        }
        setTexture(walkTexture);
        if (animationClock.getElapsedTime().asSeconds() >= animationSpeed)
        {
            currentFrame = (currentFrame + 1) % totalFrames;
            setTextureRect(sf::IntRect(currentFrame * frameSize.x, 0, frameSize.x, frameSize.y));
            animationClock.restart();
        }
    }
    else
    {
        setTexture(idleTexture);
        if (animationClock.getElapsedTime().asSeconds() >= animationSpeed)
        {
            currentFrame = (currentFrame + 1) % totalFrames;
            setTextureRect(sf::IntRect(currentFrame * frameSize.x, 0, frameSize.x, frameSize.y));
            animationClock.restart();
        }
    }
}

void Character::moveCharacter()
{
    sf::Vector2f oldpos = getPosition();
    float deltaTime = clock.restart().asSeconds();
    sf::Vector2f movement(0.f, 0.f);
    // if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    //     movement.y += speed * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        movement.x -= speed * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        movement.x += speed * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && onGround)
    {
        velocity.y = jumpVelocity;
        isJumping = true;
    }

    bool isSpacePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    if (isSpacePressed && !wasSpacePressed && !isAttacking)
    {
        isAttacking = true;
        currentFrame = 0;
        animationClock.restart();
    }
    wasSpacePressed = isSpacePressed;

    isWalking = (movement.x != 0.f);
    move(movement);
    initializeHitbox();
    updateAttackHitbox();
    for (const auto &rect : collisionList)
    {
        if (hitbox.getGlobalBounds().intersects(rect))
        {
            setPosition(oldpos);
            break;
        }
    }
}

void Character::addToCollisionList(sf::FloatRect rect)
{
    collisionList.push_back(rect);
}

void Character::initializeHitbox()
{
    float scale = 0.38f; // Adjust this value (0.8 = 80% size)
    sf::FloatRect bounds = getGlobalBounds();
    hitbox.setSize(sf::Vector2f(bounds.width * scale, bounds.height * scale - 15.f));
    hitbox.setPosition(bounds.left + (bounds.width * (1 - scale) / 2), bounds.top + (((bounds.height * (1 - scale)) + 107.f) / 2));
    hitbox.setFillColor(sf::Color::Transparent);
    hitbox.setOutlineColor(sf::Color::Red);
    hitbox.setOutlineThickness(1.f);
}

void Character::updateAttackHitbox()
{
    if (isAttacking && currentFrame >= 1 && currentFrame < attackFrames)
    {
        // Position attack hitbox next to character hitbox
        sf::FloatRect charHitbox = hitbox.getGlobalBounds();
        float attackWidth = 20.f;
        float attackHeight = 40.f;

        attackHitbox.setSize(sf::Vector2f(attackWidth, attackHeight));

        if (getScale().x < 0) // Facing left
        {
            attackHitbox.setPosition(charHitbox.left - attackWidth, charHitbox.top + (charHitbox.height - attackHeight) / 2);
        }
        else // Facing right
        {
            attackHitbox.setPosition(charHitbox.left + charHitbox.width, charHitbox.top + (charHitbox.height - attackHeight) / 2);
        }
    }
    else
    {
        // Hide attack hitbox when not attacking
        attackHitbox.setSize(sf::Vector2f(0.f, 0.f));
    }
}

void Character::characterLogic()
{
    // Update damage cooldown
    if (!canTakeDamage && damageCooldownClock.getElapsedTime().asSeconds() >= damageCooldown)
    {
        canTakeDamage = true;
    }

    float dt = physicsClock.restart().asSeconds();
    if (dt > 0.1f)
        dt = 0.016f;

    // Apply gravity
    velocity.y += 980.f * dt; // Gravity acceleration

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
            // Collision detected - determine if hitting from above or below
            if (velocity.y > 0.f) // Falling down - hit floor/platform from above
            {
                // Position character on top of the collision object
                float correctY = rect.top - getGlobalBounds().height;
                setPosition(getPosition().x, correctY);
                velocity.y = 0.f;
                onGround = true;
                isJumping = false;
                initializeHitbox(); // Update hitbox after position correction
                break;
            }
            else if (velocity.y < 0.f) // Jumping up - hit ceiling/platform from below
            {
                // Push character back slightly to stop intersection
                move(0.f, 1.f);
                velocity.y = 0.f;
                isJumping = false;
                initializeHitbox();
                break;
            }
        }
    }
}

void Character::takeDamage(float damage)
{
    if (canTakeDamage && health > 0)
    {
        health -= damage;
        isHurt = true;
        isAttacking = false;
        currentFrame = 0;
        animationClock.restart();

        if (health < 0)
            health = 0;

        canTakeDamage = false;
        damageCooldownClock.restart();

        std::cout << "Character took damage! Health: " << health << std::endl;
    }
}

float Character::getHealth() const
{
    return health;
}