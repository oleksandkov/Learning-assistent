#include <character.h>

Character::Character()
{
    totalFrames = 4; // Assume 4 frames in Idle.png
    animationSpeed = 0.2f; // 0.2 seconds per frame
    currentFrame = 0;

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

void Character::update(float deltaTime)
{
    if (animationClock.getElapsedTime().asSeconds() >= animationSpeed)
    {
        currentFrame = (currentFrame + 1) % totalFrames;
        setTextureRect(sf::IntRect(currentFrame * frameSize.x, 0, frameSize.x, frameSize.y));
        animationClock.restart();
    }
}