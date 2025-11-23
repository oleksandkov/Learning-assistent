#include <iostream>
#include <SFML/Graphics.hpp>

class Objects : public sf::RectangleShape, public sf::CircleShape
{
private:
public:
    Objects() {}
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
    if (!texture.loadFromFile("assets/pixel art.jpg"))
    {
        std::cerr << "Error loading texture from assets/pixel art.jpg" << std::endl;
        return -1;
    }

    // Create sprite and set texture
    sf::Sprite sprite;
    sprite.setTexture(texture);
    // Position the sprite, for example, center it
    sprite.setPosition(window.getSize().x / 2.f - texture.getSize().x / 2.f, window.getSize().y / 2.f - texture.getSize().y / 2.f);

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

        window.clear(sf::Color::Black);
        window.draw(background);
        window.draw(floor);
        window.draw(roof);
        window.draw(sprite);

        window.display();
    }

    return 0;
}