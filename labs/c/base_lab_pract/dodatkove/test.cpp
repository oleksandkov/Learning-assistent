#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <algorithm>

// Простий 2D вектор
struct point { double x, y; };

// Перетин двох прямих (p + r*t) і (q + s*u).
// Повертає true і записує в 'out' якщо є єдиний перетин (для невироджених прямих).
bool lineIntersection(const point& p, const point& r, const point& q, const point& s, point& out) {
    // Розв'язуємо p + r*t = q + s*u
    // t = cross(q - p, s) / cross(r, s)
    double cross_rs = r.x * s.y - r.y * s.x;
    if (cross_rs == 0) return false; // паралельні або майже
    point qp{q.x - p.x, q.y - p.y};
    double t = (qp.x * s.y - qp.y * s.x) / cross_rs;
    out.x = p.x + r.x * t;
    out.y = p.y + r.y * t;
    return true;
}

// Допоміжна функція: перетин двох відрізків (a-b) та (c-d).
// Ми не обмежуємося перевіркою, чи перетинаються тільки в межах відрізків, 
// бо для пентаграми перетини будуть всередині відрізків. Але все ж перевіримо.
bool segmentIntersection(const point& a, const point& b, const point& c, const point& d, point& out) {
    point r{b.x - a.x, b.y - a.y};
    point s{d.x - c.x, d.y - c.y};
    // використаємо параметричний підхід
    double cross_rs = r.x * s.y - r.y * s.x;
    if (cross_rs == 0) return false;
    point ac{c.x - a.x, c.y - a.y};
    double t = (ac.x * s.y - ac.y * s.x) / cross_rs;
    double u = (ac.x * r.y - ac.y * r.x) / cross_rs;
    if (t < -1e-9 || t > 1 + 1e-9 || u < -1e-9 || u > 1 + 1e-9) return false; // перетин поза відрізками
    out.x = a.x + r.x * t;
    out.y = a.y + r.y * t;
    return true;
}

int main() {
    const int width = 500, height = 500; // >= 500x500
    sf::RenderWindow window(sf::VideoMode(width, height), "Dodatkove");
    window.setFramerateLimit(60);

    // Білий фон (ми просто clear білим кольором)
    const point center{width / 2.0, height / 2.0};
    const double radius = 220.0; // радіус зовнішнього п'ятикутника

    // Обчислимо 5 вершин регулярного п'ятикутника.
    std::vector<point> outer(5);
    // Щоб п'ятикутник "стояв" вершиною вверх, початковий кут = -pi/2
    double startAngle = -M_PI / 2.0;
    for (int i = 0; i < 5; ++i) {
        double angle = startAngle + i * 2.0 * M_PI / 5.0;
        outer[i].x = center.x + radius * std::cos(angle);
        outer[i].y = center.y + radius * std::sin(angle);
    }

    // Побудуємо відрізки з кроком 2 (пентаграма): кожна вершина -> (i+2)%5
    struct Segment { point a, b; };
    std::vector<Segment> starSegments;
    for (int i = 0; i < 5; ++i) {
        Segment s{ outer[i], outer[(i + 2) % 5] };
        starSegments.push_back(s);
    }

    // Знайдемо 5 точок перетину внутрішнього п'ятикутника.
    // Інтуїтивно, точка i = перетин сегментів s_i і s_{i+1},
    // де s_i = (v[i], v[i+2]) і s_{i+1} = (v[i+1], v[i+3]).
    std::vector<point> innerPoints;
    for (int i = 0; i < 5; ++i) {
        Segment s1 = starSegments[i];
        Segment s2 = starSegments[(i + 1) % 5];
        point ip;
        if (segmentIntersection(s1.a, s1.b, s2.a, s2.b, ip)) {
            innerPoints.push_back(ip);
        }
    }



    // Сортування внутрішніх точок за кутом навколо їх центроїду, щоб задати порядок для ConvexShape.
    point centroid;
    centroid.x = 0;
    centroid.y = 0;
    for (int i = 0; i < innerPoints.size(); i++) {
    centroid.x = centroid.x + innerPoints[i].x;
    centroid.y = centroid.y + innerPoints[i].y;
}
centroid.x = centroid.x / innerPoints.size();
centroid.y = centroid.y / innerPoints.size();

std::sort(innerPoints.begin(), innerPoints.end(), [&](const point& A, const point& B) {
    double angA = std::atan2(A.y - centroid.y, A.x - centroid.x);
    double angB = std::atan2(B.y - centroid.y, B.x - centroid.x);
    return angA < angB;
});

    // Підготуємо SFML фігури:
    // 1) Нарисуємо зовнішній п'ятикутник (контур)
    sf::ConvexShape outerShape;
    outerShape.setPointCount(5);
    for (int i = 0; i < 5; ++i) outerShape.setPoint(i, sf::Vector2f((float)outer[i].x, (float)outer[i].y));
    outerShape.setFillColor(sf::Color::Transparent);
    outerShape.setOutlineColor(sf::Color::Black);
    outerShape.setOutlineThickness(2.0f);

    // 2) Відрізки зірки (тонкі лінії)
    std::vector<sf::VertexArray> starLines;
    for (auto &s : starSegments) {
        sf::VertexArray va(sf::Lines, 2);
        va[0].position = sf::Vector2f((float)s.a.x, (float)s.a.y);
        va[1].position = sf::Vector2f((float)s.b.x, (float)s.b.y);
        va[0].color = sf::Color::Black;
        va[1].color = sf::Color::Black;
        starLines.push_back(va);
    }

    // 3) Внутрішній п'ятикутник (заповнений іншим кольором)
    sf::ConvexShape innerShape;
    if (!innerPoints.empty()) {
        innerShape.setPointCount(innerPoints.size());
        for (size_t i = 0; i < innerPoints.size(); ++i) {
            innerShape.setPoint(i, sf::Vector2f((float)innerPoints[i].x, (float)innerPoints[i].y));
        }
        innerShape.setFillColor(sf::Color(220, 80, 80, 200));
        innerShape.setOutlineColor(sf::Color::Black);
        innerShape.setOutlineThickness(1.0f);
    }

    // Цикл рендерингу
    while (window.isOpen()) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) window.close();
        }

        window.clear(sf::Color::White);

        // Малюємо зовнішній контур
        window.draw(outerShape);

        // Малюємо зірку (лінії)
        for (auto &va : starLines) window.draw(va);

        // Малюємо внутрішній зафарбований п'ятикутник (якщо є)
        if (innerPoints.size() == 5) window.draw(innerShape);

        window.display();
    }

    return 0;
}