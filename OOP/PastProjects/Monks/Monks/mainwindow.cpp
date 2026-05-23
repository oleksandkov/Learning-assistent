#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainview.h"

extern MainView *view;

#define MONK_ARR 50
#define STONE_ARR 10

Stone1 **stone;
Newbie **monk;
QString text;
QString SetName();
QString Saying();

Newbie *mk1,*mk2,*mk3,*mk4;
int typ1=0,typ2=100,typ3=0;

void Some_Function(Newbie *n)
{
    emit n->printInfo("1 випадок копіюючого конструктора.");
}

Newbie *Create_Something(QString st)
{
    Newbie *temp = new Newbie(0,0,0,0,0,st);
    return temp;
}

//QDebug &operator<<( QDebug out, Newbie &m )
//{
//    out << m.tname;
//    return out.maybeSpace();

//}

int Newbie::arr_size=50;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //set window attributes
    ui->setupUi(this);
    view = new MainView();
    setCentralWidget(view);
    setWindowTitle("Monks");
    setWindowIcon(QIcon(":/images/bm.ico"));

    //set map attributes
    amount = new QGraphicsTextItem(QString("Всього монахів: "));
    amount->setFont(QFont("times",14));
    amount->setZValue(2400);
    amount->setPos(view->mapToScene(5,25));
    amount->setDefaultTextColor(QColor(59,21,21));
    view->scene->addItem(amount);
    coordinates = new QGraphicsTextItem(QString("Координати 0 0"));
    coordinates->setFont(QFont("times",14));
    coordinates->setZValue(2400);
    coordinates->setPos(view->mapToScene(5,5));
    coordinates->setDefaultTextColor(QColor(59,21,21));
    view->scene->addItem(coordinates);
    textTimer = new QTimer();
    textTimer->start(8);
    connect(textTimer,SIGNAL(timeout()),this,SLOT(setText()));
    minimap = new MiniMap();
    minimap->setWindowFlags(Qt::WindowStaysOnTopHint);
    chat = new Chat();
    chat->setWindowFlags(Qt::WindowStaysOnTopHint);


    //add items
    stone = new Stone1*[STONE_ARR]();
    monk = new Newbie*[MONK_ARR]();
    for(int i=0;i<STONE_ARR;++i)
        InsertStone(qrand()%3+1,(qrand()%3200)/300 *300, (qrand()%2400)/300 *300);

    //movement of monks
    QTimer *movementTimer = new QTimer();
    connect(movementTimer,SIGNAL(timeout()),this,SLOT(move()));
    movementTimer->start(40);



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int x,y;
    int kl=0;
    for(int i=0;i<Newbie::getArrSize();++i)
    {
        if(monk[i]!=NULL)
        {
            if(monk[i]->active)
            {
                switch(event->key())
                {
                case Qt::Key_D:
                    if(monk[i]->x <3100)
                    {
                        moveMonk(i,1,0);
                    }
                break;
                case Qt::Key_A:
                    if(monk[i]->x >0)
                    {
                        moveMonk(i,-1,0);
                    }
                break;
                case Qt::Key_W:
                    if(monk[i]->y > 0)
                    {
                        moveMonk(i,0,-1);
                    }
                break;
                case Qt::Key_S:
                    if(monk[i]->y <2300)
                    {
                        moveMonk(i,0,1);
                    }
                break;
                case Qt::Key_Delete:

                    text+="<span style=\" font-size:12pt; color:#FF0c32;\" >*****";
                    text+=monk[i]->tname;
                    text+= " завершив своє перебування на цій землі і переродився в кращі світи.*****\n\r</span>";
                    chat->addText(text);
                    text="";
                    RemoveMonk(i);
                break;
                case Qt::Key_Escape:
                    monk[i]->active=!monk[i]->active;
                    view->scene->removeItem(monk[i]->ellipse);
                    if(monk[i]->ghandarm) monk[i]->ghandarm=false;
                break;
                case Qt::Key_X:
                    monk[i]->targetx=300;
                    monk[i]->targety=400;
                    monk[i]->target->setPos(monk[i]->targetx,monk[i]->targety);
                    monk[i]->achived=false;
                    monk[i]->ignoreMonks=true;
                break;
                case Qt::Key_P:
                    monk[i]->pause=!monk[i]->pause;
                break;
                case Qt::Key_M:
                    monk[i]->ignoreMonks=!monk[i]->ignoreMonks;
                break;
                case Qt::Key_Z:
                    kl++;
                    if(kl==1) {monk[i]->ghandarm=true; monk[i]->ignoreMonks=true;}
                break;
                }

            }
        }
    }

    switch(event->key())
    {
    case Qt::Key_Insert:
        for(int i=0;i<MONK_ARR;++i)
        {
            if(monk[i]==NULL)
            {

                InsertMonkDialog *pInputDialog = new InsertMonkDialog();
                if (pInputDialog->exec() == QDialog::Accepted)
                {
                    x=view->horizontalScrollBar()->value()+rand()%700/10*10;
                    y=view->verticalScrollBar()->value()+rand()%500/10*10;
                    InsertMonk(pInputDialog->getType(),x,y,false,pInputDialog->getExpirience(),
                               pInputDialog->getEnergy(),pInputDialog->getName());
                    if(pInputDialog->getPause()) monk[i]->pause=true;
                }
                delete pInputDialog;
                return;
            }
        }
        break;
    case Qt::Key_1:

        x=view->horizontalScrollBar()->value()+rand()%700/10*10;
        y=view->verticalScrollBar()->value()+rand()%500/10*10;
        InsertMonk(1, x, y, false, 13*(qrand()%3),3.0, SetName());
            break;
    case Qt::Key_2:

        x=view->horizontalScrollBar()->value()+rand()%700/10*10;
        y=view->verticalScrollBar()->value()+rand()%500/10*10;
        InsertMonk(2, x, y, false, 13*(qrand()%3+3),0.0, SetName());
            break;
    case Qt::Key_3:

        x=view->horizontalScrollBar()->value()+rand()%700/10*10;
        y=view->verticalScrollBar()->value()+rand()%500/10*10;
        InsertMonk(3, x, y, false, 12.5*(qrand()%2+6),0.0, SetName());
            break;
    case Qt::Key_I:   //інверсія виділення
        for(int i=0;i<MONK_ARR;++i)
        {
            if(monk[i]!=NULL)
            {
                monk[i]->active=!monk[i]->active;
                if(monk[i]->active)
                {
                    view->scene->addItem(monk[i]->ellipse);
                    monk[i]->ellipse->setPos(monk[i]->x,monk[i]->y);
                }
                else view->scene->removeItem(monk[i]->ellipse);
            }
        }
        break;
    case Qt::Key_C:  //змінити напрямок
        for(int i=0;i<MONK_ARR;++i)
        {
            if(monk[i]!=NULL)
            {
                monk[i]->achived=!monk[i]->achived;
            }
        }
        break;
    case Qt::Key_E:
        mk1 = new Newbie(50,50,true,40,3,"Lol1");
        view->scene->addItem(mk1);
        view->scene->addItem(mk1->name);
        if(mk1->active) view->scene->addItem(mk1->ellipse);
        connect(mk1,SIGNAL(printInfo(QString)),chat,SLOT(addText(QString)));


        mk2 = new Newbie(150,50,true,40,3,"Lol2");
        view->scene->addItem(mk2);
        view->scene->addItem(mk2->name);
        if(mk2->active) view->scene->addItem(mk2->ellipse);

        mk3 = new Newbie(mk1); //3 випадок копіюючого конструктора
        view->scene->addItem(mk3);
        view->scene->addItem(mk3->name);
        if(mk3->active) view->scene->addItem(mk3->ellipse);
        mk3->setPosition(250,50);
        break;
    case Qt::Key_R:
        Some_Function(mk1); //1 випадок копіюючого конструктора.
        (Create_Something("2 випадок копіюючого конструктора."))->Print();


        mk4 = new Newbie();
        view->scene->addItem(mk4);
        view->scene->addItem(mk4->name);
        if(mk4->active) view->scene->addItem(mk4->ellipse);
        *mk4=*mk3;
        mk4->setPosition(350,50);


       // Newbie n1,n2;
        ++(*mk4);
        if((*mk3)<(*mk4)) qDebug()<<"true1"; else qDebug()<<"false1";
        if((*mk2)==(*mk3))qDebug()<<"true2"; else qDebug()<<"false2";

         //qDebug()<<mk2;


        break;
    case Qt::Key_T:
        delete mk1;
        delete mk2;
        delete mk3;
        delete mk4;

        break;

    }
}

