#include <iostream>
#include <cmath>
#include <vector>
#include <ctime>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include "Game.cpp"

class Button : public sf::RectangleShape
{
private:
    sf::RectangleShape button;
    sf::RectangleShape blackBackgound;
    sf::Text buttonText;

public:
    Button()
    {
        setSize(sf::Vector2f(200.f, 100.f));
        setFillColor(sf::Color(200, 200, 200, 200));
        setOutlineThickness(2.f);
        setOutlineColor(sf::Color::Black);
    };

    void isHover()
    {
        if (getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y)))
        {
            setFillColor(sf::Color(255, 255, 255, 255));
        }
        else
        {
            setFillColor(sf::Color(200, 200, 200, 200));
        }
    }

    bool isClicked(sf::Event event)
    {
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y)))
            {
                return true;
            }
        }
        return false;
    }
};

class Square : public sf::RectangleShape
{
private:
    sf::Vector2f newPos;
    sf::Vector2f currentPos;
    sf::Vector2f oldPos;
    bool hasReachedTarget;
    std::vector<Square *> others;

public:
    static long long int points;

    Square()
    {
        setSize(sf::Vector2f(100.f, 100.f));
        setFillColor(sf::Color::White);
        setOutlineThickness(1.f);
        setOutlineColor(sf::Color::Black);
        hasReachedTarget = true;
    };
    // Teleport
    void teleport(float x, float y)
    {
        setPosition(x, y);
    }
    // Points logic
    void getPoints(Square *object)
    {
        sf::FloatRect thisBounds = getGlobalBounds();
        sf::FloatRect objectBounds = object->getGlobalBounds();

        if (thisBounds.intersects(objectBounds))
        {
            points += 1;
        }
    }

    long long int showPoints()
    {
        return points;
    }

    void randomPlacement(float windowWidth = 800.f, float windowHeight = 400.f, float change_time = 2.f)
    {
        static sf::Clock timer;
        if (timer.getElapsedTime().asSeconds() > change_time)
        {

            srand(time(NULL));
            float rand_x = static_cast<float>(rand() % static_cast<int>(windowWidth - 100.f));
            float rand_y = static_cast<float>(rand() % static_cast<int>(windowHeight - 100.f));
            setPosition(sf::Vector2f(rand_x, rand_y));
            timer.restart();
        }
    }

    // Lose condition functions
    bool loseCondition(Square *other)
    {
        if (getGlobalBounds().intersects(other->getGlobalBounds()))
        {
            return true; // Return true when collision detected
        }
        return false;
    }

    void addToCollisionList(Square *other)
    {
        others.push_back(other);
    }

    bool globalCollisionCheck()
    {
        for (size_t i = 0; i < others.size(); i++)
        {
            if (collidesWith(*others[i]))
            {
                return true;
            }
        }
        return false;
    }

    void moveSquareDynamic(float speed = 5.f, float windowWidth = 800.f, float windowHeight = 600.f)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            oldPos = getPosition();
            move(0.f, -speed);
            if (globalCollisionCheck() || isOutsideWindow(windowWidth, windowHeight))
                setPosition(oldPos);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            oldPos = getPosition();
            move(0.f, speed);
            if (globalCollisionCheck() || isOutsideWindow(windowWidth, windowHeight))
                setPosition(oldPos);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            oldPos = getPosition();
            move(-speed, 0.f);
            if (globalCollisionCheck() || isOutsideWindow(windowWidth, windowHeight))
                setPosition(oldPos);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            oldPos = getPosition();
            move(speed, 0.f);
            if (globalCollisionCheck() || isOutsideWindow(windowWidth, windowHeight))
                setPosition(oldPos);
        }
    }

    void setNewPos(float x, float y)
    {
        newPos = sf::Vector2f(x, y);
        hasReachedTarget = false;
    }

    bool isAtTarget()
    {
        return hasReachedTarget;
    }

    // Simple collision check
    bool collidesWith(const Square &other)
    {
        return getGlobalBounds().intersects(other.getGlobalBounds());
    }

    // Window boundary collision check
    bool isOutsideWindow(float windowWidth, float windowHeight)
    {
        sf::Vector2f pos = getPosition();
        sf::Vector2f size = getSize();

        return (pos.x < 0 || pos.x + size.x > windowWidth ||
                pos.y < 0 || pos.y + size.y > windowHeight);
    }

    // Functions
    void moveSquare(float speed = 2.f)
    {
        sf::Vector2f currentPos = getPosition();
        sf::Vector2f direction = newPos - currentPos;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (distance > speed)
        {
            direction /= distance;
            sf::Vector2f nextPos = currentPos + sf::Vector2f(direction.x * speed, direction.y * speed);

            sf::Vector2f oldPos = getPosition();
            setPosition(nextPos);

            if (globalCollisionCheck())
            {
                setPosition(oldPos);
            }
        }
        else if (distance > 0)
        {

            sf::Vector2f oldPos = getPosition();
            setPosition(newPos);

            if (globalCollisionCheck())
            {
                setPosition(oldPos);
            }
            else
            {
                hasReachedTarget = true;
            }
        }
    }
};

