#include <iostream>
#include <SFML/Graphics.hpp>

class Objects : public sf::Sprite
{
private:
    sf::Clock clock;
    float speed;

public:
    Objects() : speed(200.f) {}
    ~Objects() {}
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

    // Load textures for animation
    sf::Texture texture1, texture2;
    if (!texture1.loadFromFile("assets/move1a.png"))
    {
        std::cerr << "Error loading texture from assets/move1a.png" << std::endl;
        return -1;
    }
    if (!texture2.loadFromFile("assets/move2a.png"))
    {
        std::cerr << "Error loading texture from assets/move2a.png" << std::endl;
        return -1;
    }
    sf::Sprite sprite;
    sprite.setTexture(texture1);
    sprite.setOrigin(texture1.getSize().x / 2.f, texture1.getSize().y / 2.f); // Center origin for flipping
    sprite.setPosition(window.getSize().x / 2.f - texture1.getSize().x / 2.f, window.getSize().y / 2.f - texture1.getSize().y / 2.f);
    sprite.setScale(0.5f, 0.5f);

    sf::Clock clock;
    float speed = 200.f;
    sf::Clock animationClock;
    float animationTime = 0.2f; // Switch texture every 0.2 seconds
    bool isMoving = false;

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
            if (event.type == sf::Event::Resized)
            {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
                sprite.setPosition(event.size.width / 2.f, event.size.height / 2.f);
            };
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F11)
            {
                static bool fullscreen = false;
                fullscreen = !fullscreen;
                window.create(fullscreen ? sf::VideoMode::getDesktopMode() : sf::VideoMode(800, 600), "The Game", fullscreen ? sf::Style::Fullscreen : sf::Style::Default);
                window.setVerticalSyncEnabled(true);
                sprite.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
            };
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

        // Animation and direction
        isMoving = (movement.x != 0.f || movement.y != 0.f);
        if (isMoving) {
            animationTime -= deltaTime;
            if (animationTime <= 0.f) {
                static bool alt = false;
                alt = !alt;
                sprite.setTexture(alt ? texture1 : texture2);
                animationTime = 0.2f;
            }
            // Set scale based on horizontal direction
            if (movement.x < 0) { // moving left
                sprite.setScale(-0.5f, 0.5f);
            } else if (movement.x > 0) { // moving right
                sprite.setScale(0.5f, 0.5f);
            }
        } else {
            // Not moving, reset to default
            sprite.setScale(0.5f, 0.5f);
            sprite.setTexture(texture1);
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