#include <iostream>
#include <SFML/Graphics.hpp>
#include <character.h>


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

    Character character;

    character.addToCollisionList(floor);
    // character.addToCollisionList(roof);

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
        character.moveCharacter();

        character.update();

        window.clear(sf::Color::Black);
        window.draw(background);
        window.draw(floor);
        window.draw(roof);
        window.draw(character);

        window.draw(character.hitbox);

        window.display();
    }

    return 0;
}