long long int Square::points = 0;

int main()
{
    // Window
    sf::RenderWindow window(sf::VideoMode(800, 600), "The Game", sf::Style::Default);
    window.setFramerateLimit(60);

    // Game logic global
    bool gameOver = false;

    // Font and text setup
    sf::Font font;
    if (!font.loadFromFile("GothicA1-Regular.ttf"))
    {
        std::cout << "Error loading font!" << std::endl;
    }

    sf::Text pointsText;
    pointsText.setFont(font);
    pointsText.setCharacterSize(24);
    pointsText.setFillColor(sf::Color::White);
    pointsText.setPosition(10.f, 10.f);

    // Game time
    sf::Clock gameTimer;
    sf::Text timerText;
    timerText.setFont(font);
    timerText.setCharacterSize(24);
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition(10.f, 50.f);

    // Objects
    Square x;
    x.setPosition(sf::Vector2f(100.f, 100.f));

    Square y;
    y.setPosition(sf::Vector2f(400.f, 100.f));
    y.setScale(sf::Vector2f(0.5f, 0.5f));

    Square z;
    z.setPosition(sf::Vector2f(-50.f, 200.f));
    z.setScale(sf::Vector2f(0.6f, 0.6f));
    Square z_1;
    z_1.setPosition(sf::Vector2f(-50.f, 200.f));
    z_1.setScale(sf::Vector2f(0.5f, 0.5f));

    Square z_2;
    z_2.setPosition(sf::Vector2f(-50.f, 200.f));
    z_2.setScale(sf::Vector2f(0.5f, 0.5f));

    Square floor;
    floor.setPosition(sf::Vector2f(0.f, 500.f));
    floor.setFillColor(sf::Color::Magenta);
    floor.setSize(sf::Vector2f(800.f, 100.f));

    Square pointArea;
    pointArea.setSize(sf::Vector2f(35.f, 35.f));
    pointArea.setFillColor(sf::Color(0, 255, 0, 100));
    pointArea.setPosition(sf::Vector2f(300.f, 300.f));

    // Command sequence state
    int currentCommand_y = 0;
    int currentCommand_x = 0;
    int currentCommand_z = 0;
    int currentCommand_z1 = 0;
    int currentCommand_z2 = 0;
    // Add to collision lists
    // x.addToCollisionList(&y);
    // x.addToCollisionList(&floor);
    // y.addToCollisionList(&x);
    y.addToCollisionList(&floor);
    floor.addToCollisionList(&x);
    floor.addToCollisionList(&y);

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
        // PointArea
        pointArea.randomPlacement(800.f, 400.f, 5.f);
        // Move x
        if (currentCommand_x == 0)
        {
            x.setNewPos(100.f, 400.f);
            x.moveSquare();
            if (x.isAtTarget())
            {
                currentCommand_x = 1;
            }
        }
        else if (currentCommand_x == 1)
        {
            x.setNewPos(400.f, 400.f);
            x.moveSquare();
            if (x.isAtTarget())
            {
                currentCommand_x = 2;
            }
        }
        else if (currentCommand_x == 2)
        {
            x.setNewPos(400.f, 100.f);
            x.moveSquare();
            if (x.isAtTarget())
            {
                currentCommand_x = 3;
            }
        }
        else if (currentCommand_x == 3)
        {
            x.setNewPos(100.f, 100.f);
            x.moveSquare();
            if (x.isAtTarget())
            {
                currentCommand_x = 0;
            }
        }
        // Move z
        //      if(currentCommand_z == 0)
        //      {
        //      z.setNewPos(870.f, 200.f);
        //      z.moveSquare(4.f);
        //      if (z.isAtTarget()) currentCommand_z = 1;
        //  } else if (currentCommand_z == 1)
        //  {
        //      z.setNewPos(-60.f, 200.f);
        //      z.moveSquare(4.f);
        //      if (z.isAtTarget())  currentCommand_z = 0;
        //  }

        static sf::Clock pauseTimer_z;
        static bool isPausing = false;

        if (currentCommand_z == 0 && !isPausing)
        {
            z.setNewPos(870.f, 200.f);
            z.moveSquare(15.f);
            if (z.isAtTarget())
            {
                isPausing = true;
                pauseTimer_z.restart();
            }
        }
        else if (currentCommand_z == 0 && isPausing)
        {

            if (pauseTimer_z.getElapsedTime().asSeconds() > 2.0f)
            {
                currentCommand_z = 1;
                isPausing = false;
            }
        }
        else if (currentCommand_z == 1 && !isPausing)
        {
            z.setNewPos(-60.f, 200.f);
            z.moveSquare(15.f);
            if (z.isAtTarget())
            {
                isPausing = true;
                pauseTimer_z.restart();
            }
        }
        else if (currentCommand_z == 1 && isPausing)
        {

            if (pauseTimer_z.getElapsedTime().asSeconds() > 2.0f)
            {
                currentCommand_z = 0;
                isPausing = false;
            }
        }
        // Move z1
        static sf::Clock pauseTimer_z1;
        static bool isPausing_z1 = false;
        static int pathChoice = 0;
        static bool pathChosen = false;

        if (!pathChosen)
        {
            srand(time(NULL));
            pathChoice = rand() % 2;
            pathChosen = true;
        }

        if (pathChoice == 0)
        {
            if (currentCommand_z1 == 0 && !isPausing_z1)
            {
                z_1.setNewPos(850.f, 460.f);
                z_1.moveSquare(10.f);
                if (z_1.isAtTarget())
                {
                    isPausing_z1 = true;
                    pauseTimer_z1.restart();
                }
            }
            else if (currentCommand_z1 == 0 && isPausing_z1)
            {
                if (pauseTimer_z1.getElapsedTime().asSeconds() > 3.0f)
                {
                    currentCommand_z1 = 1;
                    isPausing_z1 = false;
                }
            }
            else if (currentCommand_z1 == 1 && !isPausing_z1)
            {
                z_1.setNewPos(-80.f, -20.f);
                z_1.moveSquare(10.f);
                if (z_1.isAtTarget())
                {
                    isPausing_z1 = true;
                    pauseTimer_z1.restart();
                }
            }
            else if (currentCommand_z1 == 1 && isPausing_z1)
            {
                if (pauseTimer_z1.getElapsedTime().asSeconds() > 3.0f)
                {
                    currentCommand_z1 = 0;
                    isPausing_z1 = false;
                    pathChosen = false; // Allow new path selection
                }
            }
        }
        else if (pathChoice == 1)
        {
            if (currentCommand_z1 == 0 && !isPausing_z1)
            {
                z_1.setNewPos(870.f, 30.f);
                z_1.moveSquare(10.f);
                if (z_1.isAtTarget())
                {
                    isPausing_z1 = true;
                    pauseTimer_z1.restart();
                }
            }
            else if (currentCommand_z1 == 0 && isPausing_z1)
            {
                if (pauseTimer_z1.getElapsedTime().asSeconds() > 3.0f)
                {
                    currentCommand_z1 = 1;
                    isPausing_z1 = false;
                }
            }
            else if (currentCommand_z1 == 1 && !isPausing_z1)
            {
                z_1.setNewPos(-80.f, 400.f);
                z_1.moveSquare(10.f);
                if (z_1.isAtTarget())
                {
                    isPausing_z1 = true;
                    pauseTimer_z1.restart();
                }
            }
            else if (currentCommand_z1 == 1 && isPausing_z1)
            {
                if (pauseTimer_z1.getElapsedTime().asSeconds() > 3.0f)
                {
                    currentCommand_z1 = 0;
                    isPausing_z1 = false;
                    pathChosen = false; // Allow new path selection
                }
            }
        }

        static sf::Clock pauseTimer_z2;
        static bool isPausing_z2 = false;
        static float randy1 = 0.0f;
        static float randy2 = 0.0f;
        static bool initialized_z2 = false;

        if (!initialized_z2)
        {
            srand(time(NULL));
            randy1 = static_cast<float>(rand() % 500);
            randy2 = static_cast<float>(rand() % 500);
            z_2.teleport(-80.f, randy1);
            initialized_z2 = true;
        }

        if (initialized_z2)
        {
            if (currentCommand_z2 == 0 && !isPausing_z2)
            {
                z_2.setNewPos(870.f, randy1);
                z_2.moveSquare(12.f);
                if (z_2.isAtTarget())
                {
                    isPausing_z2 = true;
                    pauseTimer_z2.restart();
                }
            }
            else if (currentCommand_z2 == 0 && isPausing_z2)
            {
                if (pauseTimer_z2.getElapsedTime().asSeconds() > 2.0f)
                {
                    currentCommand_z2 = 1;
                    isPausing_z2 = false;
                }
            }
            else if (currentCommand_z2 == 1 && !isPausing_z2)
            {
                z_2.setNewPos(-80.f, randy2);
                z_2.moveSquare(12.f);
                if (z_2.isAtTarget())
                {
                    isPausing_z2 = true;
                    pauseTimer_z2.restart();
                }
            }
            else if (currentCommand_z2 == 1 && isPausing_z2)
            {
                if (pauseTimer_z2.getElapsedTime().asSeconds() > 2.0f)
                {
                    currentCommand_z2 = 0;
                    isPausing_z2 = false;
                }
            }
        }

        // Points - track and display when changed
        static long long int oldPoints = 0;
        y.getPoints(&pointArea);
        if (Square::points != oldPoints)
        {
            std::cout << "Points: " << Square::points << std::endl;
            oldPoints = Square::points;
        }
        // Update points text
        pointsText.setString("Points: " + std::to_string(Square::points));

        // Update timer text
        float currentTime = gameTimer.getElapsedTime().asSeconds();
        timerText.setString("Time: " + std::to_string((int)currentTime) + "s");

        // Lose condition check

        if (((y.loseCondition(&x) || y.loseCondition(&z) || y.loseCondition(&z_1) || y.loseCondition(&z_2)) && gameTimer.getElapsedTime().asSeconds() > 2.) && gameTimer.getElapsedTime().asSeconds() > 60.f)
        {
            std::cout << "Game Over! Final Points: " << Square::points << std::endl;
        }

        // Test collision
        y.moveSquareDynamic(5.f, 800.f, 600.f);
        if (gameTimer.getElapsedTime().asSeconds() < 2.f)
        {
            y.setFillColor(sf::Color(255, 255, 0, 128));
        }
        else
        {
            y.setFillColor(sf::Color::Yellow);
        }

        // ending
        window.clear(sf::Color::Blue);

        window.draw(x);
        window.draw(y);
        window.draw(z);
        window.draw(z_1);
        window.draw(z_2);
        window.draw(pointsText);
        window.draw(timerText);
        window.draw(floor);
        window.draw(pointArea);

        window.display();
    }

    return 0;
}

// --- Code Explanation ---
// File extension: .cpp
// Total lines: 226
// Comment lines: 9
// Empty lines: 36
// Functions detected: 8
// This file contains code in .cpp. Add more details as needed.
