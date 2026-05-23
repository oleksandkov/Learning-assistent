#include "interactionwithstonedialog.h"
#include "ui_interactionwithstonedialog.h"

InteractionWithStoneDialog::InteractionWithStoneDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InteractionWithStoneDialog)
{
    ui->setupUi(this);
    setWindowTitle("Взаємодія з камнем");
    setWindowIcon(QIcon(":/images/bm.ico"));
}

InteractionWithStoneDialog::~InteractionWithStoneDialog()
{
    delete ui;
}
