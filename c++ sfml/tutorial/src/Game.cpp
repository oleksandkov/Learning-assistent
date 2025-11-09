#include "Game.h"

// Private functions
void Game::initializeVariables()
{
    this->window = nullptr;

    // Game Logic
    this->points = 0;
    this->enemySpawnTimerMax = 10.f;
    this->enemySpawnTime = this->enemySpawnTimerMax;
    this->maxEnemies = 10;
}

void Game::initWindow()
{
    this->videoMode.height = 600;
    this->videoMode.width = 800;

    this->window = new sf::RenderWindow(this->videoMode, "My first game (Tutorial)", sf::Style::Titlebar | sf::Style::Close);
    this->window->setFramerateLimit(60);
}

void Game::initEnemies()
{
    this->enemy.setPosition(10.f, 10.f);
    this->enemy.setSize(sf::Vector2f(100.f, 100.f));
    this->enemy.setScale(sf::Vector2f(0.5f, 0.5f));
    this->enemy.setFillColor(sf::Color::Cyan);
    // this->enemy.setOutlineColor(sf::Color::Green);
    // this->enemy.setOutlineThickness(1.f);
}

// Consturctions / Destructors
Game::Game()
{

    this->initializeVariables();
    this->initWindow();
    this->initEnemies();
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

void Game::spawnEnemy()
{
    /*
    spawns enemies and sets theier colors and positions
    - sets the random postions
    - sets a random color
    - adds enemy to hte vector
    */
    this->enemy.setPosition(
        static_cast<float>(rand() % static_cast<int>(this->window->getSize().x - this->enemy.getSize().x)),
        0.f
    );
    this->enemy.setFillColor(sf::Color::Green);

    // Spawn enemy
    this->enemies.push_back(this->enemy);

    //Remove enemies at end of screen


}

void Game::updateEnemies()
{
    // Updateing the timer for enemy spawning
    if (this->enemies.size() < this->maxEnemies)
    {

        if (this->enemySpawnTime >= this->enemySpawnTimerMax)
        {
            this->spawnEnemy();
            this->enemySpawnTime = 0.f;
        }
        else
        {
            this->enemySpawnTime += 1.f;
        }
    }

    // Moving and updating enemies
    for (int i = 0; i < this->enemies.size(); i++)
    {
        bool deleted = false;
        this->enemies[i].move(0.f, 3.f);
        
        // Check if clicked upon
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            if (this->enemies[i].getGlobalBounds().contains(this->mousePosView))
            {
                deleted = true;

                // Gain points for killings
                this->points += 10.f;
            }
        }
        // IF the enemy is past the bottom of the screen
        if(this->enemies[i].getPosition().y > this->window->getSize().y)
        {
            deleted = true;
        }
        // Final delete
        if (deleted) this->enemies.erase(this->enemies.begin() + i);
    } 
}

void Game::renderEnemies()
{
    // REndering enemies
    for (auto &e : this->enemies)
    {
        this->window->draw(e);
    }
}

void Game::pollEvents()
{
    while (this->window->pollEvent(this->ev))
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

void Game::updateMousePositions()
{
    /*
    Updates the mouse postions
    * Mouse postions relateive to window (Vector2i)
    */
    this->mousePosWindow = sf::Mouse::getPosition(*this->window);
    this->mousePosView = this->window->mapPixelToCoords(this->mousePosWindow);
}

void Game::update()
{
    this->pollEvents();

    this->updateMousePositions();

    this->updateEnemies();
}

void Game::render()
{
    /*
    - return void
    - clear old frame
    - render objects
    - display frame in window
    */
    this->window->clear();

    // Draw game objects
    this->renderEnemies();
    this->window->display();
}