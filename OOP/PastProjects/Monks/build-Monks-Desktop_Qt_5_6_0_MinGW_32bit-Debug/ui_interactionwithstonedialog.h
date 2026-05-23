/********************************************************************************
** Form generated from reading UI file 'interactionwithstonedialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INTERACTIONWITHSTONEDIALOG_H
#define UI_INTERACTIONWITHSTONEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_InteractionWithStoneDialog
{
public:
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QDialog *InteractionWithStoneDialog)
    {
        if (InteractionWithStoneDialog->objectName().isEmpty())
            InteractionWithStoneDialog->setObjectName(QStringLiteral("InteractionWithStoneDialog"));
        InteractionWithStoneDialog->resize(324, 43);
        gridLayout_2 = new QGridLayout(InteractionWithStoneDialog);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        pushButton = new QPushButton(InteractionWithStoneDialog);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        gridLayout->addWidget(pushButton, 0, 0, 1, 1);

        pushButton_2 = new QPushButton(InteractionWithStoneDialog);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        gridLayout->addWidget(pushButton_2, 0, 1, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);


        retranslateUi(InteractionWithStoneDialog);
        QObject::connect(pushButton, SIGNAL(clicked()), InteractionWithStoneDialog, SLOT(accept()));
        QObject::connect(pushButton_2, SIGNAL(clicked()), InteractionWithStoneDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(InteractionWithStoneDialog);
    } // setupUi

    void retranslateUi(QDialog *InteractionWithStoneDialog)
    {
        InteractionWithStoneDialog->setWindowTitle(QApplication::translate("InteractionWithStoneDialog", "Dialog", 0));
        pushButton->setText(QApplication::translate("InteractionWithStoneDialog", "\320\237\321\226\320\264\321\202\320\262\320\265\321\200\320\264\320\270\321\202\320\270", 0));
        pushButton_2->setText(QApplication::translate("InteractionWithStoneDialog", "\320\241\320\272\320\260\321\201\321\203\320\262\320\260\321\202\320\270", 0));
    } // retranslateUi

};

namespace Ui {
    class InteractionWithStoneDialog: public Ui_InteractionWithStoneDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INTERACTIONWITHSTONEDIALOG_H
