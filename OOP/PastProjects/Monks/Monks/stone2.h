#ifndef STONE2_H
#define STONE2_H

#include <stone1.h>

class Stone2:public Stone1
{
public:
    Stone2(int x, int y);
    int getType();
protected:
    QColor Draw();
};

#endif // STONE2_H
