#ifndef INTERACTIONWITHSTONEDIALOG_H
#define INTERACTIONWITHSTONEDIALOG_H

#include <QDialog>

namespace Ui {
class InteractionWithStoneDialog;
}

class InteractionWithStoneDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InteractionWithStoneDialog(QWidget *parent = 0);
    ~InteractionWithStoneDialog();

private:
    Ui::InteractionWithStoneDialog *ui;
};

#endif // INTERACTIONWITHSTONEDIALOG_H