void MainWindow::RemoveMonk(int i)
{
    if(monk[i]==NULL) return;

    if(monk[i]->NumberOfStone!= -1)
        for(int j=0;j<stone[monk[i]->NumberOfStone]->nMonks;++j)
            if(typeid(monk[i])==typeid(stone[monk[i]->NumberOfStone]->sittingMonk[j]))
                stone[monk[i]->NumberOfStone]->sittingMonk[j]=NULL;

    view->scene->removeItem(monk[i]);
    if(monk[i]->active)view->scene->removeItem(monk[i]->ellipse);
    view->scene->removeItem(monk[i]->name);
    view->scene->removeItem(monk[i]->target);
    view->scene->removeItem(monk[i]->expirienceLine);
    view->scene->removeItem(monk[i]->energyLine);
    view->scene->removeItem(monk[i]->shadow);
    minimap->scene->removeItem(monk[i]->mini);


    delete monk[i];
    monk[i]=NULL;
}

void MainWindow::RemoveStone(int i)
{
    if(stone[i]==NULL) return;
    view->scene->removeItem(stone[i]);
    view->scene->removeItem(stone[i]->numberOfMonks);
    view->scene->removeItem(stone[i]->shadow);
    minimap->scene->removeItem(stone[i]->minirect);

    delete stone[i];
    stone[i]=NULL;
}

