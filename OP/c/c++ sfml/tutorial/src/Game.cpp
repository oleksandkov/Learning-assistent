#include "Game.h"
#include <sstream>

// Private functions
void Game::initEnemyFont()
{
    // Load a font for enemy text labels
    if(!this->enemyFont.loadFromFile("Fonts/GothicA1-Regular.ttf"))
    {
        std::cout << "ERROR::GAME::INITENEMYFONT::Failed to load enemy font!" << "\n";
    }
}

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

// Consturctions / Destructors
Game::Game()
{

    this->initializeVariables();
    this->initWindow();
    this->initFonts();
    this->initText();
    this->initEnemyFont(); 
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
void Game::initEnemies()
{
    this->enemy.shape.setPosition(10.f, 10.f);
    this->enemy.shape.setSize(sf::Vector2f(100.f, 100.f));
    this->enemy.shape.setFillColor(sf::Color::Cyan);
    this->enemy.isKeyboardEnemy = false;  // Default to mouse enemy
    this->enemy.pointValue = 1;
}

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
    // Position at random X, top of screen
    this->enemy.shape.setPosition(
        static_cast<float>(rand() % static_cast<int>(
            this->window->getSize().x - this->enemy.shape.getSize().x)),
        0.f
    );

    // Randomize enemy type (now 0-7 for more variety)
    int type = rand() % 8;
    
    switch(type)
    {   
        // MOUSE-BASED ENEMIES (original 5 types)
        case 0:
            this->enemy.shape.setFillColor(sf::Color::Magenta);
            this->enemy.shape.setSize(sf::Vector2f(30.f, 30.f));
            this->enemy.isKeyboardEnemy = false;
            this->enemy.pointValue = 10;
            break;
        case 1:
            this->enemy.shape.setFillColor(sf::Color::Blue);
            this->enemy.shape.setSize(sf::Vector2f(30.f, 30.f));
            this->enemy.isKeyboardEnemy = false;
            this->enemy.pointValue = 7;
            break;
        case 2:
            this->enemy.shape.setFillColor(sf::Color::Cyan);
            this->enemy.shape.setSize(sf::Vector2f(50.f, 50.f));
            this->enemy.isKeyboardEnemy = false;
            this->enemy.pointValue = 5;
            break;
        case 3:
            this->enemy.shape.setFillColor(sf::Color::Red);
            this->enemy.shape.setSize(sf::Vector2f(70.f, 70.f));
            this->enemy.isKeyboardEnemy = false;
            this->enemy.pointValue = 3;
            break;
        case 4:
            this->enemy.shape.setFillColor(sf::Color::Green);
            this->enemy.shape.setSize(sf::Vector2f(100.f, 100.f));
            this->enemy.isKeyboardEnemy = false;
            this->enemy.pointValue = 1;
            break;
            
        // KEYBOARD-BASED ENEMIES (new types 5-7)
        case 5:
            // Enemy requiring 'A' key - Yellow with black border
            this->enemy.shape.setFillColor(sf::Color::Yellow);
            this->enemy.shape.setSize(sf::Vector2f(60.f, 60.f));
            this->enemy.shape.setOutlineColor(sf::Color::Black);
            this->enemy.shape.setOutlineThickness(2.f);
            this->enemy.isKeyboardEnemy = true;
            this->enemy.requiredKey = sf::Keyboard::A;
            this->enemy.keyChar = 'A';
            this->enemy.pointValue = 15;
            break;
            
        case 6:
            // Enemy requiring 'S' key - White with black border
            this->enemy.shape.setFillColor(sf::Color::White);
            this->enemy.shape.setSize(sf::Vector2f(60.f, 60.f));
            this->enemy.shape.setOutlineColor(sf::Color::Black);
            this->enemy.shape.setOutlineThickness(2.f);
            this->enemy.isKeyboardEnemy = true;
            this->enemy.requiredKey = sf::Keyboard::S;
            this->enemy.keyChar = 'S';
            this->enemy.pointValue = 15;
            break;
            
        case 7:
            // Enemy requiring 'D' key - Orange (Red + Green)
            this->enemy.shape.setFillColor(sf::Color(255, 165, 0));  // Orange RGB
            this->enemy.shape.setSize(sf::Vector2f(60.f, 60.f));
            this->enemy.shape.setOutlineColor(sf::Color::Black);
            this->enemy.shape.setOutlineThickness(2.f);
            this->enemy.isKeyboardEnemy = true;
            this->enemy.requiredKey = sf::Keyboard::D;
            this->enemy.keyChar = 'D';
            this->enemy.pointValue = 15;
            break;
            
        default:
            this->enemy.shape.setFillColor(sf::Color::Yellow);
            this->enemy.shape.setSize(sf::Vector2f(100.f, 100.f));
            this->enemy.isKeyboardEnemy = false;
            this->enemy.pointValue = 1;
            break;
    }

    // Add to vector
    this->enemies.push_back(this->enemy);
}

