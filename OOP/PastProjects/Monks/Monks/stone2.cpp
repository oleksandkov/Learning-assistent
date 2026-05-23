#include "stone2.h"

Stone2::Stone2(int x, int y):Stone1(x,y)
{
    setPixmap(QPixmap(":images/Stone2.png"));


    shadow->setRect(0,0,273,50);
    shadow->setPos(x+18,y+204);

    minirect->setBrush(QBrush(Draw(),Qt::SolidPattern));
    nMonks = 3-getType()+1;
}

int Stone2::getType()
{
    return 2;
}

QColor Stone2::Draw()
{
    QColor color;
    color.setRgb(6, 56, 3);
    return color;
}
