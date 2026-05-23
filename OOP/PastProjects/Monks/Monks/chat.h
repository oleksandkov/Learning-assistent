#ifndef CHAT_H
#define CHAT_H

#include <QDialog>

namespace Ui {
class Chat;
}

class Chat : public QDialog
{
    Q_OBJECT

public:
    explicit Chat(QWidget *parent = 0);
    ~Chat();
public slots:
    void addText(QString text);

private:
    Ui::Chat *ui;
};

#endif // CHAT_H
