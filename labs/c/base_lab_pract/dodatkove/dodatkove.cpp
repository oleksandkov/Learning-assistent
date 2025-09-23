#include <GL/freeglut.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <GL/glu.h>

struct Point { double x, y; };

static int WIN_W = 600;
static int WIN_H = 600;

static double eps = 1e-9;

// 2D cross product (a x b)
double cross(double ax, double ay, double bx, double by) {
    return ax * by - ay * bx;
}

// intersection of segments p1-p2 and p3-p4
bool intersectSegments(const Point &p1, const Point &p2, const Point &p3, const Point &p4, Point &out) {
    double r_x = p2.x - p1.x;
    double r_y = p2.y - p1.y;
    double s_x = p4.x - p3.x;
    double s_y = p4.y - p3.y;

    double denom = cross(r_x, r_y, s_x, s_y);
    if (fabs(denom) < eps) return false; // parallel

    double qp_x = p3.x - p1.x;
    double qp_y = p3.y - p1.y;

    double t = cross(qp_x, qp_y, s_x, s_y) / denom;
    double u = cross(qp_x, qp_y, r_x, r_y) / denom;

    if (t > eps && t < 1.0 - eps && u > eps && u < 1.0 - eps) { // strict inside segments (avoid endpoints)
        out.x = p1.x + t * r_x;
        out.y = p1.y + t * r_y;
        return true;
    }
    return false;
}

void computePentagon(std::vector<Point> &verts, double cx, double cy, double R, double rot = 0.0) {
    verts.clear();
    for (int i = 0; i < 5; ++i) {
        double ang = rot + 2.0 * M_PI * i / 5.0;
        verts.push_back({ cx + R * cos(ang), cy + R * sin(ang) });
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Compute outer pentagon centered at (0,0) in normalized coords
    double cx = 0.0, cy = 0.0;
    double R = 0.7; // fits well in [-1,1] viewport
    std::vector<Point> pent;
    // rotate so one vertex up
    computePentagon(pent, cx, cy, R, -M_PI/2.0);

    // Draw diagonals and collect their intersections
    std::vector<Point> intersections;
    // list of diagonal segments: connect i to i+2 (mod 5) and i to i+3 are the same lines reversed, so only i->i+2 for i=0..4
    std::vector<std::pair<Point, Point>> diagonals;
    for (int i = 0; i < 5; ++i) {
        Point a = pent[i];
        Point b = pent[(i + 2) % 5];
        diagonals.push_back({ a, b });
    }

    // find all pairwise intersections among diagonals
    for (size_t i = 0; i < diagonals.size(); ++i) {
        for (size_t j = i + 1; j < diagonals.size(); ++j) {
            // skip if lines share an endpoint
            Point a1 = diagonals[i].first, a2 = diagonals[i].second;
            Point b1 = diagonals[j].first, b2 = diagonals[j].second;
            bool share = (fabs(a1.x - b1.x) < eps && fabs(a1.y - b1.y) < eps) ||
                         (fabs(a1.x - b2.x) < eps && fabs(a1.y - b2.y) < eps) ||
                         (fabs(a2.x - b1.x) < eps && fabs(a2.y - b1.y) < eps) ||
                         (fabs(a2.x - b2.x) < eps && fabs(a2.y - b2.y) < eps);
            if (share) continue;

            Point ip;
            if (intersectSegments(a1, a2, b1, b2, ip)) {
                // uniqueness check
                bool found = false;
                for (auto &p : intersections) {
                    if (fabs(p.x - ip.x) < 1e-6 && fabs(p.y - ip.y) < 1e-6) { found = true; break; }
                }
                if (!found) intersections.push_back(ip);
            }
        }
    }

    // If intersections found, compute centroid and sort them by angle for proper polygon fill
    if (!intersections.empty()) {
        double sx = 0, sy = 0;
        for (auto &p : intersections) { sx += p.x; sy += p.y; }
        double cx_i = sx / intersections.size();
        double cy_i = sy / intersections.size();

        std::sort(intersections.begin(), intersections.end(), [&](const Point &a, const Point &b){
            double aa = atan2(a.y - cy_i, a.x - cx_i);
            double bb = atan2(b.y - cy_i, b.x - cx_i);
            return aa < bb;
        });
    }

    // Draw outer pentagon outline (black)
    glColor3f(0, 0, 0);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (auto &v : pent) glVertex2d(v.x, v.y);
    glEnd();

    // Draw diagonals (thin gray)
    glColor3f(0.3f, 0.3f, 0.3f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (auto &d : diagonals) {
        glVertex2d(d.first.x, d.first.y);
        glVertex2d(d.second.x, d.second.y);
    }
    glEnd();

    // Fill inner pentagon with different color if exists
    if (intersections.size() >= 3) {
        glColor3f(1.0f, 0.6f, 0.0f); // orange fill for inner pentagon
        glBegin(GL_POLYGON);
        for (auto &p : intersections) glVertex2d(p.x, p.y);
        glEnd();

        // draw border for inner pentagon
        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(1.5f);
        glBegin(GL_LINE_LOOP);
        for (auto &p : intersections) glVertex2d(p.x, p.y);
        glEnd();
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    WIN_W = w; WIN_H = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Simple orthographic projection, always square [-1,1] x [-1,1]
    if (w > h) {
        float aspect = (float)w / (float)h;
        glScalef(1.0f / aspect, 1.0f, 1.0f);
    } else if (h > w) {
        float aspect = (float)h / (float)w;
        glScalef(1.0f, 1.0f / aspect, 1.0f);
    }
    // No GLU needed
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIN_W, WIN_H);
    glutCreateWindow("Pentagon + inner pentagon (intersections)");
    // White background
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}