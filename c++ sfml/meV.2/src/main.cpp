#include <iostream>
#include <SFML/Graphics.hpp>
#include <character.h>
#include <enemy.h>
#include "objects.h"

int main()
{
    // Window settings
    bool isFullscreen = false;
    sf::VideoMode windowedMode(800, 600);
    sf::VideoMode fullscreenMode = sf::VideoMode::getDesktopMode();

    sf::RenderWindow window(windowedMode, "The Game");
    window.setVerticalSyncEnabled(true);

    // Create camera view
    sf::View camera(sf::FloatRect(0.f, 0.f, 900.f, 800.f));
    window.setView(camera);

    // Background setup
    sf::Sprite background;
    sf::Texture backgroundTexture;

    if (!backgroundTexture.loadFromFile("assets/BIGPRE_ORIG_SIZE.png"))
    {
        std::cerr << "Error loading background texture" << std::endl;
    }
    else
    {
        std::cerr << "Background loaded! Size: " << backgroundTexture.getSize().x << "x" << backgroundTexture.getSize().y << std::endl;
        backgroundTexture.setSmooth(true);
        backgroundTexture.setRepeated(true);
        background.setTexture(backgroundTexture);
        background.setScale(0.5f, 0.5f);
    }

    // Map Floor/Roff setrup
    // Setup flloor
    Floor floor(2000.f, 100.f);
    floor.shape.move(0.f, window.getSize().y - 100.f);
    Floor roof(3000.f, -500.f);
    roof.shape.setPosition(0.f, 0.f);
    // Setup platform

    Platform platform2(200.f, 30.f, 1.0f);
    platform2.shape.setPosition(600.f, 380.f);

    Platform platform(200.f, 30.f, 1.0f);
    platform.shape.setPosition(900.f, 330.f);

    // Additional platforms to create a nice map
    Platform platform3(200.f, 30.f, 1.0f);
    platform3.shape.setPosition(1200.f, 380.f);

    Platform platform4(200.f, 30.f, 1.0f);
    platform4.shape.setPosition(1500.f, 310.f);

    Platform platform5(200.f, 30.f, 1.0f);
    platform5.shape.setPosition(1800.f, 260.f);

    Platform platform6(300.f, 30.f, 1.0f);
    platform6.shape.setPosition(1050.f, 170.f);

    Platform platform7(200.f, 30.f, 1.0f);
    platform7.shape.setPosition(1550.f, 145.f);

    // Setrup walls
    Wall leftWall(500.f, window.getSize().y);
    leftWall.shape.setPosition(0.f, 0.f);

    Wall rightWall(500.f, window.getSize().y);
    rightWall.shape.setPosition(2000.f, 0.f);

    // Game over windwo
    sf::RectangleShape gameOverWindow;
    gameOverWindow.setSize(sf::Vector2f(10000.f, 10000.f));
    gameOverWindow.setPosition(0.f, 0.f);
    gameOverWindow.setFillColor(sf::Color(0, 0, 0, 150));
    sf::Text gameOverText;
    sf::Font gameOverFont;
    if (!gameOverFont.loadFromFile("assets/fonts/GothicA1-Regular.ttf"))
    {
        std::cerr << "Error loading font" << std::endl;
    }
    gameOverText.setFont(gameOverFont);
    gameOverText.setString("Game Over");
    gameOverText.setCharacterSize(50);
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setOrigin(gameOverText.getGlobalBounds().width / 2.f, gameOverText.getGlobalBounds().height / 2.f);
    // Create character
    Character character;
    character.setPosition(700.f, 210.f);

    character.addToCollisionList(floor.shape.getGlobalBounds());
    character.addToCollisionList(roof.shape.getGlobalBounds());
    character.addToCollisionList(platform.shape.getGlobalBounds());
    character.addToCollisionList(platform2.shape.getGlobalBounds());
    character.addToCollisionList(platform3.shape.getGlobalBounds());
    character.addToCollisionList(platform4.shape.getGlobalBounds());
    character.addToCollisionList(platform5.shape.getGlobalBounds());
    character.addToCollisionList(platform6.shape.getGlobalBounds());
    character.addToCollisionList(platform7.shape.getGlobalBounds());
    character.addToCollisionList(leftWall.shape.getGlobalBounds());
    character.addToCollisionList(rightWall.shape.getGlobalBounds());

    // Create enemy
    Enemy enemy;
    enemy.setPosition(900.f, 390.f);
    enemy.isStaying = true;
    enemy.platformBounds = platform.shape.getGlobalBounds();

    enemy.addToCollisionList(floor.shape.getGlobalBounds());
    enemy.addToCollisionList(roof.shape.getGlobalBounds());
    enemy.addToCollisionList(platform2.shape.getGlobalBounds());
    enemy.addToCollisionList(platform3.shape.getGlobalBounds());
    enemy.addToCollisionList(platform4.shape.getGlobalBounds());
    enemy.addToCollisionList(platform5.shape.getGlobalBounds());
    enemy.addToCollisionList(platform6.shape.getGlobalBounds());
    enemy.addToCollisionList(platform7.shape.getGlobalBounds());
    enemy.addToCollisionList(leftWall.shape.getGlobalBounds());
    enemy.addToCollisionList(rightWall.shape.getGlobalBounds());
    enemy.setHealth(50.f);

    // Create enemy2
    Enemy enemy2;
    enemy2.setPosition(1350.f, 300.f);
    enemy2.isStaying = true;
    enemy2.platformBounds = platform7.shape.getGlobalBounds();

    enemy2.addToCollisionList(floor.shape.getGlobalBounds());
    enemy2.addToCollisionList(roof.shape.getGlobalBounds());
    enemy2.addToCollisionList(platform2.shape.getGlobalBounds());
    enemy2.addToCollisionList(platform3.shape.getGlobalBounds());
    enemy2.addToCollisionList(platform4.shape.getGlobalBounds());
    enemy2.addToCollisionList(platform5.shape.getGlobalBounds());
    enemy2.addToCollisionList(platform6.shape.getGlobalBounds());
    enemy2.addToCollisionList(platform7.shape.getGlobalBounds());
    enemy2.addToCollisionList(leftWall.shape.getGlobalBounds());
    enemy2.addToCollisionList(rightWall.shape.getGlobalBounds());

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

            // Toggle fullscreen with F11
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F11)
            {
                isFullscreen = !isFullscreen;
                window.close();

                if (isFullscreen)
                {
                    window.create(fullscreenMode, "The Game", sf::Style::Fullscreen);
                }
                else
                {
                    window.create(windowedMode, "The Game");
                }

                window.setVerticalSyncEnabled(true);
                window.setView(camera);
            }
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

        // Check if character's attack hits enemy
        if (character.attackHitbox.getGlobalBounds().intersects(enemy.hitbox.getGlobalBounds()) &&
            character.attackHitbox.getGlobalBounds().width > 0 && !enemy.getIsDead())
        {
            enemy.takeDamage(character.getDamage());
        }

        // Check if character's attack hits enemy2
        if (character.attackHitbox.getGlobalBounds().intersects(enemy2.hitbox.getGlobalBounds()) &&
            character.attackHitbox.getGlobalBounds().width > 0 && !enemy2.getIsDead())
        {
            enemy2.takeDamage(character.getDamage());
        }

        // Check if enemy's attack hits character
        if (enemy.attackHitbox.getGlobalBounds().width > 0 &&
            enemy.attackHitbox.getGlobalBounds().intersects(character.hitbox.getGlobalBounds()) &&
            !character.getIsDead())
        {
            character.takeDamage(enemy.getAttackDamage());
        }

        // Check if enemy2's attack hits character
        if (enemy2.attackHitbox.getGlobalBounds().width > 0 &&
            enemy2.attackHitbox.getGlobalBounds().intersects(character.hitbox.getGlobalBounds()) &&
            !character.getIsDead())
        {
            character.takeDamage(enemy2.getAttackDamage());
        }

        // Reset hit flags only when attack ends (attack hitbox is hidden)
        if (character.attackHitbox.getGlobalBounds().width == 0)
        {
            enemy.resetHitFlag();
            enemy2.resetHitFlag();
        }
        // Update camera to follow character
        camera.setCenter(character.getPosition().x + 50.f, character.getPosition().y);
        window.setView(camera);

        // Update background position with parallax
        float parallaxFactor = 0.3f;
        float bgX = camera.getCenter().x - (camera.getCenter().x * parallaxFactor) - (background.getGlobalBounds().width / 2.0f);
        float bgY = camera.getCenter().y - (camera.getCenter().y * parallaxFactor) - (background.getGlobalBounds().height / 2.0f);
        background.setPosition(bgX + 300.0f, bgY + 100.0f);

        // Background
        window.clear(sf::Color::Black);
        window.draw(background);
        // Map
        window.draw(leftWall.shape);
        window.draw(rightWall.shape);
        window.draw(platform2.shape);
        window.draw(platform.shape);
        window.draw(platform3.shape);
        window.draw(platform4.shape);
        window.draw(platform5.shape);
        window.draw(platform6.shape);
        window.draw(platform7.shape);
        // Essentially Floor/Roof
        window.draw(floor.shape);
        window.draw(roof.shape);
        // Character
        window.draw(character);

        if (!character.getIsDead())
        {
            window.draw(character.hitbox);
            if (character.attackHitbox.getGlobalBounds().width > 0)
                window.draw(character.attackHitbox);
        }
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
            // Draw enemy2 attack hitbox for debugging
            if (enemy2.attackHitbox.getGlobalBounds().width > 0)
                window.draw(enemy2.attackHitbox);
        }

        // Draw enemy attack hitbox for debugging
        if (!enemy.shouldBeRemoved() && enemy.attackHitbox.getGlobalBounds().width > 0)
            window.draw(enemy.attackHitbox);

        // Health interface
        character.getHealthInterface(window);
        if (character.getIsDead())
        {
            // Center game over elements on camera
            sf::Vector2f cameraCenter = camera.getCenter();
            gameOverText.setPosition(cameraCenter.x, cameraCenter.y);
            gameOverWindow.setPosition(cameraCenter.x - 5000.f, cameraCenter.y - 5000.f);

            window.draw(gameOverWindow);
            window.draw(gameOverText);
        }

        window.display();
    }

    return 0;
}