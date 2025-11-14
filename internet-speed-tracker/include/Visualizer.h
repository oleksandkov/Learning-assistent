#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "SpeedTester.h"

class Visualizer {
private:
    sf::RenderWindow window;
    sf::Font font;
    std::vector<SpeedResult> speedData;
    
    const int WINDOW_WIDTH = 1200;
    const int WINDOW_HEIGHT = 700;
    const int GRAPH_PADDING = 80;
    
    void drawGrid();
    void drawGraphs();
    void drawStats();
    void drawLabels();
    void drawLegend();
    
    float mapValue(float value, float minVal, float maxVal, float screenMin, float screenMax);
    
public:
    Visualizer();
    ~Visualizer();
    
    bool isOpen() const;
    void update(const std::vector<SpeedResult>& data);
    void render();
    bool handleEvents();
};

#endif
