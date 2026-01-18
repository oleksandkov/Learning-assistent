#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <character.h>
#include <enemy.h>
#include "objects.h"
#include "coins.h"

int main()
{
    bool isFullscreen = false;
    bool gameOver = false;
    sf::VideoMode windowedMode(800, 600);
    sf::VideoMode fullscreenMode = sf::VideoMode::getDesktopMode();

    sf::RenderWindow window(windowedMode, "Slime Face Breaker");
    window.setVerticalSyncEnabled(true);

    sf::View camera(sf::FloatRect(0.f, 0.f, 600.f, 400.f));
    window.setView(camera);

    sf::Sprite background;
    sf::Texture backgroundTexture;

    if (!backgroundTexture.loadFromFile("assets/BIGPRE_ORIG_SIZE.png"))
        std::cerr << "Error loading background texture" << std::endl;
    else
    {
        std::cerr << "Background loaded! Size: " << backgroundTexture.getSize().x << "x" << backgroundTexture.getSize().y << std::endl;
        backgroundTexture.setSmooth(true);
        backgroundTexture.setRepeated(true);
        background.setTexture(backgroundTexture);
        background.setScale(0.5f, 0.5f);
    }

    Floor floor(2000.f, 100.f);
    floor.shape.move(0.f, window.getSize().y - 100.f);
    Floor roof(3000.f, -500.f);
    roof.shape.setPosition(0.f, 0.f);

    Platform platform2(200.f, 30.f, 1.0f);
    platform2.shape.setPosition(600.f, 380.f);
    Platform platform(200.f, 30.f, 1.0f);
    platform.shape.setPosition(900.f, 330.f);
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

    Wall leftWall(500.f, window.getSize().y);
    leftWall.shape.setPosition(0.f, 0.f);
    Wall rightWall(500.f, window.getSize().y);
    rightWall.shape.setPosition(2000.f, 0.f);

    sf::RectangleShape gameOverWindow;
    gameOverWindow.setSize(sf::Vector2f(10000.f, 10000.f));
    gameOverWindow.setPosition(0.f, 0.f);
    gameOverWindow.setFillColor(sf::Color(0, 0, 0, 150));
    sf::Text gameOverText;
    sf::Font gameOverFont;
    if (!gameOverFont.loadFromFile("assets/fonts/GothicA1-Regular.ttf"))
        std::cerr << "Error loading font" << std::endl;
    gameOverText.setFont(gameOverFont);
    gameOverText.setString("Game Over");
    gameOverText.setCharacterSize(50);
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setOrigin(gameOverText.getGlobalBounds().width / 2.f, gameOverText.getGlobalBounds().height / 2.f);

    // Win condition and win window
    sf::RectangleShape winWindow;
    winWindow.setSize(sf::Vector2f(10000.f, 10000.f));
    winWindow.setPosition(0.f, 0.f);
    winWindow.setFillColor(sf::Color(255, 255, 255, 200));
    sf::Text winText;
    sf::Font winFont;
    if (!winFont.loadFromFile("assets/fonts/GothicA1-Regular.ttf"))
        std::cerr << "Error loading font" << std::endl;
    winText.setFont(winFont);
    winText.setString("You Win!");
    winText.setCharacterSize(50);
    winText.setFillColor(sf::Color::Black);
    winText.setOrigin(winText.getGlobalBounds().width / 2.f, winText.getGlobalBounds().height / 2.f);
    winText.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);

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


    std::vector<Enemy> spawnedEnemies;

    std::vector<sf::Vector2f> spawnPositions = {
        sf::Vector2f(650.f, 350.f),
        sf::Vector2f(1000.f, 300.f),
        sf::Vector2f(1300.f, 350.f),
        sf::Vector2f(1600.f, 280.f),
        sf::Vector2f(1900.f, 230.f),
        sf::Vector2f(700.f, 480.f),
        sf::Vector2f(1200.f, 480.f),
        sf::Vector2f(1800.f, 480.f)};

    Enemy::initializeSpawner(spawnPositions, 2.0f, 3);

    Coins coinsManager(2.0f, 10.0f);
    coinsManager.setSpawnInterval(2.0f);
    coinsManager.setDefaultCoinLifetime(20.0f);

    for (float x = 600.f; x <= 800.f; x += 25.f)
        coinsManager.addCoinPosition(x, 315.f);
    for (float x = 900.f; x <= 1100.f; x += 25.f)
        coinsManager.addCoinPosition(x, 265.f);
    for (float x = 1200.f; x <= 1400.f; x += 25.f)
        coinsManager.addCoinPosition(x, 315.f);
    for (float x = 1500.f; x <= 1700.f; x += 25.f)
        coinsManager.addCoinPosition(x, 245.f);
    for (float x = 1800.f; x <= 2000.f; x += 25.f)
        coinsManager.addCoinPosition(x, 195.f);
    for (float x = 1050.f; x <= 1350.f; x += 25.f)
        coinsManager.addCoinPosition(x, 105.f);
    for (float x = 1550.f; x <= 1750.f; x += 25.f)
        coinsManager.addCoinPosition(x, 80.f);
    for (float x = 520.f; x <= 1980.f; x += 30.f)
        coinsManager.addCoinPosition(x, 450.f);
    for (float x = 820.f; x <= 880.f; x += 30.f)
        coinsManager.addCoinPosition(x, 420.f);
    for (float x = 1420.f; x <= 1480.f; x += 30.f)
        coinsManager.addCoinPosition(x, 420.f);

        // Buttons

        Button restartbutton;
        restartbutton.setSize(sf::Vector2f(200.f, 80.f));
        restartbutton.setPosition(1000.f, 500.f);
        restartbutton.setFillColor(sf::Color(100, 250, 50));
        restartbutton.setFont(gameOverFont);
        restartbutton.setButtonText("Restart", 24, sf::Color::Black);

        // Sound buffers and sounds
        sf::SoundBuffer coinBuffer;
        
        
        sf::Sound coinSound;


        // Load sound files (assuming they exist in assets/sounds/)
        if (!coinBuffer.loadFromFile("assets/sounds/coin.wav"))
            std::cerr << "Error loading coin sound" << std::endl;
        else
            coinSound.setBuffer(coinBuffer);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();

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

            // Handle restart button click when game is over
            if (gameOver && restartbutton.isClicked(event, window))
            {
                // Reset game state
                gameOver = false;
                                
                // Recreate character to reset all states (this automatically sets health to 100)
                character = Character();
                character.setPosition(700.f, 210.f);
                
                // Re-add collision lists for character
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
                
                // Clear spawned enemies
                spawnedEnemies.clear();
                
                // Reset enemy spawner and counters
                Enemy::resetKillCounter();
                Enemy::resetSpawner();
                
                // Reinitialize enemy spawner with fresh settings
                Enemy::initializeSpawner(spawnPositions, 2.0f, 3);
                
                // Reset coins completely
                coinsManager.clear();
                
                // Reset coin spawn timer by recreating coins manager
                coinsManager = Coins(10.0f, 10.0f);
                
                // Re-add all coin spawn positions
                for (float x = 600.f; x <= 800.f; x += 25.f)
                    coinsManager.addCoinPosition(x, 315.f);
                for (float x = 900.f; x <= 1100.f; x += 25.f)
                    coinsManager.addCoinPosition(x, 265.f);
                for (float x = 1200.f; x <= 1400.f; x += 25.f)
                    coinsManager.addCoinPosition(x, 315.f);
                for (float x = 1500.f; x <= 1700.f; x += 25.f)
                    coinsManager.addCoinPosition(x, 245.f);
                for (float x = 1800.f; x <= 2000.f; x += 25.f)
                    coinsManager.addCoinPosition(x, 195.f);
                for (float x = 1050.f; x <= 1350.f; x += 25.f)
                    coinsManager.addCoinPosition(x, 105.f);
                for (float x = 1550.f; x <= 1750.f; x += 25.f)
                    coinsManager.addCoinPosition(x, 80.f);
                for (float x = 520.f; x <= 1980.f; x += 30.f)
                    coinsManager.addCoinPosition(x, 450.f);
                for (float x = 820.f; x <= 880.f; x += 30.f)
                    coinsManager.addCoinPosition(x, 420.f);
                for (float x = 1420.f; x <= 1480.f; x += 30.f)
                    coinsManager.addCoinPosition(x, 420.f);
                
            
                
                // Reset camera
                camera.setCenter(character.getPosition().x + 50.f, character.getPosition().y);
                window.setView(camera);
                

            }
        }

        if(!gameOver)
        {

            character.moveCharacter();
            character.update();
            character.characterLogic();
        };

        coinsManager.update();


        if (Enemy::shouldSpawnNewEnemy())
        {
            Enemy newEnemy;
            newEnemy.isStaying = true;

            sf::Vector2f spawnPos = Enemy::getRandomSpawnPosition();
            newEnemy.setPosition(spawnPos.x, spawnPos.y);

            struct PlatformZone
            {
                float minX, maxX;
                sf::FloatRect bounds;
            };
            PlatformZone zones[] = {
                {600.f, 800.f, platform2.shape.getGlobalBounds()},
                {900.f, 1100.f, platform.shape.getGlobalBounds()},
                {1200.f, 1400.f, platform3.shape.getGlobalBounds()},
                {1500.f, 1700.f, platform4.shape.getGlobalBounds()},
                {1800.f, 2000.f, platform5.shape.getGlobalBounds()}};

            bool foundPlatform = false;
            for (const auto &zone : zones)
            {
                if (spawnPos.x >= zone.minX && spawnPos.x <= zone.maxX && spawnPos.y < 400.f)
                {
                    newEnemy.platformBounds = zone.bounds;
                    foundPlatform = true;
                    break;
                }
            }

            if (!foundPlatform)
                newEnemy.platformBounds = floor.shape.getGlobalBounds();

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

        for (auto &spawnedEnemy : spawnedEnemies)
        {
            spawnedEnemy.enemyAI(character.getPosition(), character.getHealth());
            spawnedEnemy.update();
            spawnedEnemy.enemyLogic();
        }

        std::vector<Enemy> enemiesBeforeRemoval = spawnedEnemies;
        spawnedEnemies.erase(
            std::remove_if(spawnedEnemies.begin(), spawnedEnemies.end(),
                           [](Enemy &e)
                           { return e.shouldBeRemoved(); }),
            spawnedEnemies.end());

        size_t removed = enemiesBeforeRemoval.size() - spawnedEnemies.size();
        for (size_t i = 0; i < removed; i++)
            Enemy::decrementEnemyCount();

        int coinsCollected = coinsManager.checkCollision(character.hitbox.getGlobalBounds());
        if (coinsCollected > 0)
            coinSound.play();

        if (character.attackHitbox.getGlobalBounds().width > 0)
        {

            for (auto &spawnedEnemy : spawnedEnemies)
            {
                if (!spawnedEnemy.getIsDead() && character.attackHitbox.getGlobalBounds().intersects(spawnedEnemy.hitbox.getGlobalBounds()))
                    spawnedEnemy.takeDamage(character.getDamage());
            }
        }
        else
        {
            for (auto &spawnedEnemy : spawnedEnemies)
                spawnedEnemy.resetHitFlag();
        }

        if (!character.getIsDead())
        {


            for (auto &spawnedEnemy : spawnedEnemies)
            {
                if (spawnedEnemy.attackHitbox.getGlobalBounds().width > 0 && spawnedEnemy.attackHitbox.getGlobalBounds().intersects(character.hitbox.getGlobalBounds()))
                    character.takeDamage(spawnedEnemy.getAttackDamage());
            }
        }

        camera.setCenter(character.getPosition().x + 50.f, character.getPosition().y);
        window.setView(camera);

        float parallaxFactor = 0.3f;
        float bgX = camera.getCenter().x - (camera.getCenter().x * parallaxFactor) - (background.getGlobalBounds().width / 2.0f);
        float bgY = camera.getCenter().y - (camera.getCenter().y * parallaxFactor) - (background.getGlobalBounds().height / 2.0f);
        background.setPosition(bgX + 300.0f, bgY + 100.0f);

        window.clear(sf::Color::Black);
        window.draw(background);

        window.draw(leftWall.shape);
        window.draw(rightWall.shape);
        window.draw(platform2.shape);
        window.draw(platform.shape);
        window.draw(platform3.shape);
        window.draw(platform4.shape);
        window.draw(platform5.shape);
        window.draw(platform6.shape);
        window.draw(platform7.shape);
        window.draw(floor.shape);
        window.draw(roof.shape);

        coinsManager.draw(window);

        window.draw(character);
        if (!character.getIsDead())
        {
            window.draw(character.hitbox);
            if (character.attackHitbox.getGlobalBounds().width > 0)
                window.draw(character.attackHitbox);
        }

        

        for (auto &spawnedEnemy : spawnedEnemies)
        {
            if (!spawnedEnemy.shouldBeRemoved())
            {
                window.draw(spawnedEnemy);
                window.draw(spawnedEnemy.hitbox);
                if (spawnedEnemy.attackHitbox.getGlobalBounds().width > 0)
                    window.draw(spawnedEnemy.attackHitbox);
            }
        }

 

        if (character.getIsDead())
        {
            sf::Vector2f cameraCenter = camera.getCenter();
            gameOverText.setPosition(cameraCenter.x, cameraCenter.y);
            gameOverWindow.setPosition(cameraCenter.x - 5000.f, cameraCenter.y - 5000.f);
            window.draw(gameOverWindow);
            window.draw(gameOverText);
            gameOver = true;

            // Position restart button below the game over text
            sf::FloatRect gameOverBounds = gameOverText.getGlobalBounds();
            restartbutton.setPosition(cameraCenter.x - restartbutton.getSize().x / 2.f, 
                                    gameOverBounds.top + gameOverBounds.height + 50.f);
            
            restartbutton.setButtonText("Restart", 24, sf::Color::Black);
            restartbutton.isHover(window);
            window.draw(restartbutton);
            window.draw(restartbutton.getText());
        }

        size_t totalKilledEnemies = Enemy::getTotalKilledEnemies();


        if (coinsManager.getCollectedCoinsCount() >= 4 && totalKilledEnemies >= 4)
        {
            sf::Vector2f cameraCenter = camera.getCenter();
            sf::Vector2f cameraSize = camera.getSize();
            
            // Center the win window on the camera view
            winWindow.setPosition(cameraCenter.x - cameraSize.x/2.f, cameraCenter.y - cameraSize.y/2.f);
            winWindow.setSize(cameraSize);
            
            // Center the win text on the camera
            sf::FloatRect textBounds = winText.getLocalBounds();
            winText.setOrigin(textBounds.left + textBounds.width/2.f, textBounds.top + textBounds.height/2.f);
            winText.setPosition(cameraCenter.x, cameraCenter.y - 50.f);
            
            window.draw(winWindow);
            window.draw(winText);
            gameOver = true;

            
            sf::FloatRect winTextBounds = winText.getGlobalBounds();
            restartbutton.setPosition(cameraCenter.x - restartbutton.getSize().x / 2.f, 
            winTextBounds.top + winTextBounds.height + 50.f);
            
            restartbutton.setButtonText("Restart", 24, sf::Color::Black);
            restartbutton.isHover(window);
            window.draw(restartbutton);
            window.draw(restartbutton.getText());
        }
        if(!gameOver && !character.getIsDead())
        {
            character.getHealthInterface(window);
            coinsManager.getCoinInterface(window);
            Enemy::getKillInterface(window);
        }

        window.display();
    }

    return 0;
}