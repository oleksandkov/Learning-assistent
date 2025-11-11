#include "ClimbingGame.h"
#include <cmath>

ClimbingGame::ClimbingGame() 
    : window(sf::VideoMode(800, 600), "Climbing Game - Advanced Controls"),
      camera(sf::FloatRect(0.f, 0.f, 800.f, 600.f)),
      activeHand(HandControl::LEFT),
      bodySwingOffset(0.f) {
    
    window.setFramerateLimit(60);
    
    initializeCharacter(400.f, 500.f);
    initializeWall();
    initializeUI();
}

void ClimbingGame::initializeCharacter(float startX, float startY) {
    startHeight = startY;
    
    // Left hand (red)
    leftHand.position = sf::Vector2f(startX - 30.f, startY);
    leftHand.shape.setPosition(leftHand.position);
    leftHand.shape.setFillColor(sf::Color::Red);
    
    // Right hand (blue)
    rightHand.position = sf::Vector2f(startX + 30.f, startY);
    rightHand.shape.setPosition(rightHand.position);
    rightHand.shape.setFillColor(sf::Color::Blue);
    
    // Body
    body.setSize(sf::Vector2f(20.f, 40.f));
    body.setFillColor(sf::Color::Green);
    body.setOrigin(10.f, 0.f);
    body.setPosition(startX, startY + 50.f);
    
    // Head
    head.setRadius(12.f);
    head.setFillColor(sf::Color(255, 200, 150));
    head.setOrigin(12.f, 12.f);
    head.setPosition(startX, startY + 30.f);
    
    // Feet
    leftFoot.setRadius(6.f);
    leftFoot.setOrigin(6.f, 6.f);
    leftFoot.setFillColor(sf::Color(139, 69, 19));
    
    rightFoot.setRadius(6.f);
    rightFoot.setOrigin(6.f, 6.f);
    rightFoot.setFillColor(sf::Color(139, 69, 19));
    
    bodyVelocity = sf::Vector2f(0.f, 0.f);
}

void ClimbingGame::initializeWall() {
    wall.setSize(sf::Vector2f(200.f, 5000.f));
    wall.setPosition(300.f, -4400.f);
    wall.setFillColor(sf::Color(100, 100, 100));
    wall.setOutlineThickness(2.f);
    wall.setOutlineColor(sf::Color(70, 70, 70));
    
    for (float y = -4400.f; y < 600.f; y += 50.f) {
        sf::RectangleShape grip;
        grip.setSize(sf::Vector2f(200.f, 3.f));
        grip.setPosition(300.f, y);
        grip.setFillColor(sf::Color(80, 80, 80));
        wallGrips.push_back(grip);
    }
}

void ClimbingGame::initializeUI() {
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        font.loadFromFile("C:/Windows/Fonts/calibri.ttf");
    }
    
    heightText.setFont(font);
    heightText.setCharacterSize(24);
    heightText.setFillColor(sf::Color::White);
    heightText.setOutlineColor(sf::Color::Black);
    heightText.setOutlineThickness(2.f);
    
    controlText.setFont(font);
    controlText.setCharacterSize(20);
    controlText.setFillColor(sf::Color::Yellow);
    controlText.setOutlineColor(sf::Color::Black);
    controlText.setOutlineThickness(2.f);
}

sf::Vector2f ClimbingGame::getLeftShoulder() {
    return body.getPosition() + sf::Vector2f(-10.f, 5.f);
}

sf::Vector2f ClimbingGame::getRightShoulder() {
    return body.getPosition() + sf::Vector2f(10.f, 5.f);
}

bool ClimbingGame::isWithinArmReach(const sf::Vector2f& handPos, const sf::Vector2f& shoulder) {
    float dx = handPos.x - shoulder.x;
    float dy = handPos.y - shoulder.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    return distance <= maxArmLength;
}

float ClimbingGame::getHeightClimbed() {
    return startHeight - body.getPosition().y;
}

void ClimbingGame::moveActiveHand(float dx, float dy) {
    Hand* currentHand = (activeHand == HandControl::LEFT) ? &leftHand : &rightHand;
    
    if (currentHand->isGrabbing) return;
    
    sf::Vector2f newPos = currentHand->position + sf::Vector2f(dx, dy);
    sf::Vector2f shoulder = (activeHand == HandControl::LEFT) ? getLeftShoulder() : getRightShoulder();
    
    if (isWithinArmReach(newPos, shoulder)) {
        currentHand->position = newPos;
        currentHand->shape.setPosition(newPos);
    }
}

void ClimbingGame::toggleGrab() {
    Hand* currentHand = (activeHand == HandControl::LEFT) ? &leftHand : &rightHand;
    
    currentHand->isGrabbing = !currentHand->isGrabbing;
    
    if (currentHand->isGrabbing) {
        currentHand->shape.setFillColor((activeHand == HandControl::LEFT) ? 
            sf::Color(200, 0, 0) : sf::Color(0, 0, 200));
        switchActiveHand();
    } else {
        currentHand->shape.setFillColor((activeHand == HandControl::LEFT) ? 
            sf::Color::Red : sf::Color::Blue);
    }
}

void ClimbingGame::switchActiveHand() {
    activeHand = (activeHand == HandControl::LEFT) ? HandControl::RIGHT : HandControl::LEFT;
    bodySwingOffset = 0.f;
}

void ClimbingGame::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Space) {
                toggleGrab();
            }
        }
    }
}

