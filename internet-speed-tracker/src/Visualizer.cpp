#include "Visualizer.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

Visualizer::Visualizer() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Internet Speed Tracker") {
    window.setFramerateLimit(30);
    
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Warning: Could not load font\n";
    }
}

Visualizer::~Visualizer() {
    window.close();
}

bool Visualizer::isOpen() const {
    return window.isOpen();
}

bool Visualizer::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            return false;
        }
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            return false;
        }
    }
    return true;
}

void Visualizer::update(const std::vector<SpeedResult>& data) {
    speedData = data;
}

float Visualizer::mapValue(float value, float minVal, float maxVal, float screenMin, float screenMax) {
    if (maxVal == minVal) return screenMin;
    return screenMin + (value - minVal) / (maxVal - minVal) * (screenMax - screenMin);
}

void Visualizer::drawGrid() {
    sf::Color gridColor(200, 200, 200);
    
    for (int i = 0; i <= 5; i++) {
        float y = GRAPH_PADDING + 300 + (i * 60);
        sf::RectangleShape line(sf::Vector2f(WINDOW_WIDTH - 2 * GRAPH_PADDING, 1));
        line.setPosition(GRAPH_PADDING, y);
        line.setFillColor(gridColor);
        window.draw(line);
    }
    
    for (int i = 0; i <= 10; i++) {
        float x = GRAPH_PADDING + (i * (WINDOW_WIDTH - 2 * GRAPH_PADDING) / 10);
        sf::RectangleShape line(sf::Vector2f(1, 300));
        line.setPosition(x, GRAPH_PADDING);
        line.setFillColor(gridColor);
        window.draw(line);
    }
}

void Visualizer::drawGraphs() {
    if (speedData.empty()) return;
    
    double maxSpeed = 100.0;
    for (const auto& result : speedData) {
        maxSpeed = std::max(maxSpeed, std::max(result.downloadSpeed, result.uploadSpeed));
    }
    
    int graphWidth = WINDOW_WIDTH - 2 * GRAPH_PADDING;
    int graphHeight = 300;
    
    for (size_t i = 0; i < speedData.size(); i++) {
        float x = GRAPH_PADDING + (i * graphWidth / speedData.size());
        
        float downloadY = GRAPH_PADDING + graphHeight - 
                         mapValue(speedData[i].downloadSpeed, 0, maxSpeed, 0, graphHeight);
        float uploadY = GRAPH_PADDING + graphHeight - 
                       mapValue(speedData[i].uploadSpeed, 0, maxSpeed, 0, graphHeight);
        
        if (i > 0) {
            float prevDownloadY = GRAPH_PADDING + graphHeight - 
                                 mapValue(speedData[i-1].downloadSpeed, 0, maxSpeed, 0, graphHeight);
            float prevUploadY = GRAPH_PADDING + graphHeight - 
                               mapValue(speedData[i-1].uploadSpeed, 0, maxSpeed, 0, graphHeight);
            
            float prevX = GRAPH_PADDING + ((i-1) * graphWidth / speedData.size());
            
            sf::Vertex downloadLine[] = {
                sf::Vertex(sf::Vector2f(prevX, prevDownloadY), sf::Color::Green),
                sf::Vertex(sf::Vector2f(x, downloadY), sf::Color::Green)
            };
            window.draw(downloadLine, 2, sf::Lines);
            
            sf::Vertex uploadLine[] = {
                sf::Vertex(sf::Vector2f(prevX, prevUploadY), sf::Color::Red),
                sf::Vertex(sf::Vector2f(x, uploadY), sf::Color::Red)
            };
            window.draw(uploadLine, 2, sf::Lines);
        }
        
        sf::CircleShape downloadPoint(3);
        downloadPoint.setFillColor(sf::Color::Green);
        downloadPoint.setPosition(x - 3, downloadY - 3);
        window.draw(downloadPoint);
        
        sf::CircleShape uploadPoint(3);
        uploadPoint.setFillColor(sf::Color::Red);
        uploadPoint.setPosition(x - 3, uploadY - 3);
        window.draw(uploadPoint);
    }
}

void Visualizer::drawLabels() {
    sf::Text yLabel("Speed (Mbps)", font, 14);
    yLabel.setPosition(10, GRAPH_PADDING + 100);
    yLabel.setFillColor(sf::Color::Black);
    window.draw(yLabel);
    
    sf::Text xLabel("Time", font, 14);
    xLabel.setPosition(WINDOW_WIDTH - 100, GRAPH_PADDING + 320);
    xLabel.setFillColor(sf::Color::Black);
    window.draw(xLabel);
}

void Visualizer::drawLegend() {
    int legendX = WINDOW_WIDTH - 250;
    int legendY = 20;
    
    sf::CircleShape downloadDot(4);
    downloadDot.setFillColor(sf::Color::Green);
    downloadDot.setPosition(legendX, legendY);
    window.draw(downloadDot);
    
    sf::Text downloadLabel("Download", font, 12);
    downloadLabel.setPosition(legendX + 20, legendY - 2);
    downloadLabel.setFillColor(sf::Color::Black);
    window.draw(downloadLabel);
    
    sf::CircleShape uploadDot(4);
    uploadDot.setFillColor(sf::Color::Red);
    uploadDot.setPosition(legendX, legendY + 25);
    window.draw(uploadDot);
    
    sf::Text uploadLabel("Upload", font, 12);
    uploadLabel.setPosition(legendX + 20, legendY + 23);
    uploadLabel.setFillColor(sf::Color::Black);
    window.draw(uploadLabel);
}

void Visualizer::drawStats() {
    int statsX = 20;
    int statsY = GRAPH_PADDING + 330;
    
    if (!speedData.empty()) {
        double avgDown = 0, avgUp = 0, avgPing = 0;
        for (const auto& r : speedData) {
            avgDown += r.downloadSpeed;
            avgUp += r.uploadSpeed;
            avgPing += r.ping;
        }
        avgDown /= speedData.size();
        avgUp /= speedData.size();
        avgPing /= speedData.size();
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Tests: " << speedData.size() 
           << " | Avg Down: " << avgDown << " Mbps"
           << " | Avg Up: " << avgUp << " Mbps"
           << " | Avg Ping: " << avgPing << " ms";
        
        sf::Text statsText(ss.str(), font, 12);
        statsText.setPosition(statsX, statsY);
        statsText.setFillColor(sf::Color::Black);
        window.draw(statsText);
    }
}

void Visualizer::render() {
    window.clear(sf::Color::White);
    
    drawGrid();
    drawGraphs();
    drawLabels();
    drawLegend();
    drawStats();
    
    window.display();
}
