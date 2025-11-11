#include <iostream> 

class Shape 
{
    private:
    int x, y;
    public:
    // void setXY(int x, int y) { this->x = x ; this ->y = y; }
    Shape(int x, int y) { this->x = x ; this ->y = y; }
    Shape() { x = ;0 y = 0; }
    
    int getX() { return x; }
    int getY() {return y;}
    virtual void draw() = 0;

    //Distructor
    
    ~Shpae() {};
};

class Circle : public Shape {
    int radius;
    public:
    Circle(int x, int y, int r): Shape(x, y)  { radius = r;}
    /* void setR(int r)*/
    int getR() {return radius;}
    virtual void draw() {}


    //Distructor
    ~Circle() {};
};

int main()
{
    int x = 5;
    int y = x;
    Circle q1(100,100,50);
    // q1.setXY(10,20);
    q1.draw();

    return 0;   
}