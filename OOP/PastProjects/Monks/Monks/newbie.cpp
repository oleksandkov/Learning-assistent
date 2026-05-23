#include "newbie.h"
#include "stone1.h"
#include "mainview.h"

extern Stone1 **stone;
extern Newbie **monk;
extern MainView *view;

Newbie::Newbie(int x, int y, bool active, double expirience, double energy, QString name):Entity(x,y),speed(10)
{
    setPixmap(QPixmap(":images/Newbie.png"));
    this->active=active;
    this->expirience=expirience;

    if(energy<0) this->energy=0;
    else if(energy>getType()*2+1) energy=getType()*2+1;
    else this->energy=energy;

    ellipse = new QGraphicsEllipseItem();
    ellipse->setRect(0,0,100,100);
    ellipse->setPen(QPen(QBrush(Draw()),2,Qt::DotLine));

    target = new QGraphicsRectItem();
    target->setRect(0,0,2,2);
    target->setPen(QPen(QBrush(Draw()),2,Qt::DotLine));
    targetx=qrand()%3000+x%100;
    targety=qrand()%2200+x%100;
    achived=false;
    pause=false;

    this->tname=name;
    this->name = new QGraphicsTextItem();
    this->name->setPlainText(name);
    this->name->setDefaultTextColor(Qt::white);
    this->name->setFont(QFont("times",12));

    shadow = new QGraphicsEllipseItem();
    shadow->setBrush(QBrush(QColor(52,21,21)));
    shadow->setRect(0,0,58,8);
    shadow->setPen(Qt::NoPen);
    shadow->setOpacity(0.8);


    mini = new QGraphicsEllipseItem();
    mini->setRect(0,0,12.5,12.5);
    mini->setBrush(QBrush(Draw(),Qt::SolidPattern));
    mini->setOpacity(0.7);

    expirienceLine = new QGraphicsRectItem();
    expirienceLine->setBrush(QBrush(Qt::cyan,Qt::SolidPattern));

    energyLine = new QGraphicsRectItem();
    energyLine->setBrush(QBrush(Qt::green,Qt::SolidPattern));

    setLines();
    setPosition(x,y);

    ntypes = 3;
    Blessing = new int[ntypes];
    for(int i=0;i<ntypes;++i) Blessing[i]=0;
    nInteractingMonks = 1;
    MonksEnergy = new  int[10000];
    MonkWhoGaveEnergy = new  int[10000];
    MonkWhoGaveEnergy[0]=-1;

    NumberOfStone = -1;

    ignoreEverything=false;
    ignoreMonks=false;
    ghandarm=false;
}

Newbie::Newbie(const Newbie *m):Entity(x,y)
{
    setPixmap(m->pixmap());
    active=m->active;
    expirience=m->expirience;
    energy=m->energy;
    speed=m->speed;

    ellipse = new QGraphicsEllipseItem();
    ellipse->setRect(0,0,100,100);
    ellipse->setPen(QPen(QBrush(Draw()),2,Qt::DotLine));

    target = new QGraphicsRectItem();
    target->setRect(0,0,2,2);
    target->setPen(QPen(QBrush(Draw()),2,Qt::DotLine));
    targetx=m->targetx;
    targety=m->targety;
    achived=m->achived;
    pause=m->pause;
    //x=m->x;
    //y=m->y;

    tname=m->tname;
    name = new QGraphicsTextItem();
    name->setPlainText(tname);
    name->setDefaultTextColor(Qt::white);
    name->setFont(QFont("times",12));

    shadow = new QGraphicsEllipseItem();
    shadow->setBrush(QBrush(QColor(52,21,21)));
    shadow->setRect(m->shadow->boundingRect());
    shadow->setPen(Qt::NoPen);
    shadow->setOpacity(0.8);


    mini = new QGraphicsEllipseItem();
    mini->setRect(0,0,12.5,12.5);
    mini->setBrush(QBrush(Draw(),Qt::SolidPattern));
    mini->setOpacity(0.7);

    expirienceLine = new QGraphicsRectItem();
    expirienceLine->setBrush(QBrush(Qt::cyan,Qt::SolidPattern));

    energyLine = new QGraphicsRectItem();
    energyLine->setBrush(QBrush(Qt::green,Qt::SolidPattern));

    setLines();
    setPosition(x,y);

    ntypes = m->ntypes;
    Blessing = new int[ntypes];
    for(int i=0;i<ntypes;++i) Blessing[i]=m->Blessing[i];
    nInteractingMonks = m->nInteractingMonks;
    MonksEnergy = new  int[10000];
    MonkWhoGaveEnergy = new  int[10000];

    for(int i=0;i<nInteractingMonks;++i)
    {
        MonkWhoGaveEnergy[i]=m->MonkWhoGaveEnergy[i];
        MonksEnergy[i]=m->MonksEnergy[i];
    }

    NumberOfStone = m->NumberOfStone;

    ignoreEverything=m->ignoreEverything;
    ignoreMonks=m->ignoreMonks;
}

