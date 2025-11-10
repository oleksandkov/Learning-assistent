#include "Game.h"
#include <sstream>

// Private functions
void Game::initializeVariables()
{
    this->window = nullptr;

    // Game Logic
    this->endGame = false;
    this->health = 25;
    this->points = 0;
    this->enemySpawnTimerMax = 20.f;
    this->enemySpawnTime = this->enemySpawnTimerMax;
    this->maxEnemies = 5;
    this->mouseHeld = false;
}

void Game::initWindow()
{
    this->videoMode.height = 600;
    this->videoMode.width = 800;

    this->window = new sf::RenderWindow(this->videoMode, "My first game (Tutorial)", sf::Style::Titlebar | sf::Style::Close);
    this->window->setFramerateLimit(60);
}
void Game::initFonts()
{
    if(this->font.loadFromFile("Fonts/GothicA1-Regular.ttf"))
    {
        std::cout << "ERROR::GAME::INIFONTS::Failed to load font!" << "\n";
    }
}
void Game::initText()
{
    this->uiText.setFont(this->font);
    this->uiText.setCharacterSize(24);
    this->uiText.setFillColor(sf::Color::White);
    this->uiText.setString("NONE");
}

void Game::initEnemies()
{
    this->enemy.setPosition(10.f, 10.f);
    this->enemy.setSize(sf::Vector2f(100.f, 100.f));
    // this->enemy.setScale(sf::Vector2f(0.5f, 0.5f));
    this->enemy.setFillColor(sf::Color::Cyan);
    // this->enemy.setOutlineColor(sf::Color::Green);
    // this->enemy.setOutlineThickness(1.f);
}

// Consturctions / Destructors
Game::Game()
{

    this->initializeVariables();
    this->initWindow();
    this->initFonts();
    this->initText();
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

const bool Game::getEndGame() const
{
    return this->endGame;
}

// Functions

void Game::updateText()
{
    std::stringstream ss;

    ss << "Points: " << this->points << "\n"
        << "Health: " << this->health << '\n';

        this->uiText.setString(ss.str());
}


void Game::renderText(sf::RenderTarget* target)
{
    target->draw(this->uiText);

}
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

    //Randomize enemy type
    int type = rand() % 5;
    switch(type)
    {   
        case 0:
            this->enemy.setFillColor(sf::Color::Magenta);
            this->enemy.setSize(sf::Vector2f(10.f, 10.f));
            break;
        case 1:
            this->enemy.setFillColor(sf::Color::Blue);
            this->enemy.setSize(sf::Vector2f(30.f, 30.f));
            break;
        case 2:
            this->enemy.setFillColor(sf::Color::Cyan);
            this->enemy.setSize(sf::Vector2f(50.f, 50.f));
            break;
        case 3:
            this->enemy.setFillColor(sf::Color::Red);
            this->enemy.setSize(sf::Vector2f(70.f, 70.f));
            break;
        case 4:
            this->enemy.setFillColor(sf::Color::Green);
            this->enemy.setSize(sf::Vector2f(100.f, 100.f));
            break;
            
        default:
         this->enemy.setFillColor(sf::Color::Yellow);
            this->enemy.setSize(sf::Vector2f(100.f, 100.f));
            break;
    }



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
        
        // IF the enemy is past the bottom of the screen
        if(this->enemies[i].getPosition().y > this->window->getSize().y)
        {
            this->enemies.erase(this->enemies.begin() + i);
            this->health -= 1;
            std::cout << "Health: " << this->health << "\n";
        }
        
        // Check if clicked upon
        
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            if(this->mouseHeld == false)
            {
                this->mouseHeld = true;
                bool deleted = false;
                for(size_t i = 0; i < this->enemies.size() && deleted == false; i++)
                {
                    if (this->enemies[i].getGlobalBounds().contains(this->mousePosView))
                    {
                        
                    // Gain points for killings
                    if(this->enemies[i].getFillColor() == sf::Color::Magenta)
                        this->points += 10;
                    else if(this->enemies[i].getFillColor() == sf::Color::Blue)
                        this->points += 7.f;
                    else if(this->enemies[i].getFillColor() == sf::Color::Cyan)
                        this->points += 5.f;
                    else if(this->enemies[i].getFillColor() == sf::Color::Red)
                        this->points += 3.f;
                    else if(this->enemies[i].getFillColor() == sf::Color::Green)
                        this->points += 1.f;
                    std::cout << "Points: " << this->points << "\n";

                    //Delete the enemy
                        deleted = true;
                        
                    this->enemies.erase(this->enemies.begin() + i);
                    }
                
                }
            }
        }
        else 
        {
            this->mouseHeld = false;
        }
        
        
    } 
}

void Game::renderEnemies(sf::RenderTarget* target)
{
    // REndering enemies
    for (auto &e : this->enemies)
    {
       target->draw(e);
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
    if(this->endGame == false)
    {

        
        this->updateMousePositions();
        this->updateText();
        
        this->updateEnemies();
    }
    if(this->health <= 0)
    {
        this->endGame = true;

    }
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
    this->renderEnemies(this->window);
    this->renderText(this->window);
    this->window->display();
}