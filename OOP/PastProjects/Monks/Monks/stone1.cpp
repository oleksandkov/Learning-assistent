#include "stone1.h"

extern Newbie **monk;

Stone1::Stone1(int x, int y):Entity(x,y)
{
    setPixmap(QPixmap(":images/Stone1.png"));
    setPos(x,y);
    setZValue(y+270);

    minirect = new QGraphicsRectItem();
    minirect->setRect(0,0,37.5,37.5);
    QBrush brush(Draw());
    brush.setStyle(Qt::SolidPattern);
    minirect->setBrush(brush);
    minirect->setPos(x/8,y/8);
    minirect->setOpacity(0.5);

    shadow = new QGraphicsEllipseItem();
    shadow->setBrush(QBrush(QColor(52,21,21)));
    shadow->setRect(0,0,178,37);
    shadow->setPos(x+72,y+220);
    shadow->setZValue(y+260);
    shadow->setPen(Qt::NoPen);
    shadow->setOpacity(0.87);

    sittingMonk = new Newbie*[3]();
    for(int i=0;i<3;++i) sittingMonk[i]=NULL;
    nMonks = 3-getType()+1;

    kilkist=0;
    numberOfMonks = new QGraphicsTextItem();
    numberOfMonks->setPlainText(QString("Кількість монахів: ")+ QString::number(kilkist));
    numberOfMonks->setDefaultTextColor(Qt::white);
    numberOfMonks->setFont(QFont("times",12));
    numberOfMonks->setPos(x+75,y+260);



    timer = new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(ReturnsMonks()));

}

Stone1::~Stone1()
{
    delete []sittingMonk;
    delete minirect;
    delete shadow;
}

int Stone1::getType()
{
    return 1;
}

void Stone1::addMonk(Newbie *p)
{
    p->Blessing[getType()-1]--;
    p->ignoreEverything=true;
    for(int i=0;i<nMonks;++i)
    {
        if(sittingMonk[i]==NULL)
        {
            sittingMonk[i]=p;
            kilkist++;
            sittingMonk[i]->setPosition(sittingMonk[i]->x=x+100+i*4,sittingMonk[i]->y=y+100-i*4);
            sittingMonk[i]->setZValue(y+280);
            sittingMonk[i]->name->setZValue(y+280);
            sittingMonk[i]->energyLine->setZValue(y+280);
            sittingMonk[i]->expirienceLine->setZValue(y+280);
            break;
        }
    }
    numberOfMonks->setPlainText(QString("Кількість монахів: ")+ QString::number(kilkist));
    if(kilkist>=nMonks )
    {
        for(int i=0;i<nMonks;++i)
        {
            if(sittingMonk[i]!=NULL)
            {
                for(int j=0;j<sittingMonk[i]->nInteractingMonks;++j)
                {
                    if(sittingMonk[i]->MonkWhoGaveEnergy[j]!=-1
                            && sittingMonk[i]->MonksEnergy[j]!=0
                            && monk[sittingMonk[i]->MonkWhoGaveEnergy[j]]!=NULL)
                        monk[sittingMonk[i]->MonkWhoGaveEnergy[j]]->Karma(sittingMonk[i]->MonksEnergy[j]);
                    sittingMonk[i]->MonkWhoGaveEnergy[j]=-1;
                }

                sittingMonk[i]->expirience+=sittingMonk[i]->energy;
                sittingMonk[i]->energy=0;
                sittingMonk[i]->setLines();
            }
        }
    }
}

void Stone1::returnMonks()
{
    timer->start(getType()*5000);
}

QColor Stone1::Draw()
{
    QColor color;
    color.setRgb(66, 1, 1);
    return color;
}

void Stone1::ReturnsMonks()
{
    timer->stop();
    if(sittingMonk[0]!=NULL) sittingMonk[0]->setPosition(sittingMonk[0]->x=x+300,sittingMonk[0]->y=y+100);
    if(sittingMonk[1]!=NULL) sittingMonk[1]->setPosition(sittingMonk[1]->x=x+100,sittingMonk[1]->y=y-100);
    if(sittingMonk[2]!=NULL) sittingMonk[2]->setPosition(sittingMonk[2]->x=x-100,sittingMonk[2]->y=y+100);
    for(int i=0;i<nMonks;++i)
    {
        if(sittingMonk[i]!=NULL)
        {
            sittingMonk[i]->ignoreEverything=false;
            sittingMonk[i]=NULL;
        }
    }
    kilkist=0;
    numberOfMonks->setPlainText(QString("Кількість монахів: ")+ QString::number(kilkist));
}
