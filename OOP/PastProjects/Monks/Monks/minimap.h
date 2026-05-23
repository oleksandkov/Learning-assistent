#ifndef MINIMAP_H
#define MINIMAP_H

#include <QDialog>
#include <QGraphicsView>

namespace Ui {
class MiniMap;
}

class MiniMap : public QDialog
{
    Q_OBJECT

public:
    explicit MiniMap(QWidget *parent = 0);
    ~MiniMap();
    QGraphicsScene *scene;

private:
    Ui::MiniMap *ui;
};

#endif // MINIMAP_H
