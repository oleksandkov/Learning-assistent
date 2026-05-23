#ifndef MONK_H
#define MONK_H


#include "entity.h"

class Monk : public Entity
{
public:
    Monk(int x, int y, bool active=true, double expirience=0.0,double energy=0.0, QString name="DzenMonk");
    bool                 active;
    double               expirience;
    double               energy;
    int                  speed;
    int                  targetx,targety;
    bool                 achived;
    bool                 pause;
    QString              tname;
    QString              htmlColor;
    QGraphicsRectItem    *target;
    QGraphicsEllipseItem *ellipse;
    QGraphicsTextItem    *name;
    QGraphicsEllipseItem *mini;
    QGraphicsRectItem    *expirienceLine;
    QGraphicsRectItem    *energyLine;
    int                  *Blessing;
    int                  ntypes;//типи благословення
    int                  *MonksEnergy;
    int                  *MonkWhoGaveEnergy;
    int                  nInteractingMonks;
    int                  NumberOfStone;
    bool                 ignoreEverything;

    QColor setColor();
    ~Monk();
    virtual int getSpeed();
    virtual void setPosition(qreal x, qreal y);
    virtual void setLines();
    virtual int getType();
    int getTrueType();
    void addEnergy(double n, int m);
    void Karma(int n);
    void OpacityEffect(int value);
};

#endif // MONK_H
