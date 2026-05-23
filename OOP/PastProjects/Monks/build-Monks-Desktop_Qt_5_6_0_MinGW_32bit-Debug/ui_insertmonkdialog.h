/********************************************************************************
** Form generated from reading UI file 'insertmonkdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INSERTMONKDIALOG_H
#define UI_INSERTMONKDIALOG_H

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
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_InsertMonkDialog
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QVBoxLayout *verticalLayout;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QRadioButton *radioButton_3;
    QVBoxLayout *verticalLayout_3;
    QCheckBox *checkBox_2;
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QDialog *InsertMonkDialog)
    {
        if (InsertMonkDialog->objectName().isEmpty())
            InsertMonkDialog->setObjectName(QStringLiteral("InsertMonkDialog"));
        InsertMonkDialog->resize(214, 137);
        gridLayout = new QGridLayout(InsertMonkDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        label = new QLabel(InsertMonkDialog);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout_2->addWidget(label);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        radioButton = new QRadioButton(InsertMonkDialog);
        radioButton->setObjectName(QStringLiteral("radioButton"));

        verticalLayout->addWidget(radioButton);

        radioButton_2 = new QRadioButton(InsertMonkDialog);
        radioButton_2->setObjectName(QStringLiteral("radioButton_2"));

        verticalLayout->addWidget(radioButton_2);

        radioButton_3 = new QRadioButton(InsertMonkDialog);
        radioButton_3->setObjectName(QStringLiteral("radioButton_3"));

        verticalLayout->addWidget(radioButton_3);


        verticalLayout_2->addLayout(verticalLayout);


        horizontalLayout_2->addLayout(verticalLayout_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        checkBox_2 = new QCheckBox(InsertMonkDialog);
        checkBox_2->setObjectName(QStringLiteral("checkBox_2"));

        verticalLayout_3->addWidget(checkBox_2);

        lineEdit = new QLineEdit(InsertMonkDialog);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        verticalLayout_3->addWidget(lineEdit);

        lineEdit_2 = new QLineEdit(InsertMonkDialog);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));

        verticalLayout_3->addWidget(lineEdit_2);


        horizontalLayout_2->addLayout(verticalLayout_3);


        gridLayout->addLayout(horizontalLayout_2, 0, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        pushButton = new QPushButton(InsertMonkDialog);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(InsertMonkDialog);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        horizontalLayout->addWidget(pushButton_2);


        gridLayout->addLayout(horizontalLayout, 1, 0, 1, 1);


        retranslateUi(InsertMonkDialog);
        QObject::connect(pushButton, SIGNAL(clicked()), InsertMonkDialog, SLOT(accept()));
        QObject::connect(pushButton_2, SIGNAL(clicked()), InsertMonkDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(InsertMonkDialog);
    } // setupUi

    void retranslateUi(QDialog *InsertMonkDialog)
    {
        InsertMonkDialog->setWindowTitle(QApplication::translate("InsertMonkDialog", "Dialog", 0));
        label->setText(QApplication::translate("InsertMonkDialog", "\320\242\320\270\320\277 \320\274\320\276\320\275\320\260\321\205\320\260:", 0));
        radioButton->setText(QApplication::translate("InsertMonkDialog", "Newbie", 0));
        radioButton_2->setText(QApplication::translate("InsertMonkDialog", "Expirienced", 0));
        radioButton_3->setText(QApplication::translate("InsertMonkDialog", "Guru", 0));
        checkBox_2->setText(QApplication::translate("InsertMonkDialog", "\320\227\321\203\320\277\320\270\320\275\320\270\321\202\320\270", 0));
        lineEdit->setText(QString());
        lineEdit->setPlaceholderText(QApplication::translate("InsertMonkDialog", "\320\232\321\226\320\273\321\214\320\272\321\226\321\201\321\202\321\214 \320\265\320\275\320\265\321\200\320\263\321\226\321\227", 0));
        lineEdit_2->setPlaceholderText(QApplication::translate("InsertMonkDialog", "\320\206\320\274'\321\217", 0));
        pushButton->setText(QApplication::translate("InsertMonkDialog", "\320\237\321\226\320\264\321\202\320\262\320\265\321\200\320\264\320\270\321\202\320\270", 0));
        pushButton_2->setText(QApplication::translate("InsertMonkDialog", "\320\241\320\272\320\260\321\201\321\203\320\262\320\260\321\202\320\270", 0));
    } // retranslateUi

};

namespace Ui {
    class InsertMonkDialog: public Ui_InsertMonkDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INSERTMONKDIALOG_H