void MainWindow::InsertMonk(int type, int x, int y, bool active, double expirience, double energy, QString name)
{
    for(int i=0;i<MONK_ARR;++i)
    {
        if(monk[i]==NULL)
        {
            switch (type) {
            case 1:
                monk[i]= new Newbie(x,y, active, expirience, energy, name);
                break;
            case 2:
                monk[i]= new Expirienced(x,y, active, expirience, energy, name);
                break;
            case 3:
                monk[i]= new Guru(x,y, active, expirience, energy, name);
                break;
            }

            view->scene->addItem(monk[i]);
            view->scene->addItem(monk[i]->name);
            view->scene->addItem(monk[i]->target);
            view->scene->addItem(monk[i]->expirienceLine);\
            view->scene->addItem(monk[i]->energyLine);
            view->scene->addItem(monk[i]->shadow);
            minimap->scene->addItem(monk[i]->mini);
            if(active) view->scene->addItem(monk[i]->ellipse);

            monk[i]->setZValue(monk[i]->y+100);
            monk[i]->name->setZValue(monk[i]->y+100);
            monk[i]->ellipse->setZValue(monk[i]->y+100);

            connect(monk[i],SIGNAL(changeLevel(Newbie*)),this,SLOT(ChangeLevel(Newbie*)));
            connect(monk[i],SIGNAL(printInfo(QString)),chat,SLOT(addText(QString)));

            switch (type) {
            case 1:
                text+="<span style=\" font-size:12pt; color:#000000;\" >*****Newbie ";
                break;
            case 2:
                text+="<span style=\" font-size:12pt; color:#000000;\" >*****Expirienced ";
                break;
            case 3:
                text+="<span style=\" font-size:12pt; color:#000000;\" >*****Guru ";
                break;
            }

            text+=monk[i]->tname;
            text+= ". *****\n\r</span>";
            chat->addText(text);
            text="";
            return;
        }
    }
}

void MainWindow::InsertStone(int type, int x, int y)
{
    for(int i=0;i<STONE_ARR;++i)
    {
        if(stone[i]==NULL)
        {
            switch (type) {
            case 1:
                stone[i]= new Stone1(x,y);
                break;
            case 2:
                stone[i]= new Stone2(x,y);
                break;
            case 3:
                stone[i]= new Stone3(x,y);
                break;
            }

            view->scene->addItem(stone[i]);
            view->scene->addItem(stone[i]->numberOfMonks);
            view->scene->addItem(stone[i]->shadow);
            minimap->scene->addItem(stone[i]->minirect);

            return;
        }
    }
}

