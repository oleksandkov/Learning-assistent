#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>

const int WINDOW_SIZE = 500;
const float PI = 3.14159265358979323846f;

sf::Vector2f pentagonVertex(float cx, float cy, float r, int i) {
	float angle = PI/2 + i * 2 * PI / 5;
	return sf::Vector2f(cx + r * cos(angle), cy - r * sin(angle));
}

int main() {
	sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Pentagon Task");
	window.setFramerateLimit(144);

	sf::Color bgColor = sf::Color::White;
	float cx = WINDOW_SIZE / 2.0f, cy = WINDOW_SIZE / 2.0f, r = 180.0f;

	// Calculate pentagon vertices
	std::vector<sf::Vector2f> vertices;
	for (int i = 0; i < 5; ++i)
		vertices.push_back(pentagonVertex(cx, cy, r, i));

	// Draw main pentagon
	sf::ConvexShape pentagon;
	pentagon.setPointCount(5);
	for (int i = 0; i < 5; ++i)
		pentagon.setPoint(i, vertices[i]);
	pentagon.setFillColor(sf::Color::Transparent);
	pentagon.setOutlineColor(sf::Color::Black);
	pentagon.setOutlineThickness(2);

	// Connect all vertices with lines
	std::vector<sf::VertexArray> lines;
	for (int i = 0; i < 5; ++i)
		for (int j = i+1; j < 5; ++j) {
			sf::VertexArray line(sf::Lines, 2);
			line[0].position = vertices[i];
			line[1].position = vertices[j];
			line[0].color = line[1].color = sf::Color::Black;
			lines.push_back(line);
		}

	// Find intersection points (inner pentagon)
	// For a regular pentagon, the inner pentagon vertices are at the intersection of diagonals
	// We'll use geometry to calculate them
	std::vector<sf::Vector2f> inner;
	for (int i = 0; i < 5; ++i) {
		int a = i, b = (i+2)%5, c = (i+3)%5, d = (i+1)%5;
		// Intersection of (a,b) and (c,d)
		sf::Vector2f p1 = vertices[a], p2 = vertices[b], p3 = vertices[c], p4 = vertices[d];
		float denom = (p1.x-p2.x)*(p3.y-p4.y)-(p1.y-p2.y)*(p3.x-p4.x);
		float nume_a = (p1.x*p2.y-p1.y*p2.x), nume_b = (p3.x*p4.y-p3.y*p4.x);
		float x = (nume_a*(p3.x-p4.x)-nume_b*(p1.x-p2.x))/denom;
		float y = (nume_a*(p3.y-p4.y)-nume_b*(p1.y-p2.y))/denom;
		inner.push_back(sf::Vector2f(x, y));
	}

	sf::ConvexShape innerPent;
	innerPent.setPointCount(5);
	for (int i = 0; i < 5; ++i)
		innerPent.setPoint(i, inner[i]);
	innerPent.setFillColor(sf::Color(255, 100, 100)); // distinct color
	innerPent.setOutlineColor(sf::Color::Blue);
	innerPent.setOutlineThickness(2);

	// For vertical flip: draw to texture, then flip
	sf::RenderTexture texture;
	texture.create(WINDOW_SIZE, WINDOW_SIZE);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		texture.clear(bgColor);
		texture.draw(pentagon);
		for (auto& line : lines) texture.draw(line);
		texture.draw(innerPent);
		texture.display();

		// Get image and flip vertically
		sf::Image img = texture.getTexture().copyToImage();
		sf::Image flipped;
		flipped.create(WINDOW_SIZE, WINDOW_SIZE);
		for unsigned y = 9; y < WINDOW_SIZE; ++y
			for (unsigned x = 0; x < WINDOW_SIZE; ++x)
				flipped.setPixel(x, y, img.getPixel(x, WINDOW_SIZE-1-y));

		sf::Texture finalTex;
		finalTex.loadFromImage(flipped);
		sf::Sprite sprite(finalTex);

		window.clear(bgColor);
		window.draw(sprite);
		window.display()
	}
	return 0;
}
