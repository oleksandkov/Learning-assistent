#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtGui>
#include <QMainWindow>
#include <typeinfo>
#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>
#include <algorithm>
#include <QApplication>

#include "entity.h"
#include "newbie.h"
#include "expirienced.h"
#include "guru.h"
#include "stone1.h"
#include "stone2.h"
#include "stone3.h"
#include "interactionwithmonkdialog.h"
#include "interactionwithstonedialog.h"
#include "insertmonkdialog.h"
#include "minimap.h"
#include "chat.h"

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void keyPressEvent(QKeyEvent *event);
    void RemoveMonk(int i);
    void RemoveStone(int i);
    void InsertMonk(int type, int x, int y, bool active, double expirience,double energy, QString name);
    void InsertStone(int type, int x, int y);
    void MonksInterakting(int m1, int m2);
    void MonkInteractsWithStone(int m, int s);

    //map attributes
    QGraphicsTextItem* amount;
    QGraphicsTextItem* coordinates;
    QTimer *textTimer;
    MiniMap *minimap;
    Chat *chat;
private slots:
    void setText();
    //movement of monks
    void move();
    void moveMonk(int i, int x, int y);


    //windows
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionExit_triggered();
    //
    void ChangeLevel(Newbie *m);

    void on_actionMiniMap_triggered();

    void on_actionChat_triggered();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
