#include <character.h>

Character::Character()
{
    totalFrames = 8; 
    animationSpeed = 0.1f; 
    currentFrame = 0;
    speed = 200.f;

    if (!characterTexture.loadFromFile("assets/Idle.png"))
    {
        std::cerr << "Error loading character texture" << std::endl;
    }

    frameSize.x = characterTexture.getSize().x / totalFrames;
    frameSize.y = characterTexture.getSize().y;

    setTexture(characterTexture);

    setTextureRect(sf::IntRect(0, 0, frameSize.x, frameSize.y));
    setPosition(100.f, 100.f);
}

Character::~Character()
{
}

void Character::update()
{
    if (animationClock.getElapsedTime().asSeconds() >= animationSpeed)
    {
        currentFrame = (currentFrame + 1) % totalFrames;
        setTextureRect(sf::IntRect(currentFrame * frameSize.x, 0, frameSize.x, frameSize.y));
        animationClock.restart();
    }
}

void Character::moveCharacter()
{
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
    move(movement);
}

void Character::addToCollisionList(sf::RectangleShape& rect)
{
    collisionList.push_back(rect.getGlobalBounds());
}

void Character::collisionDetection()
{
    sf::Vector2f oldpos = getPosition();
    for (const auto& rect : collisionList)
    {
        if (getGlobalBounds().intersects(rect))
        {
            setPosition(oldpos);
        }
    }
}