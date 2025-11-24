#include <character.h>

Character::Character()
{
    totalFrames = 8; 
    animationSpeed = 0.1f; 
    currentFrame = 0;
    speed = 200.f;
    isWalking = false;

    if (!idleTexture.loadFromFile("assets/Idle.png"))
    {
        std::cerr << "Error loading idle texture" << std::endl;
    }
    if (!walkTexture.loadFromFile("assets/Walk.png"))
    {
        std::cerr << "Error loading walk texture" << std::endl;
    }

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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        movement.y -= speed * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        movement.y += speed * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        movement.x -= speed * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        movement.x += speed * deltaTime;
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

void Character::addToCollisionList(sf::RectangleShape& rect)
{
    collisionList.push_back(rect.getGlobalBounds());
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