void MainWindow::MonksInterakting(int m1, int m2)
{
    //qDebug()<<monk[m1]->tname<<" interacts with "<<monk[m2]->tname;
    int y,x1,x2;
    y=max(monk[m1]->y,monk[m2]->y) - abs(monk[m1]->y - monk[m2]->y)/2;
    x1=x2=max(monk[m1]->x,monk[m2]->x) - abs(monk[m1]->x - monk[m2]->x)/2;
    x1-=51;
    x2+=51;
    monk[m1]->x=x1;
    monk[m2]->x=x2;
    monk[m1]->y=monk[m2]->y=y;
    monk[m1]->setPosition(monk[m1]->x,monk[m1]->y);
    monk[m2]->setPosition(monk[m2]->x,monk[m2]->y);
    monk[m1]->achived=monk[m2]->achived=true;
    if(monk[m1]->active)
    {
        InteractionWithMonkDialog *pInputDialog = new InteractionWithMonkDialog(0,m1,m2);
        if (pInputDialog->exec() == QDialog::Accepted)
        {
            if(pInputDialog->getBlessing()) //Якщо когось благословляєш, то тебе благословляють у відповідь
            {
                monk[m1]->Blessing[monk[m2]->getType()-1]++;
                monk[m2]->Blessing[monk[m1]->getType()-1]++;
            }
            monk[m2]->addEnergy(pInputDialog->getEnergy(),m1);

        }
        delete pInputDialog;
    }
    else
    {
        if(qrand()&1)
        {
            if(qrand()&1)
            {
                monk[m2]->Blessing[monk[m1]->getType()-1]++;
            }
            monk[m2]->addEnergy(qrand()%14 -7,m1);
        }
    }

    if(monk[m2]->active)
    {
        InteractionWithMonkDialog *pInputDialog = new InteractionWithMonkDialog(0,m2,m1);
        if (pInputDialog->exec() == QDialog::Accepted)
        {
            if(pInputDialog->getBlessing()) //Якщо когось благословляєш, то тебе благословляють у відповідь
            {
                monk[m1]->Blessing[monk[m2]->getType()-1]++;
                monk[m2]->Blessing[monk[m1]->getType()-1]++;
            }
            monk[m1]->addEnergy(pInputDialog->getEnergy(),m2);

        }
        delete pInputDialog;
    }
    else
    {
        if(qrand()&1)
        {
            if(qrand()&1)
            {
                monk[m1]->Blessing[monk[m2]->getType()-1]++;
            }
            monk[m1]->addEnergy(qrand()%14 -7,m2);
        }
    }
}

void MainWindow::MonkInteractsWithStone(int m, int s)
{
    if(monk[m]->Blessing[stone[s]->getType()-1]>0 && !monk[m]->ignoreEverything && monk[m]->energy!=0)
    {
        for(int i=0;i<stone[s]->nMonks;++i)
        {
            if(stone[s]->sittingMonk[i]==NULL)
            {
                if(!monk[m]->ignoreEverything)
                {
                    monk[m]->NumberOfStone=s;
                    if(monk[m]->active)
                    {
                        InteractionWithStoneDialog *pInputDialog = new InteractionWithStoneDialog();
                        if (pInputDialog->exec() == QDialog::Accepted)
                        {

                            stone[s]->addMonk(monk[m]);
                        }
                        else monk[m]->setPosition(monk[m]->x=stone[s]->x+100,monk[m]->y=stone[s]->y+300);
                        delete pInputDialog;
                    }
                    else stone[s]->addMonk(monk[m]);
                }
                if(stone[s]->kilkist>=stone[s]->nMonks)
                {
                    //stone[s]->numberOfMonks->setPlainText();
                    stone[s]->returnMonks();
                    for(int j=0;j<stone[s]->nMonks;++j)
                    {

                    }
                }
                return;
            }
        }
    }
}

