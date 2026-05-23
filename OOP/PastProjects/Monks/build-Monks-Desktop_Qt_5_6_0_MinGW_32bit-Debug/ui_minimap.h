/********************************************************************************
** Form generated from reading UI file 'minimap.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MINIMAP_H
#define UI_MINIMAP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_MiniMap
{
public:
    QGraphicsView *graphicsView;

    void setupUi(QDialog *MiniMap)
    {
        if (MiniMap->objectName().isEmpty())
            MiniMap->setObjectName(QStringLiteral("MiniMap"));
        MiniMap->resize(400, 300);
        graphicsView = new QGraphicsView(MiniMap);
        graphicsView->setObjectName(QStringLiteral("graphicsView"));
        graphicsView->setGeometry(QRect(0, 0, 400, 300));

        retranslateUi(MiniMap);

        QMetaObject::connectSlotsByName(MiniMap);
    } // setupUi

    void retranslateUi(QDialog *MiniMap)
    {
        MiniMap->setWindowTitle(QApplication::translate("MiniMap", "Dialog", 0));
    } // retranslateUi

};

namespace Ui {
    class MiniMap: public Ui_MiniMap {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MINIMAP_H
