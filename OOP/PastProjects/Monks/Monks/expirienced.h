#ifndef EXPIRIENCED_H
#define EXPIRIENCED_H

#include <newbie.h>

class Expirienced : public Newbie
{
public:
    Expirienced(int x, int y, bool active, double expirience,double energy,QString name);
    QColor Draw();
    void setLines();

    ~Expirienced();
    int getSpeed();
    int getType();
    //void addEnergy(double n, int m);
};

#endif // EXPIRIENCED_H