void Game::updateEnemies()
{
    // Spawn timer logic (unchanged)
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
        // Move enemy down
        this->enemies[i].shape.move(0.f, 3.f);
        
        // Check if enemy fell off screen
        if(this->enemies[i].shape.getPosition().y > this->window->getSize().y)
        {
            this->enemies.erase(this->enemies.begin() + i);
            this->health -= 1;
            std::cout << "Health: " << this->health << "\n";
            continue;  // Skip to next iteration after erasing
        }
    }
    
    // SEPARATE LOOP: Check for destruction (mouse clicks)
    if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        if(this->mouseHeld == false)
        {
            this->mouseHeld = true;
            
            for(int i = 0; i < this->enemies.size(); i++)
            {
                // Only check MOUSE enemies
                if(!this->enemies[i].isKeyboardEnemy && 
                   this->enemies[i].shape.getGlobalBounds().contains(this->mousePosView))
                {
                    // Award points
                    this->points += this->enemies[i].pointValue;
                    std::cout << "Points: " << this->points << "\n";
                    
                    // Delete enemy
                    this->enemies.erase(this->enemies.begin() + i);
                    break;  // Only destroy one per click
                }
            }
        }
    }
    else 
    {
        this->mouseHeld = false;
    }
    
    // NEW: Check for destruction (keyboard presses)
    for(int i = 0; i < this->enemies.size(); i++)
    {
        if(this->enemies[i].isKeyboardEnemy)
        {
            // Check if the required key is pressed
            if(sf::Keyboard::isKeyPressed(this->enemies[i].requiredKey))
            {
                // Award points
                this->points += this->enemies[i].pointValue;
                std::cout << "Key Enemy Destroyed! Points: " << this->points << "\n";
                
                // Delete enemy
                this->enemies.erase(this->enemies.begin() + i);
                i--;  // Adjust index after erasing
                
                // Small delay to prevent multiple detections
                sf::sleep(sf::milliseconds(100));
            }
        }
    }
}

void Game::renderEnemies(sf::RenderTarget* target)
{
    // Render all enemy shapes
    for (auto &e : this->enemies)
    {
        target->draw(e.shape);
        
        // If it's a keyboard enemy, draw the required key on it
        if(e.isKeyboardEnemy == true)
        {
            sf::Text keyText;
            keyText.setFont(this->enemyFont);
            keyText.setString(std::string(1, e.keyChar));  // Convert char to string
            keyText.setCharacterSize(24);
            keyText.setFillColor(sf::Color::Black);
            
            // Center the text on the enemy
            sf::FloatRect textBounds = keyText.getLocalBounds();
            keyText.setOrigin(
                textBounds.left + textBounds.width / 2.0f,
                textBounds.top + textBounds.height / 2.0f
            );
            keyText.setPosition(
                e.shape.getPosition().x + e.shape.getSize().x / 2.0f,
                e.shape.getPosition().y + e.shape.getSize().y / 2.0f
            );
            
            target->draw(keyText);
        }
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