/********************************************************************************
** Form generated from reading UI file 'interactionwithmonkdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INTERACTIONWITHMONKDIALOG_H
#define UI_INTERACTIONWITHMONKDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_InteractionWithMonkDialog
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QCheckBox *checkBox;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QLineEdit *lineEdit;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QDialog *InteractionWithMonkDialog)
    {
        if (InteractionWithMonkDialog->objectName().isEmpty())
            InteractionWithMonkDialog->setObjectName(QStringLiteral("InteractionWithMonkDialog"));
        InteractionWithMonkDialog->resize(384, 142);
        gridLayout = new QGridLayout(InteractionWithMonkDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        checkBox = new QCheckBox(InteractionWithMonkDialog);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        verticalLayout->addWidget(checkBox);

        label = new QLabel(InteractionWithMonkDialog);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_2 = new QLabel(InteractionWithMonkDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout->addWidget(label_2);

        lineEdit = new QLineEdit(InteractionWithMonkDialog);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        horizontalLayout->addWidget(lineEdit);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        pushButton = new QPushButton(InteractionWithMonkDialog);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout_2->addWidget(pushButton);

        pushButton_2 = new QPushButton(InteractionWithMonkDialog);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        horizontalLayout_2->addWidget(pushButton_2);


        verticalLayout->addLayout(horizontalLayout_2);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        retranslateUi(InteractionWithMonkDialog);
        QObject::connect(pushButton, SIGNAL(clicked()), InteractionWithMonkDialog, SLOT(accept()));
        QObject::connect(pushButton_2, SIGNAL(clicked()), InteractionWithMonkDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(InteractionWithMonkDialog);
    } // setupUi

    void retranslateUi(QDialog *InteractionWithMonkDialog)
    {
        InteractionWithMonkDialog->setWindowTitle(QApplication::translate("InteractionWithMonkDialog", "Dialog", 0));
        checkBox->setText(QApplication::translate("InteractionWithMonkDialog", "\320\221\320\273\320\260\320\263\320\276\321\201\320\273\320\276\320\262\320\265\320\275\320\275\321\217", 0));
        label->setText(QApplication::translate("InteractionWithMonkDialog", "<html><head/><body><p><span style=\" font-size:7pt;\">(\320\221\320\273\320\260\320\263\320\276\321\201\320\273\320\276\320\262\320\270\321\202\320\270 \320\274\320\276\320\275\320\260\321\205\320\260 \320\277\320\276\320\274\320\265\320\264\320\270\321\202\321\203\320\262\320\260\321\202\320\270 \320\275\320\260 \320\272\320\260\320\274\320\265\320\275\321\226 \321\201\320\262\320\276\320\263\320\276 \321\200\321\226\320\262\320\275\321\217)</span></p></body></html>", 0));
        label_2->setText(QApplication::translate("InteractionWithMonkDialog", "\320\227\320\261\321\226\320\273\321\214\321\210\320\270\321\202\320\270/\320\227\320\274\320\265\320\275\321\210\320\270\321\202\320\270 \320\265\320\275\320\265\321\200\320\263\321\226\321\216 \320\275\320\260 (-7;7) :", 0));
        pushButton->setText(QApplication::translate("InteractionWithMonkDialog", "\320\237\321\226\320\264\321\202\320\262\320\265\321\200\320\264\320\270\321\202\320\270", 0));
        pushButton_2->setText(QApplication::translate("InteractionWithMonkDialog", "\320\241\320\272\320\260\321\201\321\203\320\262\320\260\321\202\320\270", 0));
    } // retranslateUi

};

namespace Ui {
    class InteractionWithMonkDialog: public Ui_InteractionWithMonkDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INTERACTIONWITHMONKDIALOG_H
