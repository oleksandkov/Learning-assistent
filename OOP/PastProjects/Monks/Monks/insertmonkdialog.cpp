#include "insertmonkdialog.h"
#include "ui_insertmonkdialog.h"

InsertMonkDialog::InsertMonkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InsertMonkDialog)
{
    ui->setupUi(this);
    setWindowTitle("Додати монаха");
}

InsertMonkDialog::~InsertMonkDialog()
{
    delete ui;
}

int InsertMonkDialog::getType()
{
    if(ui->radioButton->isChecked()) return 1;
    if(ui->radioButton_2->isChecked()) return 2;
    if(ui->radioButton_3->isChecked()) return 3;


}

bool InsertMonkDialog::getPause()
{
    return ui->checkBox_2->checkState();
}

double InsertMonkDialog::getEnergy()
{
    return ui->lineEdit->text().toDouble();
}

double InsertMonkDialog::getExpirience()
{
    switch(getType())
    {
    case 1:
        return 0;
    case 2:
        return 37.5;
    case 3:
        return 75;
    }
}

QString InsertMonkDialog::getName()
{
    return ui->lineEdit_2->text();
}
