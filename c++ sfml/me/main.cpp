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
    
    
    

   
    bool collidesWithAny() {
        for (Square* other : others) {
            if (other != this && collidesWith(*other)) {
                return true;
            }
        }
        return false;
    }

    void moveSquareDynamic(const Square &other, float speed = 5.f)
    {
        oldPos =  getPosition();
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        move(0.f, -speed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            move(0.f, speed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            move(-speed, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            move(speed, 0.f);

            if(collidesWith(other))
                setPosition(oldPos);
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

    // void moveSquare(float speed = 2.f)
    // {
    //     sf::Vector2f currentPos = getPosition();
    //     sf::Vector2f direction = newPos - currentPos;
    //     float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    //     if (distance > speed)
    //     {
    //         direction /= distance;
    //         move(direction.x * speed, direction.y * speed);
    //     }
    //     else if (distance > 0)
    //     {
    //         setPosition(newPos);
    //         hasReachedTarget = true;
    //     }
    // }

    // Funcitons
    void moveSquare(const Square &other, float speed = 2.f)
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

            if (collidesWith(other))
            {
                setPosition(oldPos);
            }
            
        }
        else if (distance > 0)
        {
            
            sf::Vector2f oldPos = getPosition();
            setPosition(newPos); 

            if (collidesWith(other)) 
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

        // Update - Sequential command execution
        // Move y
        // if (currentCommand_y == 0)
        // {
        //     if (!y.isAtTarget())
        //     {
        //         y.moveSquare();
        //     }
        //     else
        //     {
        //         y.setNewPos(400.f, 400.f);
        //         currentCommand_y = 1;
        //     }
        // }
        // else if (currentCommand_y == 1)
        // {
        //     if (!y.isAtTarget())
        //     {
        //         y.moveSquare();
        //     }
        //     else
        //     {
        //         y.setNewPos(100.f, 100.f);
        //         currentCommand_y = 0;
        //     }
        // }

        // Move x 
        if (currentCommand_x == 0)
        {
            x.setNewPos(100.f, 400.f);   
            x.moveSquare(y); 
            if (x.isAtTarget())
            {
                currentCommand_x = 1;
            }
        }
        else if (currentCommand_x == 1)
        {
            x.setNewPos(400.f, 400.f);    
            x.moveSquare(y); 
            if (x.isAtTarget())
            {
                currentCommand_x = 2;
            }
        }
        else if (currentCommand_x == 2)
        {
            x.setNewPos(400.f, 100.f);    
            x.moveSquare(y); 
            if (x.isAtTarget())
            {
                currentCommand_x = 3;
            }
        }
        else if (currentCommand_x == 3)
        {
            x.setNewPos(100.f, 100.f);    
            x.moveSquare(y); 
            if (x.isAtTarget())
            {
                currentCommand_x = 0; 
            }
        }

        //Test collision
        y.moveSquareDynamic(x);                      
        
        //ending
        window.clear(sf::Color::Blue);

        window.draw(x);
        window.draw(y);

        window.display();
    }

    return 0;
}