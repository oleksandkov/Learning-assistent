
#ifndef GAME_H
#define GAME_H

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
    //Varibles
    //Window
    sf::RenderWindow* window;
    sf::VideoMode videoMode;
    sf::Event ev;

    //Prive functions
    void initializeVariables();
    void initWindow();

public:
    // Constructors /Destructors
    Game();
    virtual ~Game();
    //Accessors
    const bool getWindowIsOpen() const;

    // Functions
    void pollEvents();
    void update();
    void render();
};

#endif