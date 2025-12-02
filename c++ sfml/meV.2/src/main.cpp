#include <iostream>
#include <SFML/Graphics.hpp>
#include <character.h>
#include <enemy.h>
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
    background.setSize(sf::Vector2f(5000.f, 3000.f));
    background.setPosition(0.f, 0.f);
    background.setFillColor(sf::Color(100.f, 149.f, 237.f));
    // Map Floor/Roff setrup
    // Setup flloor
    Floor floor(2000.f, 100.f);
    floor.shape.move(0.f, window.getSize().y - 100.f);
    Floor roof(3000.f, -500.f);
    roof.shape.setPosition(0.f, 0.f);
    // Setup platform

    Platform platform2(150.f, 20.f);
    platform2.shape.setPosition(600.f, 330.f);

    // Setrup walls
    Wall leftWall(500.f, window.getSize().y);
    leftWall.shape.setPosition(0.f, 0.f);

    Wall rightWall(500.f, window.getSize().y);
    rightWall.shape.setPosition(2000.f, 0.f);

    Character character;
    character.setPosition(600.f, 100.f);

    character.addToCollisionList(floor.shape.getGlobalBounds());
    character.addToCollisionList(roof.shape.getGlobalBounds());
    // character.addToCollisionList(platform.shape.getGlobalBounds());
    character.addToCollisionList(platform2.shape.getGlobalBounds());
    character.addToCollisionList(leftWall.shape.getGlobalBounds());
    character.addToCollisionList(rightWall.shape.getGlobalBounds());

    // Create enemy
    Enemy enemy;
    enemy.setPosition(900.f, 100.f);

    enemy.addToCollisionList(floor.shape.getGlobalBounds());
    enemy.addToCollisionList(roof.shape.getGlobalBounds());
    enemy.addToCollisionList(platform2.shape.getGlobalBounds());
    enemy.addToCollisionList(leftWall.shape.getGlobalBounds());
    enemy.addToCollisionList(rightWall.shape.getGlobalBounds());

    Enemy enemy2;
    enemy2.setPosition(1200.f, 100.f);

    enemy2.addToCollisionList(floor.shape.getGlobalBounds());
    enemy2.addToCollisionList(roof.shape.getGlobalBounds());
    enemy2.addToCollisionList(platform2.shape.getGlobalBounds());
    enemy2.addToCollisionList(leftWall.shape.getGlobalBounds());
    enemy2.addToCollisionList(rightWall.shape.getGlobalBounds());

    Enemy enemy3;
    enemy3.setPosition(1500.f, 100.f);
    enemy3.addToCollisionList(floor.shape.getGlobalBounds());
    enemy3.addToCollisionList(roof.shape.getGlobalBounds());
    enemy3.addToCollisionList(platform2.shape.getGlobalBounds());
    enemy3.addToCollisionList(leftWall.shape.getGlobalBounds());


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

        // Update enemy
        enemy.enemyAI(character.getPosition(), character.getHealth());
        enemy.update();
        enemy.enemyLogic();

        // Update enemy2
        enemy2.enemyAI(character.getPosition(), character.getHealth());
        enemy2.update();
        enemy2.enemyLogic();

        // Update enemy3
        enemy3.enemyAI(character.getPosition(), character.getHealth());
        enemy3.update();
        enemy3.enemyLogic();

        

        // Check if character's attack hits enemy
        if (character.attackHitbox.getGlobalBounds().intersects(enemy.hitbox.getGlobalBounds()) &&
            character.attackHitbox.getGlobalBounds().width > 0 && !enemy.getIsDead())
        {
            enemy.takeDamage();
        }

        // Check if character's attack hits enemy2
        if (character.attackHitbox.getGlobalBounds().intersects(enemy2.hitbox.getGlobalBounds()) &&
            character.attackHitbox.getGlobalBounds().width > 0 && !enemy2.getIsDead())
        {
            enemy2.takeDamage();
        }
        // Check if character's attack hits enemy3
        if (character.attackHitbox.getGlobalBounds().intersects(enemy3.hitbox.getGlobalBounds()) &&
            character.attackHitbox.getGlobalBounds().width > 0 && !enemy3.getIsDead())
        {
            enemy3.takeDamage();
        }

        // Check if enemy touches character
        if (!enemy.shouldBeRemoved() &&
            enemy.hitbox.getGlobalBounds().intersects(character.hitbox.getGlobalBounds()))
        {
            character.takeDamage(10.f);
        }

        // Check if enemy2 touches character
        if (!enemy2.shouldBeRemoved() &&
            enemy2.hitbox.getGlobalBounds().intersects(character.hitbox.getGlobalBounds()))
        {
            character.takeDamage(10.f);
        }

        // Update camera to follow character
        camera.setCenter(character.getPosition().x + 50.f, character.getPosition().y);
        window.setView(camera);
        // Background
        window.clear(sf::Color::Black);
        window.draw(background);
        // Map
        window.draw(leftWall.shape);
        window.draw(rightWall.shape);
        window.draw(platform2.shape);
        // Essentially Floor/Roof
        window.draw(floor.shape);
        window.draw(roof.shape);
        // Character
        window.draw(character);

        window.draw(character.hitbox);
        if (character.attackHitbox.getGlobalBounds().width > 0)
            window.draw(character.attackHitbox);

        // Enemy
        if (!enemy.shouldBeRemoved())
        {
            window.draw(enemy);
            window.draw(enemy.hitbox);
        }

        // Enemy2
        if (!enemy2.shouldBeRemoved())
        {
            window.draw(enemy2);
            window.draw(enemy2.hitbox);
        }
        // Enemy3
        if (!enemy3.shouldBeRemoved())
        {
            window.draw(enemy3);
            window.draw(enemy3.hitbox);
        }

        window.display();
    }

    return 0;
}