#include "guru.h"


Guru::Guru(int x, int y, bool active, double expirience, double energy, QString name):Expirienced(x,y,active,expirience,energy,name)
{
    setPixmap(QPixmap(":images/Guru.png"));

    speed=20;
    ellipse->setPen(QPen(QBrush(Draw()),2,Qt::DotLine));
    target->setPen(QPen(QBrush(Draw()),2,Qt::DotLine));
    mini->setBrush(QBrush(Draw(),Qt::SolidPattern));

    setPosition(x,y);
}

QColor Guru::Draw()
{
    QColor color;
    double a = expirience / 12.5;
    int b = (int)a;
    switch (b)
    {
    case 6:
    {
        htmlColor="#660066";
        color.setRgb(102,0,102);
        return color;
    }
    case 7:
    {
        htmlColor="#99FFFF";
        color.setRgb(153,255,255);
        return color;
    }
    default:
        color.setRgb(0,0,0);
        return color;
    }
}

void Guru::setLines()
{
    qreal e=70*expirience/100,m=70*energy/7;
    expirienceLine->setRect(0,0,e,2);
    energyLine->setRect(0,0,m,2);
    ellipse->setPen(QPen(QBrush(Draw()),2,Qt::DotLine));
    mini->setBrush(QBrush(Draw(),Qt::SolidPattern));
    if(getType()!=getTrueType()) emit changeLevel(this);
}

Guru::~Guru()
{
    //qDebug()<<"destructor of Guru";
}

int Guru::getSpeed()
{
    return speed;
}

int Guru::getType()
{
    return 3;
}


