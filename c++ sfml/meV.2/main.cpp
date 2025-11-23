#include <iostream>
#include <SFML/Graphics.hpp>

class Objects : public sf::RectangleShape, public sf::CircleShape
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

    // Load texture from image
    sf::Texture texture;
    if (!texture.loadFromFile("assets/pixe.png"))
    {
        std::cerr << "Error loading texture from assets/pixe.png" << std::endl;
        return -1;
    }
    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setPosition(window.getSize().x / 2.f - texture.getSize().x / 2.f, window.getSize().y / 2.f - texture.getSize().y / 2.f);
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
            if (event.type == sf::Event::Resized)
            {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            };
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F11)
            {
                static bool fullscreen = false;
                fullscreen = !fullscreen;
                window.create(fullscreen ? sf::VideoMode::getDesktopMode() : sf::VideoMode(800, 600), "The Game", fullscreen ? sf::Style::Fullscreen : sf::Style::Default);
                window.setVerticalSyncEnabled(true);
            };
        }

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

        window.clear(sf::Color::Black);
        window.draw(background);
        window.draw(floor);
        window.draw(roof);
        window.draw(sprite);

        window.display();
    }

    return 0;
}