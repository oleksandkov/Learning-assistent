#include "Game.h"


// Private functions
void Game::initializeVariables()
{
    this->window = nullptr;
}

void Game::initWindow()
{
    this->videoMode.height = 600;
    this->videoMode.width = 800;

    this->window = new sf::RenderWindow(this->videoMode, "My first game (Tutorial)", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);

}

// Consturctions / Destructors
Game::Game()
{

    this->initializeVariables();
    this->initWindow();

}

Game::~Game()
{
    delete this->window;


}

// Accessors
const bool Game::getWindowIsOpen() const
{
    return this->window->isOpen();
}

// Functions
void Game::pollEvents()
{
    while(this->window->pollEvent(this->ev))
    {
        switch (this->ev.type)
        {
            case sf::Event::Closed:
                this->window->close();
                break;
            case sf::Event::KeyPressed:
                if (this->ev.key.code == sf::Keyboard::Escape)
                this->window->close();
                break;
        }
    }

}



void Game::update()
{
    this->pollEvents();

}

void Game::render()
{
    /*
    - return void
    - clear old frame
    - render objects
    - display frame in window
    */
    this->window->clear(sf::Color(255,0,0,5));

    //Draw game objects
    this->window->display();
}