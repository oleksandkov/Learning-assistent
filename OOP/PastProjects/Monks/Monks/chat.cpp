#include "chat.h"
#include "ui_chat.h"

Chat::Chat(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
    setWindowTitle("Чат");
    setWindowIcon(QIcon(":/images/bm.ico"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

Chat::~Chat()
{
    delete ui;
}

void Chat::addText(QString text)
{
    ui->textBrowser->append(text);
}
