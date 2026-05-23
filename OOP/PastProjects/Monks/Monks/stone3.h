#ifndef STONE3_H
#define STONE3_H

#include <stone2.h>

class Stone3: public Stone2
{
public:
    Stone3(int x, int y);
    int getType();
protected:
    QColor Draw();
};

#endif // STONE3_H
