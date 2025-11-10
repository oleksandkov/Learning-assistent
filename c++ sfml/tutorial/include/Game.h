
#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <vector>
#include <ctime>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

/*
    Class that acts as the game engine
    Wrapper class. Where all functions
*/

class Game
{
private:
    // Varibles
    // Window
    sf::RenderWindow *window;
    sf::VideoMode videoMode;
    sf::Event ev;

    // Mouse positions
    sf::Vector2i mousePosWindow;
    sf::Vector2f mousePosView;


    //Resources
    sf::Font font;

    //Text
    sf::Text uiText;

    // Game logic
    bool endGame;
    int health;
    unsigned points;
    float enemySpawnTime;
    float enemySpawnTimerMax;
    int maxEnemies;
    bool mouseHeld;
    // Game objects
    std::vector<sf::RectangleShape> enemies;
    sf::RectangleShape enemy;

    // Prive functions
    void initText();
    void initFonts();
    void initializeVariables();
    void initWindow();
    void initEnemies();

public:
    // Constructors /Destructors
    Game();
    virtual ~Game();
    // Accessors
    const bool getWindowIsOpen() const;
    const bool getEndGame() const;

    // Functions
    void renderText(sf::RenderTarget* target);
    void updateText();
    void spawnEnemy();
    void updateMousePositions();
    void pollEvents();
    void update();
    void updateEnemies();
    void render();
    void renderEnemies();
};

#endif