Newbie::Newbie():Entity(x,y)
{
    setPixmap(QPixmap(":images/Newbie.png"));
    active=true;
    expirience=0;
    energy=0;
    speed=1;

    ellipse = new QGraphicsEllipseItem();
    ellipse->setRect(0,0,100,100);
    ellipse->setPen(QPen(QBrush(Draw()),2,Qt::DotLine));

    target = new QGraphicsRectItem();
    target->setRect(0,0,2,2);
    target->setPen(QPen(QBrush(Draw()),2,Qt::DotLine));
    targetx=500;
    targety=500;
    achived=false;
    pause=true;
    //x=m->x;
    //y=m->y;

    tname="Lol3";
    name = new QGraphicsTextItem();
    name->setPlainText(tname);
    name->setDefaultTextColor(Qt::white);
    name->setFont(QFont("times",12));

    shadow = new QGraphicsEllipseItem();
    shadow->setBrush(QBrush(QColor(52,21,21)));
    shadow->setRect(0,0,58,8);
    shadow->setPen(Qt::NoPen);
    shadow->setOpacity(0.8);


    mini = new QGraphicsEllipseItem();
    mini->setRect(0,0,12.5,12.5);
    mini->setBrush(QBrush(Draw(),Qt::SolidPattern));
    mini->setOpacity(0.7);

    expirienceLine = new QGraphicsRectItem();
    expirienceLine->setBrush(QBrush(Qt::cyan,Qt::SolidPattern));

    energyLine = new QGraphicsRectItem();
    energyLine->setBrush(QBrush(Qt::green,Qt::SolidPattern));

    setLines();
    setPosition(x,y);

    ntypes = 1;
    Blessing = new int[ntypes];
    MonksEnergy = new  int[10000];
    MonkWhoGaveEnergy = new  int[10000];


    for(int i=0;i<nInteractingMonks;++i)
    {
        MonkWhoGaveEnergy[i]=-1;
        MonksEnergy[i]=0;
    }

    NumberOfStone = 0;

    ignoreEverything=false;
    ignoreMonks=false;
}

Newbie &Newbie::operator=(Newbie &m)
{
    if(this==&m) return (*this);
    setPixmap(m.pixmap());
    active=m.active;
    expirience=m.expirience;
    energy=m.energy;
    speed=m.speed;
    x=m.x;
    y=m.y;

    ellipse->setPen(QPen(QBrush(m.Draw()),2,Qt::DotLine));

    target->setPen(QPen(QBrush(Draw()),2,Qt::DotLine));
    targetx=m.targetx;
    targety=m.targety;
    achived=m.achived;
    pause=m.pause;

    tname=m.tname;
    name->setPlainText(tname);

    setLines();
    setPosition(x,y);

    ntypes = m.ntypes;
    delete []Blessing;

    Blessing = new int[ntypes];
    for(int i=0;i<ntypes;++i) Blessing[i]=m.Blessing[i];
    nInteractingMonks = m.nInteractingMonks;
    delete []MonksEnergy;
    delete []MonkWhoGaveEnergy;
    MonksEnergy = new  int[10000];
    MonkWhoGaveEnergy = new  int[10000];

    for(int i=0;i<nInteractingMonks;++i)
    {
        MonkWhoGaveEnergy[i]=m.MonkWhoGaveEnergy[i];
        MonksEnergy[i]=m.MonksEnergy[i];
    }

    NumberOfStone = m.NumberOfStone;

    ignoreEverything=m.ignoreEverything;
    ignoreMonks=m.ignoreMonks;
    return *this;
}

bool Newbie::operator<(Newbie &arg)
{
    if (energy<arg.energy) return true;
    return false;
}

void Newbie::operator++()
{
    energy++;
}