void MainWindow::setText()
{
    static int hScrVal = 0, vScrVal = 0, spAmount = 0;
    //detect scrolling
    if(hScrVal != view->horizontalScrollBar()->value() ||
            vScrVal != view->verticalScrollBar()->value())
    {
        amount->setPos(view->mapToScene(5,25));
        coordinates->setPos(view->mapToScene(5,5));
        coordinates->setPlainText(QString("Координати: ")+
                                    QString::number(view->horizontalScrollBar()->value())+
                                    QString(" ")+
                                    QString::number(view->verticalScrollBar()->value()));
        hScrVal = view->horizontalScrollBar()->value();
        vScrVal = view->verticalScrollBar()->value();
    }

    int k=0;
    for(int i=0;i<MONK_ARR;++i)
        if(monk[i]!=NULL) k++;

    //detect amount
    if(spAmount != k)
    {
        amount->setPlainText(QString("Всього монахів: ")+
                             QString::number(k));
        spAmount = k;
    }
    view->scene->update();
}

void MainWindow::move()
{
    //автоматичний рух
    for(int i=0;i<MONK_ARR;++i)
    {
        if(monk[i]!=NULL )
        {
            if(!monk[i]->pause && !monk[i]->active)
            {
                if(!monk[i]->achived)
                {
                    switch (monk[i]->targetx & 1)
                    {
                    case 1:
                        if(monk[i]->x <= monk[i]->targetx && monk[i]->x+100 >= monk[i]->targetx)
                            if(monk[i]->y > monk[i]->targety)
                            {
                                moveMonk(i,0,-1);
                                if(monk[i]->x <= monk[i]->targetx
                                        && monk[i]->x+100 >= monk[i]->targetx
                                        && monk[i]->y <= monk[i]->targety
                                        && monk[i]->y+100 > monk[i]->targety) monk[i]->achived=true;
                            }
                            else
                            {
                                moveMonk(i,0,1);
                                if(monk[i]->x <= monk[i]->targetx
                                        && monk[i]->x+100 > monk[i]->targetx
                                        && monk[i]->y <= monk[i]->targety
                                        && monk[i]->y+100 > monk[i]->targety) monk[i]->achived=true;
                            }
                        else
                            if(monk[i]->x > monk[i]->targetx)
                            {
                                moveMonk(i,-1,0);
                                if(monk[i]->x <= monk[i]->targetx
                                        && monk[i]->x+100 > monk[i]->targetx
                                        && monk[i]->y <= monk[i]->targety
                                        && monk[i]->y+100 > monk[i]->targety) monk[i]->achived=true;
                            }
                            else
                            {
                                moveMonk(i,1,0);
                                if(monk[i]->x <= monk[i]->targetx
                                        && monk[i]->x+100 > monk[i]->targetx
                                        && monk[i]->y <= monk[i]->targety
                                        && monk[i]->y+100 > monk[i]->targety) monk[i]->achived=true;
                            }

                        break;

                    case 0:
                        if(monk[i]->y <= monk[i]->targety && monk[i]->y+100 > monk[i]->targety)
                            if(monk[i]->x > monk[i]->targetx)
                            {
                                moveMonk(i,-1,0);
                                if(monk[i]->x <= monk[i]->targetx
                                        && monk[i]->x+100 > monk[i]->targetx
                                        && monk[i]->y <= monk[i]->targety
                                        && monk[i]->y+100 >= monk[i]->targety) monk[i]->achived=true;
                            }
                            else
                            {
                                moveMonk(i,1,0);
                                if(monk[i]->x <= monk[i]->targetx
                                        && monk[i]->x+100 > monk[i]->targetx
                                        && monk[i]->y <= monk[i]->targety
                                        && monk[i]->y+100 > monk[i]->targety) monk[i]->achived=true;
                            }
                        else
                            if(monk[i]->y > monk[i]->targety)
                            {
                                moveMonk(i,0,-1);
                                if(monk[i]->x <= monk[i]->targetx
                                        && monk[i]->x+100 > monk[i]->targetx
                                        && monk[i]->y <= monk[i]->targety
                                        && monk[i]->y+100 > monk[i]->targety) monk[i]->achived=true;
                            }
                            else
                            {
                                moveMonk(i,0,1);
                                if(monk[i]->x <= monk[i]->targetx
                                        && monk[i]->x+100 > monk[i]->targetx
                                        && monk[i]->y <= monk[i]->targety
                                        && monk[i]->y+100 > monk[i]->targety) monk[i]->achived=true;


                            }

                        break;
                    default:
                        break;
                    }


//                    if(qrand()%100==5)
//                        qDebug()<<monk[i]->x<<" "<<monk[i]->y<<"    "<<monk[i]->targetx<<" "<<monk[i]->targety;
                }
                else
                {
                    if(monk[i]->targetx==300 && monk[i]->targety==400) monk[i]->pause=true;
                    monk[i]->targetx=qrand()%3000+monk[i]->x%100;
                    monk[i]->targety=qrand()%2200+monk[i]->x%100;
                    monk[i]->target->setPos(monk[i]->targetx,monk[i]->targety);
                    monk[i]->achived=false;


                }
            }

            //взаємодія
            if(!monk[i]->ignoreEverything)
            {
                QList<QGraphicsItem*> colliding_items= monk[i]->collidingItems();
                for(int j=0,n=colliding_items.size();j<n;++j)
                {
                    for(int i1=0;i1<MONK_ARR;++i1)
                    {
                        if(colliding_items[j]==monk[i1] && !monk[i1]->ignoreEverything && !monk[i]->ignoreMonks && !monk[i1]->ignoreMonks )
                        {
                            MonksInterakting(i,i1);
                            break;
                        }
                        if(colliding_items[j]==monk[i1] && monk[i]->ghandarm)
                        {
                            qDebug()<<"lol";
                            switch(monk[i1]->getType())
                            {
                            case 1:
                                monk[i1]->x=typ1;
                                monk[i1]->y=0;
                                typ1+=100;
                                break;
                            case 2:
                                monk[i1]->x=0;
                                monk[i1]->y=typ2;
                                typ2+=100;
                                break;
                            case 3:
                                monk[i1]->x=typ3;
                                monk[i1]->y=2300;
                                typ3+=100;
                                break;
                            }

                            monk[i1]->setPosition(monk[i1]->x,monk[i1]->y);
                            monk[i1]->ignoreMonks=true;
                            monk[i1]->pause=true;
                        }
                    }
                    for(int i1=0;i1<STONE_ARR;++i1)
                    {
                        if(colliding_items[j]==stone[i1] && !monk[i]->ghandarm)
                        {
                            MonkInteractsWithStone(i,i1);
                            break;
                        }
                    }
                }
            }
            //кінець(взаємодія)

        }
    }
    //кінець(автоматичний рух)

    //Інформаційне табло

    if(qrand()%30==0 && monk[0]!=NULL)
    {
        QString text;
        int l=qrand()%MONK_ARR;
        while(monk[l]==NULL)
        {
            l=qrand()%MONK_ARR;
        }
        text+="<span style=\" font-size:8pt; color:";
        text+=monk[l]->htmlColor+";\" >";
        text+=monk[l]->tname+": ";
        text+= Saying();
        text+="\n\r</span>";
        chat->addText(text);
    }
    //кінець(Інформаційне табло)

}

