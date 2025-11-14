#include <iostream>
#include <cmath>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include "Game.cpp"

class Square : public sf::RectangleShape
{
private:
    sf::Vector2f newPos;
    sf::Vector2f currentPos;
    sf::Vector2f oldPos;
    bool hasReachedTarget;
    std::vector<Square*> others;
    
    public:
    Square()
    {
        setSize(sf::Vector2f(100.f, 100.f));
        setFillColor(sf::Color::White);
        setOutlineThickness(1.f);
        setOutlineColor(sf::Color::Black);
        hasReachedTarget = true;
    };
    
    
    

    void addToCollisionList(Square* other)
    {
        others.push_back(other);
    }

    bool globalCollisionCheck()
    {
        for (size_t i = 0; i < others.size(); i++)
        {
            if(collidesWith(*others[i]))
            {
                return true;
            }
        }
        return false;
    }

    void moveSquareDynamic(float speed = 5.f)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            oldPos = getPosition();
            move(0.f, -speed);
            if(globalCollisionCheck())
                setPosition(oldPos);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            oldPos = getPosition();
            move(0.f, speed);
            if(globalCollisionCheck())
                setPosition(oldPos);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            oldPos = getPosition();
            move(-speed, 0.f);
            if(globalCollisionCheck())
                setPosition(oldPos);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            oldPos = getPosition();
            move(speed, 0.f);
            if(globalCollisionCheck())
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

int main()
{
    // Window
    sf::RenderWindow window(sf::VideoMode(800, 600), "The Game", sf::Style::Default);
    window.setFramerateLimit(60);
    // Objects
    Square x;
    x.setPosition(sf::Vector2f(100.f, 100.f));

    Square y;
    y.setPosition(sf::Vector2f(400.f, 100.f));

    // Command sequence state
    int currentCommand_y = 0;
    int currentCommand_x = 0;

    // Add to collision lists
    x.addToCollisionList(&y);
    y.addToCollisionList(&x);

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

        //Test collision
        y.moveSquareDynamic();                      
        
        //ending
        window.clear(sf::Color::Blue);

        window.draw(x);
        window.draw(y);

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