void Newbie::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        active=!active;
        if(active)
        {
            view->scene->addItem(ellipse);
            ellipse->setPos(x,y);
        }
        else view->scene->removeItem(ellipse);
    }
    else
    {
        emit printInfo(QString("\n\r-----------------\n")+tname);
        emit printInfo(QString("Досвід: ")+QString::number(expirience));
        emit printInfo(QString("Енергія: ") + QString::number(energy));
        emit printInfo(QString("Тип: ")+QString::number(getType()));
        if(ignoreMonks)emit printInfo(QString("Не взаємодіє з іншими монахами"));
        if(ignoreEverything)emit printInfo(QString("Медитує"));
        if(pause)emit printInfo(QString("Зупинений"));
        emit printInfo(QString("Благословення:"));
        emit printInfo(QString("Звичайний камень: ")+ QString::number(Blessing[0]));
        emit printInfo(QString("Містичний камень: ")+ QString::number(Blessing[1]));
        emit printInfo(QString("Містичний валун: ")+ QString::number(Blessing[2]));
        emit printInfo(QString("-----------------\n\r"));



    }
}

int Newbie::getArrSize()
{
    return arr_size;
}

QColor Newbie::Draw()
{
    QColor color;
    double a = expirience / 12.5;
    int b = (int)a;
    switch (b)
    {
    case 0:
    {
        htmlColor="#CC0000";
        color.setRgb(204,0,0);
        return color;
    }
    case 1:
    {
        htmlColor="#FF8000";
        color.setRgb(255,128,0);
        return color;
    }
    case 2:
    {
        htmlColor="#FFFF00";
        color.setRgb(255,255,0);
        return color;
    }
    default:
        color.setRgb(0,0,0);
        return color;
    }
}


Newbie::~Newbie()
{
    //qDebug()<<"destructor of Newbie";
    delete name;
    delete mini;
    delete target;
    delete ellipse;
    delete shadow;
    delete []Blessing;
    delete []MonksEnergy;
    delete []MonkWhoGaveEnergy;
}

int Newbie::getSpeed()
{
    return speed;
}

void Newbie::setPosition(qreal x, qreal y)
{
    setPos(x,y);
    ellipse->setPos(x,y);
    name->setPos(x+15, y-30);
    mini->setPos(x/8,y/8);
    energyLine->setPos(x+15,y-34);
    expirienceLine->setPos(x+15,y-40);
    shadow->setPos(x+20,y+90);
    setZValue(y+100);
    name->setZValue(y+100);
    ellipse->setZValue(y+100);
    energyLine->setZValue(y+100);
    expirienceLine->setZValue(y+100);
    shadow->setZValue(y+90);
}

void Newbie::setLines()
{
    qreal e=70*expirience/100,m=70*energy/3;
    expirienceLine->setRect(0,0,e,2);
    energyLine->setRect(0,0,m,2);
    ellipse->setPen(QPen(QBrush(Draw()),2,Qt::DotLine));
    mini->setBrush(QBrush(Draw(),Qt::SolidPattern));
    if(getType()!=getTrueType()) emit changeLevel(this);

}

int Newbie::getType()
{
    return 1;
}

int Newbie::getTrueType()
{
    double a = expirience / 12.5;
    a=int(a);
    if(a>=0 && a<=2) return 1;
    if(a>2 && a<=5) return 2;
    if(a>5 && a<=7) return 3;
    //return 1;
}

void Newbie::addEnergy(double n, int m)
{
    int val;//на скільки зменшиться або збільшиться енергія монаха, який передавав енергію

    if(energy+n<0) { val =-energy; energy=0;}
    else
    if(energy+n>3) {val=getType()*2+1 -energy; energy=getType()*2+1;}
    else {val=n;energy+=n;}

    setLines();


    for(int i=0;i<nInteractingMonks;++i)
    {
        if(MonkWhoGaveEnergy[i]==-1 || MonkWhoGaveEnergy[i]==NULL)
        {
            MonkWhoGaveEnergy[i]=m;
            MonksEnergy[i]=val;
            return;
        }
    }
    MonkWhoGaveEnergy[nInteractingMonks]=m;
    MonksEnergy[nInteractingMonks]=val;
    nInteractingMonks++;

}

void Newbie::Karma(int n)
{
    if(expirience+n<0) this->expirience=0;
    else if(expirience+n>100) expirience=100;
    else expirience+=n;
    setLines();
    //qDebug()<<tname<<" karma "<<n<<" exp:"<<expirience;
}

void Newbie::OpacityEffect(int value)
{
    double begin,end;
    switch (value)
    {
    case 0:
        begin=1;
        end=0.4;
        break;
    case 1:
        begin=0.4;
        end=1;
        break;
    }

//        QTimer *timer1= new QTimer();
//        timer1->start(50);
//        while(begin!=end)
//        {
//            if(timer1->)
//        }

}

void Newbie::Print()
{
    qDebug()<<tname;
}


