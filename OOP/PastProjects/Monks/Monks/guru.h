#ifndef GURU_H
#define GURU_H

#include <expirienced.h>

class Guru: public Expirienced
{
public:
    Guru(int x, int y, bool active, double expirience,double energy, QString name);
    QColor Draw();
    void setLines();

    ~Guru();
    int getSpeed();
    int getType();
    //void addEnergy(double n, int m);
};

#endif // GURU_H
