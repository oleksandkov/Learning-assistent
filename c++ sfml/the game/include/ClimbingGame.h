#ifndef CLIMBING_GAME_H
#define CLIMBING_GAME_H

#include <SFML/Graphics.hpp>
#include <sstream>
#include <iomanip>

enum class HandControl {
    LEFT,
    RIGHT
};

struct Hand {
    sf::CircleShape shape;
    sf::Vector2f position;
    bool isGrabbing;
    
    Hand() : isGrabbing(false) {
        shape.setRadius(8.f);
        shape.setOrigin(8.f, 8.f);
    }
};

class ClimbingGame {
private:
    // Window and view
    sf::RenderWindow window;
    sf::View camera;
    
    // Character parts
    Hand leftHand;
    Hand rightHand;
    sf::RectangleShape body;
    sf::CircleShape head;
    sf::CircleShape leftFoot;
    sf::CircleShape rightFoot;
    
    // Control state
    HandControl activeHand;
    float bodySwingOffset;
    
    // Physics
    sf::Vector2f bodyVelocity;
    float startHeight;
    const float gravity = 400.f;
    const float maxArmLength = 60.f;
    const float swingSpeed = 100.f;
    
    // Wall
    sf::RectangleShape wall;
    std::vector<sf::RectangleShape> wallGrips;
    
    // UI
    sf::Font font;
    sf::Text heightText;
    sf::Text controlText;
    
    // Helper methods
    void initializeCharacter(float startX, float startY);
    void initializeWall();
    void initializeUI();
    
    void handleEvents();
    void handleInput(float deltaTime);
    void updatePhysics(float deltaTime);
    void updateCamera();
    void render();
    
    void moveActiveHand(float dx, float dy);
    void toggleGrab();
    void switchActiveHand();
    
    bool isWithinArmReach(const sf::Vector2f& handPos, const sf::Vector2f& shoulder);
    sf::Vector2f getLeftShoulder();
    sf::Vector2f getRightShoulder();
    float getHeightClimbed();
    
public:
    ClimbingGame();
    void run();
};

#endif