void MainWindow::moveMonk(int i,int x, int y)
{
    if(!monk[i]->ignoreEverything)
    {
        monk[i]->x+= x*monk[i]->getSpeed();
        monk[i]->y+= y*monk[i]->getSpeed();
        monk[i]->setPosition(monk[i]->x,monk[i]->y);
    }
}

QString SetName()
{
    QString ar[15]={"Veij","Guang","Jen","Ji","Li","Cheng","Arjuna",
                   "Brama","Ganesha", "Daijarma", "Indra","Lakshman","Manu",
                   "Rajesh","Jei"};
    return ar[rand()%15];
}

QString Saying()
{
    QString ar[19]={"Екзистенціалізм - це гуманізм.",
                    "Омм...",
                    "Нароод! Го реінкарніруватись на іншу землю. Хто зі мною??",
                    "Ха-ха. Ви всі дурні. Цей світ - це ілюзія вашого розуму. Ніщо не вічне.",
                    "Дао, виражене словами, не є справжнє Дао.",
                    "Хто знає, той не говорить, хто говорить, той - не знає.",
                    "Казала мені мама, не йти в будисти. Тут всі поїхавші(",
                    "Соц. мережа «ВСансарі» упала( Ніхто не знає, що сталось?",
                    "В астралі сьогодні погодка не дуже...",
                    "Я люблю вас всіх ♥.",
                    "З часом ти зрозумієш, що немає ніякого часу. Але для цього потрібен час.",
                    "Не вірте вашим почуттям. Це все ілюзія. Для вашого же блага. Просто вийдіть за межі вашого розуму і тоді ви все зрозумієте...",
                    "Всім добрий дзен.",
                    "Всім Омм...",
                    "Хорошому будисту - велика карма :))",
                    "Я згоден.",
                    "",
                    "",
                    ""
                   };
    return ar[rand()%19];
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Завантаження"), "",
        tr("Text Files (*.txt)"));

    if (fileName != "")
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
            return;
        }
          for (int i = 0; i<MONK_ARR; i++)
              RemoveMonk(i);
          for(int i=0; i<STONE_ARR;++i)
              RemoveStone(i);

       QTextStream in(&file);
       /*
        do
       {
       }while(!in.atEnd());
       */
       int kmonk,kstone;
       in>>kmonk;
       for(int i=0;i<kmonk;++i)
       {
           int type,x,y;
           int active;
           double expirience,energy;
           QString name;
           in>>type>>x>>y;
           in>>active;
           in>>expirience;
           in>>energy;
           in>>name;
           InsertMonk(type,x,y,active,expirience,energy,name);
       }

       in>>kstone;
       for(int i=0;i<kstone;++i)
       {
           int type,x,y;
           in>>type>>x>>y;
           InsertStone(type,x,y);
       }


        in.flush();
        file.close();
        //ui->textBrowser->clear();
    }
}

