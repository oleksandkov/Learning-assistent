#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>

class Objects : public sf::Sprite
{
private:
    sf::Clock clock;
    float speed;
    std::vector<Objects *> others;

public:
    Objects() {}
    ~Objects() {}

    bool isCharacterMoving()
    {
        return sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
               sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
               sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
               sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    }
    bool loseCondition(sf::RectangleShape *shape)
    {
        if (getGlobalBounds().intersects(shape->getGlobalBounds()))
        {
            return true; // Return true when collision detected
        }
        return false;
    }

    void getCollisionWith(const sf::RectangleShape &shape)
    {
        sf::Vector2f position = this->getPosition();
        if (this->getGlobalBounds().intersects(shape.getGlobalBounds()))
        {
            sf::FloatRect spriteBounds = this->getGlobalBounds();
            sf::FloatRect shapeBounds = shape.getGlobalBounds();

            // Resolve collision with floor (bottom)
            if (spriteBounds.top + spriteBounds.height > shapeBounds.top && spriteBounds.top < shapeBounds.top)
            {
                this->setPosition(this->getPosition().x, shapeBounds.top - spriteBounds.height);
            }

            // Resolve collision with roof (top)
            if (spriteBounds.top < shapeBounds.top + shapeBounds.height && spriteBounds.top + spriteBounds.height > shapeBounds.top + shapeBounds.height)
            {
                this->setPosition(this->getPosition().x, shapeBounds.top + shapeBounds.height);
            }

            // Resolve collision with left wall
            if (spriteBounds.left + spriteBounds.width > shapeBounds.left && spriteBounds.left < shapeBounds.left)
            {
                this->setPosition(shapeBounds.left - spriteBounds.width, this->getPosition().y);
            }

            // Resolve collision with right wall
            if (spriteBounds.left < shapeBounds.left + shapeBounds.width && spriteBounds.left + spriteBounds.width > shapeBounds.left + shapeBounds.width)
            {
                this->setPosition(shapeBounds.left + shapeBounds.width, this->getPosition().y);
            }
        }
    }

    void addToCollisionList(Objects *other)
    {
        others.push_back(other);
    }

    bool globalCollisionCheck()
    {
        for (size_t i = 0; i < others.size(); i++)
        {
            if (getGlobalBounds().intersects(others[i]->getGlobalBounds()))
            {
                return true;
            }
        }
        return false;
    }
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "The Game");
    window.setVerticalSyncEnabled(true);

    // Background setup
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(window.getSize()));
    background.setPosition(0.f, 0.f);
    background.setFillColor(sf::Color(100.f, 149.f, 237.f));
    // Map Floor/Roff setrup
    // Setup flloor
    sf::RectangleShape floor;
    floor.setSize(sf::Vector2f(window.getSize().x, 100.f));
    floor.setPosition(0.f, window.getSize().y - 100.f);
    floor.setFillColor(sf::Color(139, 69, 19));
    floor.setOutlineColor(sf::Color::Black);
    floor.setOutlineThickness(2.f);

    // Setup roof
    sf::RectangleShape roof;
    roof.setSize(sf::Vector2f(window.getSize().x, 50.f));
    roof.setPosition(0.f, 0.f);
    roof.setFillColor(sf::Color(169, 169, 169));
    roof.setOutlineColor(sf::Color::Black);
    roof.setOutlineThickness(2.f);

    // Load texture from images
    sf::Texture staticpos;
    if (!staticpos.loadFromFile("assets/pixe.png"))
    {
        std::cerr << "Error loading texture from assets/pixe.png" << std::endl;
        return -1;
    }
    sf::Texture move1;
    if (!move1.loadFromFile("assets/move1a.png"))
    {
        std::cerr << "Error loading texture from assets/move1a.png" << std::endl;
        return -1;
    }
    sf::Texture move2;
    if (!move2.loadFromFile("assets/move2a.png"))
    {
        std::cerr << "Error loading texture from assets/move2a.png" << std::endl;
        return -1;
    }
    // Character animation logic initialization
    bool isMoving = false;

    static sf::Clock animClock;
    static bool toggle = false;

    Objects sprite;
    sprite.setTexture(staticpos);
    sprite.setOrigin(staticpos.getSize().x / 2.f, staticpos.getSize().y / 2.f);
    sprite.setScale(0.5f, 0.5f);

    sf::Clock clock;
    float speed = 200.f;

    // Main loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();
        }

        // Move sprites
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
        sprite.move(movement);

        // Handle collisions
        sprite.getCollisionWith(floor);
        sprite.getCollisionWith(roof);

        // Character animation logic

        isMoving = sprite.isCharacterMoving();
        if (isMoving && (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            {
                sprite.setScale(0.5f, 0.5f); // Flip horizontally for left
                if (animClock.getElapsedTime().asSeconds() > 0.15f)
                {
                    if (toggle)
                        sprite.setTexture(move1);
                    else
                        sprite.setTexture(move2);
                    toggle = !toggle;
                    animClock.restart();
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            {
                sprite.setScale(-0.5f, 0.5f); // Normal for right
                if (animClock.getElapsedTime().asSeconds() > 0.15f)
                {
                    if (toggle)
                        sprite.setTexture(move1);
                    else
                        sprite.setTexture(move2);
                    toggle = !toggle;
                    animClock.restart();
                }
            }
        }
        else
        {
            sprite.setScale(0.3f, 0.3f);
            sprite.setTexture(staticpos);
        }

        window.clear(sf::Color::Black);
        window.draw(background);
        window.draw(floor);
        window.draw(roof);
        window.draw(sprite);

        window.display();
    }

    return 0;
}