#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QtCore>
#include <QtGui>
#include <QGraphicsView>


class MainView : public QGraphicsView
{
public:
    MainView();
    QGraphicsScene *scene;

};

#endif // MAINVIEW_H