void MainWindow::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Зберегти"), "",
    tr("Text Files (*.txt)"));
    //fileName+=".txt";

    if (fileName != "")
    {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            // error message
            QMessageBox::critical(this, tr("Error"), tr("Could not save file"));
        }
        else
        {
            int kmonk=0,kstone=0;
            QTextStream stream(&file);
            for (int i=0;i<MONK_ARR;++i)
            {
                if(monk[i]!=NULL) kmonk++;
                if(stone[i]!=NULL) kstone++;
            }
            stream<<kmonk<<"\r\n";
            for(int i=0;i<MONK_ARR;++i)
            {
                if(monk[i]!=NULL)
                {
                    Guru *pg = dynamic_cast<Guru *>(monk[i]);
                    if( pg != NULL )
                    {
                        stream<<3<<"\r\n";
                    }
                    else
                    {
                        Expirienced *pe = dynamic_cast<Expirienced *>(monk[i]);
                        if( pe != NULL )
                        {
                            stream<<2<<"\r\n";
                        }
                        else
                        {
                            Newbie *pn = dynamic_cast<Newbie *>(monk[i]);
                            if(pn != NULL)
                            {
                                stream<<1<<"\r\n";
                            }
                        }
                    }

                    stream<<monk[i]->x<<" "<<monk[i]->y<<"\r\n";
                    stream<<monk[i]->active<<"\r\n";
                    stream<<monk[i]->expirience<<"\r\n";
                    stream<<monk[i]->energy<<"\r\n";
                    stream<<monk[i]->tname<<"\r\n";
                }
            }

            stream<<kstone<<"\r\n";
            for(int i=0;i<STONE_ARR;++i)
            {
                if(stone[i]!=NULL)
                {
                    Stone3 *pg = dynamic_cast<Stone3 *>(stone[i]);
                    if( pg != NULL )
                    {
                        stream<<3<<"\r\n";
                    }
                    else
                    {
                        Stone2 *pe = dynamic_cast<Stone2 *>(stone[i]);
                        if( pe != NULL )
                        {
                            stream<<2<<"\r\n";
                        }
                        else
                        {
                            Stone1 *pn = dynamic_cast<Stone1 *>(stone[i]);
                            if(pn != NULL)
                            {
                                stream<<1<<"\r\n";
                            }
                        }
                    }

                    stream<<stone[i]->x<<" "<<stone[i]->y<<"\r\n";
                }
            }


            stream.flush();
            file.close();
        }
    }
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::ChangeLevel(Newbie *m)
{
    qDebug()<<m->tname<<" leveled from "<<m->getType()<<" to "<<m->getTrueType();

    for(int i=0;i<MONK_ARR;++i)
        if(typeid(m)==typeid(monk[i]))
        {
            RemoveMonk(i);
            break;
        }
}

void MainWindow::on_actionMiniMap_triggered()
{
    minimap->show();
}

void MainWindow::on_actionChat_triggered()
{
    chat->show();
}
