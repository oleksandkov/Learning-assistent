#include "stone3.h"

Stone3::Stone3(int x, int y):Stone2(x,y)
{
    setPixmap(QPixmap(":images/Stone3.png"));


    shadow->setRect(0,0,262,65);
    shadow->setPos(x+39,y+198);

    minirect->setBrush(QBrush(Draw(),Qt::SolidPattern));
    nMonks = 3-getType()+1;
}

int Stone3::getType()
{
    return 3;
}

QColor Stone3::Draw()
{
    QColor color;
    color.setRgb(68, 69, 67);
    return color;

}
