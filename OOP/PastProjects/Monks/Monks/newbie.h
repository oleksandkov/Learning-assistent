#ifndef NEWBIE_H
#define NEWBIE_H

#include <QtCore>
#include <QtGui>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <iostream>
#include <QDebug>

using namespace std;

#include "entity.h"

class Newbie : public QObject, public Entity
{
    Q_OBJECT
public:
    Newbie(int x, int y, bool active=true, double expirience=0.0,double energy=0.0, QString name="DzenMonk");
    Newbie(const Newbie *m);//copy constructor
    Newbie();
    Newbie &operator=(Newbie &m);//
    bool operator< (Newbie &arg);
    void operator++();
    friend bool operator==(Newbie &w1, Newbie &w2)
    {
        if(w1.energy==w2.energy) return true;
        return false;
    }

//    friend ostream &operator<<( ostream &ost, Newbie &m )
//    {
//        ost<< m.tname;
//        return ost;
//    }

    void mousePressEvent(QGraphicsSceneMouseEvent *event);

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
    QGraphicsEllipseItem *shadow;
    QGraphicsRectItem    *expirienceLine;
    QGraphicsRectItem    *energyLine;
    int                  *Blessing;
    int                  ntypes;//типи благословення
    int                  *MonksEnergy;
    int                  *MonkWhoGaveEnergy;
    int                  nInteractingMonks;
    int                  NumberOfStone;
    bool                 ignoreEverything;
    bool                 ignoreMonks;

    bool                 ghandarm;
    static int arr_size;
    static int getArrSize();

    QColor Draw();
    virtual ~Newbie();
    virtual int getSpeed();
    virtual void setPosition(qreal x, qreal y);
    virtual void setLines();
    virtual int getType();
    int getTrueType();
    void addEnergy(double n, int m);
    void Karma(int n);
    void OpacityEffect(int value);
    void Print();
protected:

signals:
    changeLevel(Newbie* m);
    printInfo(QString st);
};

//QDebug operator<<(QDebug out, Newbie &m)
//{
//    out << m.tname;
//    return out.maybeSpace();
//}



#endif // NEWBIE_H