void ClimbingGame::handleInput(float deltaTime) {
    Hand* activeHandPtr = (activeHand == HandControl::LEFT) ? &leftHand : &rightHand;
    Hand* passiveHandPtr = (activeHand == HandControl::LEFT) ? &rightHand : &leftHand;
    
    // Active hand controls (move the free hand)
    if (activeHand == HandControl::LEFT) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) moveActiveHand(0.f, -3.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) moveActiveHand(0.f, 3.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) moveActiveHand(-3.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) moveActiveHand(3.f, 0.f);
    } else {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) moveActiveHand(0.f, -3.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) moveActiveHand(0.f, 3.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) moveActiveHand(-3.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) moveActiveHand(3.f, 0.f);
    }
    
    // Passive hand controls body swing when other hand is active
    if (passiveHandPtr->isGrabbing) {
        if (activeHand == HandControl::RIGHT) {
            // Left hand grabbed, use WASD to swing body
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                bodySwingOffset -= swingSpeed * deltaTime;
                bodySwingOffset = std::max(bodySwingOffset, -30.f);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                bodySwingOffset += swingSpeed * deltaTime;
                bodySwingOffset = std::min(bodySwingOffset, 30.f);
            }
        } else {
            // Right hand grabbed, use arrows to swing body
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                bodySwingOffset -= swingSpeed * deltaTime;
                bodySwingOffset = std::max(bodySwingOffset, -30.f);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                bodySwingOffset += swingSpeed * deltaTime;
                bodySwingOffset = std::min(bodySwingOffset, 30.f);
            }
        }
    }
}

void ClimbingGame::updatePhysics(float deltaTime) {
    sf::Vector2f targetBodyPos;
    
    if (leftHand.isGrabbing && rightHand.isGrabbing) {
        targetBodyPos.x = (leftHand.position.x + rightHand.position.x) / 2.f + bodySwingOffset;
        targetBodyPos.y = (leftHand.position.y + rightHand.position.y) / 2.f + 50.f;
        bodyVelocity.y = 0.f;
    } else if (leftHand.isGrabbing) {
        targetBodyPos.x = leftHand.position.x + bodySwingOffset;
        targetBodyPos.y = leftHand.position.y + 50.f;
        bodyVelocity.y = 0.f;
    } else if (rightHand.isGrabbing) {
        targetBodyPos.x = rightHand.position.x + bodySwingOffset;
        targetBodyPos.y = rightHand.position.y + 50.f;
        bodyVelocity.y = 0.f;
    } else {
        targetBodyPos = body.getPosition();
        bodyVelocity.y += gravity * deltaTime;
        targetBodyPos.y += bodyVelocity.y * deltaTime;
        
        if (targetBodyPos.y > 550.f) {
            targetBodyPos.y = 550.f;
            bodyVelocity.y = 0.f;
        }
    }
    
    if (leftHand.isGrabbing || rightHand.isGrabbing) {
        sf::Vector2f diff = targetBodyPos - body.getPosition();
        body.setPosition(body.getPosition() + diff * 0.15f);
    } else {
        body.setPosition(targetBodyPos);
    }
    
    head.setPosition(body.getPosition().x, body.getPosition().y - 20.f);
    
    leftFoot.setPosition(body.getPosition().x - 8.f, body.getPosition().y + 50.f);
    rightFoot.setPosition(body.getPosition().x + 8.f, body.getPosition().y + 50.f);
    
    // Decay swing offset when not actively swinging
    bodySwingOffset *= 0.95f;
}

void ClimbingGame::updateCamera() {
    camera.setCenter(400.f, body.getPosition().y);
}

void ClimbingGame::render() {
    window.clear(sf::Color(135, 206, 235));
    window.setView(camera);
    
    // Draw wall
    for (auto& grip : wallGrips) {
        window.draw(grip);
    }
    window.draw(wall);
    
    // Draw limbs
    sf::Vertex leftArm[] = {
        sf::Vertex(getLeftShoulder(), sf::Color::White),
        sf::Vertex(leftHand.position, sf::Color::White)
    };
    sf::Vertex rightArm[] = {
        sf::Vertex(getRightShoulder(), sf::Color::White),
        sf::Vertex(rightHand.position, sf::Color::White)
    };
    sf::Vertex leftLeg[] = {
        sf::Vertex(body.getPosition() + sf::Vector2f(-5.f, 40.f), sf::Color::White),
        sf::Vertex(leftFoot.getPosition(), sf::Color::White)
    };
    sf::Vertex rightLeg[] = {
        sf::Vertex(body.getPosition() + sf::Vector2f(5.f, 40.f), sf::Color::White),
        sf::Vertex(rightFoot.getPosition(), sf::Color::White)
    };
    
    window.draw(leftArm, 2, sf::Lines);
    window.draw(rightArm, 2, sf::Lines);
    window.draw(leftLeg, 2, sf::Lines);
    window.draw(rightLeg, 2, sf::Lines);
    
    // Draw body parts
    window.draw(body);
    window.draw(head);
    window.draw(leftHand.shape);
    window.draw(rightHand.shape);
    window.draw(leftFoot);
    window.draw(rightFoot);
    
    // Draw UI
    std::ostringstream oss;
    oss << "Height: " << std::fixed << std::setprecision(1) << getHeightClimbed() << "m";
    heightText.setString(oss.str());
    heightText.setPosition(camera.getCenter().x - 380.f, camera.getCenter().y - 280.f);
    
    std::string controlStr = "Controlling: ";
    controlStr += (activeHand == HandControl::LEFT) ? "LEFT (WASD)" : "RIGHT (Arrows)";
    controlStr += " | Space: Grab | Other keys: Swing body";
    controlText.setString(controlStr);
    controlText.setPosition(camera.getCenter().x - 380.f, camera.getCenter().y + 250.f);
    
    window.draw(heightText);
    // window.draw(controlText);
    
    window.display();
}

void ClimbingGame::run() {
    sf::Clock clock;
    
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        
        handleEvents();
        handleInput(deltaTime);
        updatePhysics(deltaTime);
        updateCamera();
        render();
    }
}
