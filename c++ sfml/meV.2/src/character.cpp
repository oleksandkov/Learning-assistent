#include <character.h>

Character::Character()
{
    totalFrames = 8;
    attackFrames = 4;
    hurtFrames = 6;
    deadFrames = 3;
    animationSpeed = 0.1f;
    jumpAnimationSpeed = 0.15f;
    currentFrame = 0;
    speed = 250.f;
    jumpVelocity = -500.f;
    health = 100.f;
    damageCooldown = 1.0f;
    damage = 20.f;
    isWalking = false;
    isAttacking = false;
    wasSpacePressed = false;
    isHurt = false;
    isDead = false;
    onGround = false;
    canTakeDamage = true;
    velocity = sf::Vector2f(0.f, 0.f);

    if (!idleTexture.loadFromFile("assets/Idle.png"))
        std::cerr << "Error loading idle texture" << std::endl;
    if (!walkTexture.loadFromFile("assets/Walk.png"))
        std::cerr << "Error loading walk texture" << std::endl;
    if (!attackTexture.loadFromFile("assets/Attack_1.png"))
        std::cerr << "Error loading attack texture" << std::endl;
    if (!hurtTexture.loadFromFile("assets/Hurt.png"))
        std::cerr << "Error loading hurt texture" << std::endl;
    if (!deadTexture.loadFromFile("assets/Dead.png"))
        std::cerr << "Error loading dead texture" << std::endl;

    frameSize.x = idleTexture.getSize().x / totalFrames;
    frameSize.y = idleTexture.getSize().y;
    setTexture(idleTexture);
    setTextureRect(sf::IntRect(0, 0, frameSize.x, frameSize.y));
    setPosition(100.f, 100.f);

    attackHitbox.setSize(sf::Vector2f(20.f, 20.f));
    attackHitbox.setFillColor(sf::Color::Transparent);
    attackHitbox.setOutlineColor(sf::Color::Blue);
    attackHitbox.setOutlineThickness(2.f);
}

Character::~Character() {}

void Character::update()
{
    if (isDead)
    {
        setTexture(deadTexture);
        if (animationClock.getElapsedTime().asSeconds() >= animationSpeed)
        {
            if (currentFrame < deadFrames - 1)
                currentFrame++;
            setTextureRect(sf::IntRect(currentFrame * frameSize.x, 0, frameSize.x, frameSize.y));
            animationClock.restart();
        }
        return;
    }
    else if (isHurt)
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
    if (isDead)
        return;

    sf::Vector2f oldpos = getPosition();
    float deltaTime = clock.restart().asSeconds();
    sf::Vector2f movement(0.f, 0.f);

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

void Character::updateAttackHitbox()
{
    if (isAttacking && currentFrame >= 1 && currentFrame < attackFrames)
    {
        sf::FloatRect charHitbox = hitbox.getGlobalBounds();
        float attackWidth = 20.f;
        float attackHeight = 40.f;
        attackHitbox.setSize(sf::Vector2f(attackWidth, attackHeight));

        if (getScale().x < 0)
            attackHitbox.setPosition(charHitbox.left - attackWidth, charHitbox.top + (charHitbox.height - attackHeight) / 2);
        else
            attackHitbox.setPosition(charHitbox.left + charHitbox.width, charHitbox.top + (charHitbox.height - attackHeight) / 2);
    }
    else
    {
        attackHitbox.setSize(sf::Vector2f(0.f, 0.f));
    }
}

void Character::addToCollisionList(sf::FloatRect rect)
{
    collisionList.push_back(rect);
}

void Character::initializeHitbox()
{
    float scale = 0.38f;
    sf::FloatRect bounds = getGlobalBounds();
    hitbox.setSize(sf::Vector2f(bounds.width * scale, bounds.height * scale - 15.f));
    hitbox.setPosition(bounds.left + (bounds.width * (1 - scale) / 2), bounds.top + (((bounds.height * (1 - scale)) + 107.f) / 2));
    hitbox.setFillColor(sf::Color::Transparent);
    hitbox.setOutlineColor(sf::Color::Red);
    hitbox.setOutlineThickness(1.f);
}

void Character::characterLogic()
{
    if (!canTakeDamage && damageCooldownClock.getElapsedTime().asSeconds() >= damageCooldown)
        canTakeDamage = true;

    float dt = physicsClock.restart().asSeconds();
    if (dt > 0.1f)
        dt = 0.016f;

    sf::Vector2f oldPosition = getPosition();
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
                setPosition(oldPosition);
                initializeHitbox();
                velocity.y = 0.f;
                onGround = true;
                isJumping = false;
                break;
            }
            else if (velocity.y < 0.f)
            {
                setPosition(oldPosition);
                initializeHitbox();
                velocity.y = 0.f;
                isJumping = false;
                break;
            }
        }
    }
}

void Character::takeDamage(float damage)
{
    if (canTakeDamage && health > 0 && !isDead)
    {
        health -= damage;

        if (health <= 0)
        {
            health = 0;
            isDead = true;
            isHurt = false;
            isAttacking = false;
            currentFrame = 0;
            animationClock.restart();
            std::cout << "Character died!" << std::endl;
        }
        else
        {
            isHurt = true;
            isAttacking = false;
            currentFrame = 0;
            animationClock.restart();
            std::cout << "Character took damage! Health: " << health << std::endl;
        }

        canTakeDamage = false;
        damageCooldownClock.restart();
    }
}

void Character::getHealthInterface(sf::RenderWindow &window)
{
    static bool heartLoaded = false;
    if (!heartLoaded)
    {
        if (!heartTexture.loadFromFile("assets/Heart_pixelart_(transparent_background).svg.png"))
            std::cerr << "Error loading heart texture" << std::endl;
        heartLoaded = true;
    }

    sf::View currentView = window.getView();
    sf::Vector2f viewCenter = currentView.getCenter();
    sf::Vector2f viewSize = currentView.getSize();

    int numHearts = static_cast<int>(health / 20.f);
    if (health > 0 && numHearts == 0)
        numHearts = 1;

    hearts.clear();

    float heartSize = 30.f;
    float heartSpacing = 5.f;
    float startX = viewCenter.x - viewSize.x / 2.f + 10.f;
    float startY = viewCenter.y - viewSize.y / 2.f + 10.f;

    for (int i = 0; i < numHearts && i < 10; i++)
    {
        sf::Sprite heart;
        heart.setTexture(heartTexture);
        sf::Vector2u textureSize = heartTexture.getSize();
        heart.setScale(heartSize / textureSize.x, heartSize / textureSize.y);
        heart.setPosition(startX + i * (heartSize + heartSpacing), startY);
        hearts.push_back(heart);
        window.draw(heart);
    }
}

float Character::getHealth() const { return health; }
float Character::getDamage() const { return damage; }
bool Character::getIsAttacking() const { return isAttacking; }
bool Character::getIsDead() const { return isDead; }