#include "mainview.h"

MainView::MainView()
{
    setBaseSize(1000,700);

    scene = new QGraphicsScene(this);
    setScene(scene);
    centerOn(0,0);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCursor(Qt::ArrowCursor);

    scene->setSceneRect(0,0,3200,2400);
    scene->setBackgroundBrush(QBrush(QPixmap(":/images/Grass")));

}
