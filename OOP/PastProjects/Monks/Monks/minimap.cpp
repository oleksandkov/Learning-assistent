#include "minimap.h"
#include "ui_minimap.h"

MiniMap::MiniMap(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MiniMap)
{
    ui->setupUi(this);
    scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
    setWindowTitle("Міні-карта");
    setWindowIcon(QIcon(":/images/bm.ico"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);


    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

MiniMap::~MiniMap()
{
    delete ui;
}
