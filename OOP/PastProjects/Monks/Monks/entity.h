#ifndef ENTITY_H
#define ENTITY_H

#include <QtCore>
#include <QtGui>
#include <QGraphicsPixmapItem>


class Entity :public QGraphicsPixmapItem
{
public:
    Entity(int x, int y);
    Entity();
    int x,y;
protected:
    virtual QColor Draw()=0;
};

#endif // ENTITY_H
