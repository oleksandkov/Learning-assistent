#ifndef STONE1_H
#define STONE1_H

#include <entity.h>
#include <newbie.h>
#include <QTimer>

class Stone1: public QObject, public Entity //, public QObject
{
    Q_OBJECT
public:

    Stone1(int x, int y);
    ~Stone1();
    QGraphicsRectItem *minirect;
    QGraphicsTextItem *numberOfMonks;
    QGraphicsEllipseItem *shadow;
    Newbie **sittingMonk;
    int nMonks;
    int kilkist;

    virtual int getType();
    void addMonk(Newbie *p);
    void returnMonks();
    QTimer *timer;

protected:
    QColor Draw();
public slots:
    void ReturnsMonks();

};

#endif // STONE1_H
