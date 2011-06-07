/********************************************************************************
** Form generated from reading UI file 'cfd3.ui'
**
** Created: Tue Jun 7 13:23:56 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CFD3_H
#define UI_CFD3_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_cfd3Class
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *cfd3Class)
    {
        if (cfd3Class->objectName().isEmpty())
            cfd3Class->setObjectName(QString::fromUtf8("cfd3Class"));
        cfd3Class->resize(391, 285);
        centralwidget = new QWidget(cfd3Class);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        cfd3Class->setCentralWidget(centralwidget);
        menubar = new QMenuBar(cfd3Class);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 391, 25));
        cfd3Class->setMenuBar(menubar);
        statusbar = new QStatusBar(cfd3Class);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        cfd3Class->setStatusBar(statusbar);

        retranslateUi(cfd3Class);

        QMetaObject::connectSlotsByName(cfd3Class);
    } // setupUi

    void retranslateUi(QMainWindow *cfd3Class)
    {
        cfd3Class->setWindowTitle(QApplication::translate("cfd3Class", "MainWindow", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class cfd3Class: public Ui_cfd3Class {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CFD3_H
