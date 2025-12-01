#include <iostream>
#include <SFML/Graphics.hpp>
#include <character.h>
#include "objects.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "The Game");

    window.setVerticalSyncEnabled(true);

    // Create camera view
    sf::View camera(sf::FloatRect(0.f, 0.f, 700.f, 400.f));
    window.setView(camera);

    // Background setup
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(window.getSize()));
    background.setPosition(0.f, 0.f);
    background.setFillColor(sf::Color(100.f, 149.f, 237.f));
    // Map Floor/Roff setrup
    // Setup flloor
    Floor floor(2000.f, 100.f);
    floor.shape.move(0.f, window.getSize().y - 100.f);
    // Setup platform
    // Platform platform(200.f, 20.f);
    // platform.shape.setPosition(300.f, 400.f);

    Platform platform2(150.f, 20.f);
    platform2.shape.setPosition(600.f, 330.f);

    // Setup roof
    sf::RectangleShape roof;
    roof.setSize(sf::Vector2f(window.getSize().x, 50.f));
    roof.setPosition(0.f, 0.f);
    roof.setFillColor(sf::Color(169, 169, 169));
    roof.setOutlineColor(sf::Color::Black);
    roof.setOutlineThickness(2.f);

    Character character;

    character.addToCollisionList(floor.shape.getGlobalBounds());
    character.addToCollisionList(roof.getGlobalBounds());
    // character.addToCollisionList(platform.shape.getGlobalBounds());
    character.addToCollisionList(platform2.shape.getGlobalBounds());

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

        character.characterLogic();

        // Update camera to follow character
        camera.setCenter(character.getPosition().x + 50.f, character.getPosition().y);
        window.setView(camera);

        window.clear(sf::Color::Black);
        window.draw(background);
        window.draw(floor.shape);
        // window.draw(platform.shape);
        window.draw(platform2.shape);
        window.draw(roof);
        window.draw(character);

        window.draw(character.hitbox);
        if (character.attackHitbox.getGlobalBounds().width > 0)
            window.draw(character.attackHitbox);

        window.display();
    }

    return 0;
}