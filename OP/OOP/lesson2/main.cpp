#include <SFML/Graphics.hpp>
#include <iostream>





int main()
{
    // Create a window with dimensions 800x600 and a title
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Window");
    
    // Set frame rate limit to 60 FPS
    window.setFramerateLimit(60);
    

    // Main game loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window when the close button is clicked
            if (event.type == sf::Event::Closed)
                window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();
        }

        // Movement variables
        static sf::Vector2f position(400.f, 300.f); // Start in the center
        float speed = 5.f;

        // Handle keyboard input for movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            position.x += speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            position.x -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            position.y -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            position.y += speed;

        // Clear the window with black color
        window.clear(sf::Color::Black);

        // Draw a simple shape (e.g., a circle) at the position
        sf::CircleShape shape(30.f);
        shape.setFillColor(sf::Color::Green);
        shape.setPosition(position);
        window.draw(shape);
        
        // Display the window
        window.display();
    }
    
    return 0;
}