#include "interactionwithmonkdialog.h"
#include "ui_interactionwithmonkdialog.h"
#include "newbie.h"
#include "stone1.h"

extern Stone1 **stone;
extern Newbie **monk;

InteractionWithMonkDialog::InteractionWithMonkDialog(QWidget *parent, int m1, int m2) :
    QDialog(parent),
    ui(new Ui::InteractionWithMonkDialog)
{
    ui->setupUi(this);
    setWindowTitle("Взаємодія "+monk[m1]->tname +" з монахом " + monk[m2]->tname);
    setWindowIcon(QIcon(":/images/bm.ico"));
}

InteractionWithMonkDialog::~InteractionWithMonkDialog()
{
    delete ui;
}

bool InteractionWithMonkDialog::getBlessing()
{
    return ui->checkBox->checkState();
}

double InteractionWithMonkDialog::getEnergy()
{
    return ui->lineEdit->text().toDouble();
}
