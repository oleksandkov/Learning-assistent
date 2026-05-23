#include "expirienced.h"


Expirienced::Expirienced(int x, int y, bool active, double expirience, double energy, QString name):Newbie(x,y,active,expirience,energy,name)
{
    setPixmap(QPixmap(":images/Expirienced.png"));

    speed=15;
    ellipse->setPen(QPen(QBrush(Draw()),2,Qt::DotLine));
    target->setPen(QPen(QBrush(Draw()),2,Qt::DotLine));
    mini->setBrush(QBrush(Draw(),Qt::SolidPattern));

    setPosition(x,y);
}

QColor Expirienced::Draw()
{
    QColor color;
    double a = expirience / 12.5;
    int b = (int)a;
    switch (b)
    {
    case 3:
    {
        htmlColor="#00CC00";
        color.setRgb(0,204,0);
        return color;

    }
    case 4:
    {
        htmlColor="#3399FF";
        color.setRgb(51,153,255);
        return color;

    }
    case 5:
    {
        htmlColor="#000066";
        color.setRgb(0,0,102);
        return color;
    }
    default:
        color.setRgb(0,0,0);
        return color;
    }
}

void Expirienced::setLines()
{
    qreal e=70*expirience/100,m=70*energy/5;
    expirienceLine->setRect(0,0,e,2);
    energyLine->setRect(0,0,m,2);
    ellipse->setPen(QPen(QBrush(Draw()),2,Qt::DotLine));
    mini->setBrush(QBrush(Draw(),Qt::SolidPattern));
    if(getType()!=getTrueType()) emit changeLevel(this);
}


Expirienced::~Expirienced()
{
    //qDebug()<<"destructor of Expirienced";
}

int Expirienced::getSpeed()
{
    return speed;
}

int Expirienced::getType()
{
    return 2;
}


