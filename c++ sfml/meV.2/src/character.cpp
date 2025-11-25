#include <character.h>

Character::Character()
{
    totalFrames = 8; 
    animationSpeed = 0.1f; 
    currentFrame = 0;
    speed = 200.f;
    isWalking = false;
    velocity = sf::Vector2f(0.f, 0.f);
    onGround = false;
    jumpVelocity = -500.f;

    if (!idleTexture.loadFromFile("assets/Idle.png"))
    {
        std::cerr << "Error loading idle texture" << std::endl;
    }
    if (!walkTexture.loadFromFile("assets/Walk.png"))
    {
        std::cerr << "Error loading walk texture" << std::endl;
    }
    // if (!jumpTexture.loadFromFile("assets/Jump.png"))
    //     std::cerr << "Error loading jump texture" << std::endl;

    frameSize.x = idleTexture.getSize().x / totalFrames;
    frameSize.y = idleTexture.getSize().y;

    setTexture(idleTexture);

    setTextureRect(sf::IntRect(0, 0, frameSize.x, frameSize.y));
    setPosition(100.f, 100.f);
}

Character::~Character()
{
}

void Character::update()
{
    if (isWalking)
    {
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            setScale(-1.f, 1.f); 
            setOrigin(frameSize.x, 0);
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            setScale(1.f, 1.f); 
            setOrigin(0, 0);
        }
        setTexture(walkTexture);
    }
    else
    {
        setTexture(idleTexture);
    }
    if (animationClock.getElapsedTime().asSeconds() >= animationSpeed)
    {
        currentFrame = (currentFrame + 1) % totalFrames;
        setTextureRect(sf::IntRect(currentFrame * frameSize.x, 0, frameSize.x, frameSize.y));
        animationClock.restart();
    }
}

void Character::moveCharacter()
{
    sf::Vector2f oldpos = getPosition();
    float deltaTime = clock.restart().asSeconds();
    sf::Vector2f movement(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        movement.y += speed * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        movement.x -= speed * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        movement.x += speed * deltaTime;
    // Jumping
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && onGround) {
        velocity.y = jumpVelocity;
        isJumping = true;
    }
    isWalking = (movement.x != 0.f);
    move(movement);
    initializeHitbox();
    for (const auto& rect : collisionList)
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
    hitbox.setPosition(bounds.left + (bounds.width * (1 - scale) / 2), bounds.top + (((bounds.height * (1 - scale)) + 107.f)/ 2));
    hitbox.setFillColor(sf::Color::Transparent);
    hitbox.setOutlineColor(sf::Color::Red);
    hitbox.setOutlineThickness(1.f);
}

void Character::characterLogic() {
    float dt = 0.016f; // Approximate delta time for 60 FPS

    // Check if on ground (intersects any collision object)
    onGround = false;
    for (const auto& rect : collisionList) {
        if (getGlobalBounds().intersects(rect)) {
            onGround = true;
            break;
        }
    }

    // Apply gravity if not on ground
    if (!onGround) {
        velocity.y += 980.f * dt; // Gravity acceleration
    } else {
        velocity.y = 0.f;
    }

    // Move by velocity
    move(0.f, velocity.y * dt);

    // Check for collision after movement with any object
    for (const auto& rect : collisionList) {
        if (getGlobalBounds().intersects(rect)) {
            setPosition(getPosition().x, rect.top - getGlobalBounds().height);
            velocity.y = 0.f;
            onGround = true;
            isJumping = false;
            break; // Stop at the first collision (highest object)
        }
    }
}
