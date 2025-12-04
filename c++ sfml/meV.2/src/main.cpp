#include <iostream>
#include <SFML/Graphics.hpp>
#include <character.h>
#include <enemy.h>
#include "objects.h"
#include "coins.h"

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

    // Vector to hold spawned enemies
    std::vector<Enemy> spawnedEnemies;

    // Setup enemy spawner - define spawn positions on platforms
    std::vector<sf::Vector2f> spawnPositions = {
        sf::Vector2f(650.f, 350.f),  // On platform2
        sf::Vector2f(1000.f, 300.f), // On platform
        sf::Vector2f(1300.f, 350.f), // On platform3
        sf::Vector2f(1600.f, 280.f), // On platform4
        sf::Vector2f(1900.f, 230.f), // On platform5
        sf::Vector2f(700.f, 480.f),  // On floor
        sf::Vector2f(1200.f, 480.f), // On floor
        sf::Vector2f(1800.f, 480.f)  // On floor
    };

    // Initialize spawner with 2.0 second spawn period and max 3 enemies
    Enemy::initializeSpawner(spawnPositions, 2.0f, 3);

    // Create coins and place them on platforms
    Coins coinsManager(2.0f, 10.0f); // 2 second spawn interval, 10 second coin lifetime
    coinsManager.setSpawnInterval(10.0f);
    coinsManager.setDefaultCoinLifetime(20.0f);

    for (float x = 600.f; x <= 800.f; x += 25.f)
    {
        coinsManager.addCoinPosition(x, 315.f); // Above platform only
    }

    // Platform (900, 330) - 200px wide - single layer above
    for (float x = 900.f; x <= 1100.f; x += 25.f)
    {
        coinsManager.addCoinPosition(x, 265.f); // Above platform only
    }

    // Platform 3 (1200, 380) - 200px wide - single layer above
    for (float x = 1200.f; x <= 1400.f; x += 25.f)
    {
        coinsManager.addCoinPosition(x, 315.f); // Above platform only
    }

    // Platform 4 (1500, 310) - 200px wide - single layer above
    for (float x = 1500.f; x <= 1700.f; x += 25.f)
    {
        coinsManager.addCoinPosition(x, 245.f); // Above platform only
    }

    // Platform 5 (1800, 260) - 200px wide - single layer above
    for (float x = 1800.f; x <= 2000.f; x += 25.f)
    {
        coinsManager.addCoinPosition(x, 195.f); // Above platform only
    }

    // Platform 6 (1050, 170) - 300px wide - single layer above
    for (float x = 1050.f; x <= 1350.f; x += 25.f)
    {
        coinsManager.addCoinPosition(x, 105.f); // Above platform only
    }

    // Platform 7 (1550, 145) - 200px wide - single layer above
    for (float x = 1550.f; x <= 1750.f; x += 25.f)
    {
        coinsManager.addCoinPosition(x, 80.f); // Above platform only
    }

    // Higher floor level coins - avoiding walls (left wall: 0-500px, right wall: 2000+px)
    // Safe floor area: 520px to 1980px - moved higher from y=500 to y=450
    for (float x = 520.f; x <= 1980.f; x += 30.f)
    {
        coinsManager.addCoinPosition(x, 450.f); // Higher floor level
    }

    // Additional coins in air between platforms - single layer only
    // Between platform2 and platform (800-900)
    for (float x = 820.f; x <= 880.f; x += 30.f)
    {
        coinsManager.addCoinPosition(x, 420.f); // Single layer between platforms
    }

    // Between platform3 and platform4 (1400-1500)
    for (float x = 1420.f; x <= 1480.f; x += 30.f)
    {
        coinsManager.addCoinPosition(x, 420.f); // Single layer between platforms
    }

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

        // Update coins
        coinsManager.update();

        // Update enemy
        enemy.enemyAI(character.getPosition(), character.getHealth());
        enemy.update();
        enemy.enemyLogic();

        // Update enemy2
        enemy2.enemyAI(character.getPosition(), character.getHealth());
        enemy2.update();
        enemy2.enemyLogic();

        // Spawn new enemies if needed
        if (Enemy::shouldSpawnNewEnemy())
        {
            Enemy newEnemy;
            newEnemy.isStaying = true;

            // Randomly assign a platform based on spawn position
            sf::Vector2f spawnPos = Enemy::getRandomSpawnPosition();
            newEnemy.setPosition(spawnPos.x, spawnPos.y);

            // Assign platform bounds - determine which platform based on X position
            if (spawnPos.x >= 600.f && spawnPos.x <= 800.f && spawnPos.y < 400.f)
                newEnemy.platformBounds = platform2.shape.getGlobalBounds();
            else if (spawnPos.x >= 900.f && spawnPos.x <= 1100.f && spawnPos.y < 400.f)
                newEnemy.platformBounds = platform.shape.getGlobalBounds();
            else if (spawnPos.x >= 1200.f && spawnPos.x <= 1400.f && spawnPos.y < 400.f)
                newEnemy.platformBounds = platform3.shape.getGlobalBounds();
            else if (spawnPos.x >= 1500.f && spawnPos.x <= 1700.f && spawnPos.y < 400.f)
                newEnemy.platformBounds = platform4.shape.getGlobalBounds();
            else if (spawnPos.x >= 1800.f && spawnPos.x <= 2000.f && spawnPos.y < 400.f)
                newEnemy.platformBounds = platform5.shape.getGlobalBounds();
            else
                newEnemy.platformBounds = floor.shape.getGlobalBounds();

            // Add collision list for all platforms
            newEnemy.addToCollisionList(floor.shape.getGlobalBounds());
            newEnemy.addToCollisionList(roof.shape.getGlobalBounds());
            newEnemy.addToCollisionList(platform2.shape.getGlobalBounds());
            newEnemy.addToCollisionList(platform3.shape.getGlobalBounds());
            newEnemy.addToCollisionList(platform4.shape.getGlobalBounds());
            newEnemy.addToCollisionList(platform5.shape.getGlobalBounds());
            newEnemy.addToCollisionList(platform6.shape.getGlobalBounds());
            newEnemy.addToCollisionList(platform7.shape.getGlobalBounds());
            newEnemy.addToCollisionList(leftWall.shape.getGlobalBounds());
            newEnemy.addToCollisionList(rightWall.shape.getGlobalBounds());
            newEnemy.setHealth(30.f);

            spawnedEnemies.push_back(newEnemy);
        }

        // Update all spawned enemies
        for (auto &spawnedEnemy : spawnedEnemies)
        {
            spawnedEnemy.enemyAI(character.getPosition(), character.getHealth());
            spawnedEnemy.update();
            spawnedEnemy.enemyLogic();
        }

        // Remove dead enemies from spawn list
        std::vector<Enemy> enemiesBeforeRemoval = spawnedEnemies;
        spawnedEnemies.erase(
            std::remove_if(spawnedEnemies.begin(), spawnedEnemies.end(),
                           [](Enemy &e)
                           { return e.shouldBeRemoved(); }),
            spawnedEnemies.end());

        // Decrement enemy count for each removed enemy
        size_t removed = enemiesBeforeRemoval.size() - spawnedEnemies.size();
        for (size_t i = 0; i < removed; i++)
        {
            Enemy::decrementEnemyCount();
        }

        // Check coin collection
        coinsManager.checkCollision(character.hitbox.getGlobalBounds());

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

        // Check collisions with spawned enemies
        for (auto &spawnedEnemy : spawnedEnemies)
        {
            // Check if character's attack hits spawned enemy
            if (character.attackHitbox.getGlobalBounds().intersects(spawnedEnemy.hitbox.getGlobalBounds()) &&
                character.attackHitbox.getGlobalBounds().width > 0 && !spawnedEnemy.getIsDead())
            {
                spawnedEnemy.takeDamage(character.getDamage());
            }

            // Check if spawned enemy's attack hits character
            if (spawnedEnemy.attackHitbox.getGlobalBounds().width > 0 &&
                spawnedEnemy.attackHitbox.getGlobalBounds().intersects(character.hitbox.getGlobalBounds()) &&
                !character.getIsDead())
            {
                character.takeDamage(spawnedEnemy.getAttackDamage());
            }
        }

        // Reset hit flags only when attack ends (attack hitbox is hidden)
        if (character.attackHitbox.getGlobalBounds().width == 0)
        {
            enemy.resetHitFlag();
            enemy2.resetHitFlag();
            // Reset hit flags for all spawned enemies
            for (auto &spawnedEnemy : spawnedEnemies)
            {
                spawnedEnemy.resetHitFlag();
            }
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

        // Draw coins
        coinsManager.draw(window);

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

        // Draw spawned enemies
        for (auto &spawnedEnemy : spawnedEnemies)
        {
            if (!spawnedEnemy.shouldBeRemoved())
            {
                window.draw(spawnedEnemy);
                window.draw(spawnedEnemy.hitbox);
                // Draw spawned enemy attack hitbox for debugging
                if (spawnedEnemy.attackHitbox.getGlobalBounds().width > 0)
                    window.draw(spawnedEnemy.attackHitbox);
            }
        }

        // Draw enemy attack hitbox for debugging
        if (!enemy.shouldBeRemoved() && enemy.attackHitbox.getGlobalBounds().width > 0)
            window.draw(enemy.attackHitbox);

        // Health interface
        character.getHealthInterface(window);

        // Coin interface
        coinsManager.getCoinInterface(window);

        // Enemy kill interface
        Enemy::getKillInterface(window);

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