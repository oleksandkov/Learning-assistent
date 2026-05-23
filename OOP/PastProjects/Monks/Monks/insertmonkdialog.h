#ifndef INSERTMONKDIALOG_H
#define INSERTMONKDIALOG_H

#include <QDialog>

namespace Ui {
class InsertMonkDialog;
}

class InsertMonkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InsertMonkDialog(QWidget *parent = 0);
    ~InsertMonkDialog();
    int getType();
    bool getPause();
    double getEnergy();
    double getExpirience();
    QString getName();


private:
    Ui::InsertMonkDialog *ui;
};

#endif // INSERTMONKDIALOG_H
