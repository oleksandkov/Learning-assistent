#ifndef INTERACTIONWITHMONKDIALOG_H
#define INTERACTIONWITHMONKDIALOG_H

#include <QDialog>

namespace Ui {
class InteractionWithMonkDialog;
}

class InteractionWithMonkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InteractionWithMonkDialog(QWidget *parent = 0,int m1=0,int m2=1);
    ~InteractionWithMonkDialog();

    bool getBlessing();
    double getEnergy();

private:
    Ui::InteractionWithMonkDialog *ui;
};

#endif // INTERACTIONWITHMONKDIALOG_H
