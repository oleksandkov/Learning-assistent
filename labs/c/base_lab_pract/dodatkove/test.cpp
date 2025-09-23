#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFML Graphics Test");
    
    // Create a circle
    sf::CircleShape circle(50);
    circle.setFillColor(sf::Color::Red);
    circle.setPosition(100, 100);
    
    // Create a rectangle
    sf::RectangleShape rectangle(sf::Vector2f(120, 80));
    rectangle.setFillColor(sf::Color::Green);
    rectangle.setPosition(300, 200);
    
    // Create another circle with outline
    sf::CircleShape blueCircle(40);
    blueCircle.setFillColor(sf::Color::Blue);
    blueCircle.setOutlineThickness(5);
    blueCircle.setOutlineColor(sf::Color::Yellow);
    blueCircle.setPosition(500, 300);
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        // Clear screen with black color
        window.clear(sf::Color::Black);
        
        // Draw all shapes
        window.draw(circle);
        window.draw(rectangle);
        window.draw(blueCircle);
        
        // Display everything
        window.display();
    }
    return 0;
}