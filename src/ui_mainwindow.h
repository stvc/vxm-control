/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.1.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionE_xit;
    QAction *actionSerialConfig;
    QAction *actionCameraConfig;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QPushButton *btnConnect;
    QLabel *labelMove;
    QGridLayout *gridLayout;
    QRadioButton *radioRight;
    QRadioButton *radioLeft;
    QRadioButton *radioDown;
    QRadioButton *radioUp;
    QGridLayout *gridLayout_2;
    QSpinBox *spinBox;
    QLabel *label;
    QPushButton *btnDoMove;
    QLabel *labelDraw;
    QPushButton *btnDrawLine;
    QPushButton *btnDrawRectangle;
    QSpacerItem *verticalSpacer;
    QGraphicsView *cameraFeed;
    QMenuBar *menubar;
    QMenu *menu_File;
    QMenu *menu_Connect;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(866, 486);
        actionE_xit = new QAction(MainWindow);
        actionE_xit->setObjectName(QStringLiteral("actionE_xit"));
        actionSerialConfig = new QAction(MainWindow);
        actionSerialConfig->setObjectName(QStringLiteral("actionSerialConfig"));
        actionCameraConfig = new QAction(MainWindow);
        actionCameraConfig->setObjectName(QStringLiteral("actionCameraConfig"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        btnConnect = new QPushButton(centralwidget);
        btnConnect->setObjectName(QStringLiteral("btnConnect"));
        btnConnect->setMaximumSize(QSize(180, 16777215));

        verticalLayout->addWidget(btnConnect);

        labelMove = new QLabel(centralwidget);
        labelMove->setObjectName(QStringLiteral("labelMove"));

        verticalLayout->addWidget(labelMove, 0, Qt::AlignHCenter);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        radioRight = new QRadioButton(centralwidget);
        radioRight->setObjectName(QStringLiteral("radioRight"));
        radioRight->setMaximumSize(QSize(60, 16777215));

        gridLayout->addWidget(radioRight, 1, 2, 1, 1);

        radioLeft = new QRadioButton(centralwidget);
        radioLeft->setObjectName(QStringLiteral("radioLeft"));
        radioLeft->setMaximumSize(QSize(60, 16777215));

        gridLayout->addWidget(radioLeft, 1, 0, 1, 1);

        radioDown = new QRadioButton(centralwidget);
        radioDown->setObjectName(QStringLiteral("radioDown"));
        radioDown->setMaximumSize(QSize(60, 16777215));

        gridLayout->addWidget(radioDown, 2, 1, 1, 1);

        radioUp = new QRadioButton(centralwidget);
        radioUp->setObjectName(QStringLiteral("radioUp"));

        gridLayout->addWidget(radioUp, 0, 1, 1, 1);


        verticalLayout->addLayout(gridLayout);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        spinBox = new QSpinBox(centralwidget);
        spinBox->setObjectName(QStringLiteral("spinBox"));
        spinBox->setMaximum(999);
        spinBox->setSingleStep(1);

        gridLayout_2->addWidget(spinBox, 0, 1, 1, 1);

        label = new QLabel(centralwidget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        btnDoMove = new QPushButton(centralwidget);
        btnDoMove->setObjectName(QStringLiteral("btnDoMove"));

        gridLayout_2->addWidget(btnDoMove, 1, 1, 1, 1);


        verticalLayout->addLayout(gridLayout_2);

        labelDraw = new QLabel(centralwidget);
        labelDraw->setObjectName(QStringLiteral("labelDraw"));

        verticalLayout->addWidget(labelDraw, 0, Qt::AlignHCenter);

        btnDrawLine = new QPushButton(centralwidget);
        btnDrawLine->setObjectName(QStringLiteral("btnDrawLine"));
        btnDrawLine->setMaximumSize(QSize(180, 16777215));

        verticalLayout->addWidget(btnDrawLine);

        btnDrawRectangle = new QPushButton(centralwidget);
        btnDrawRectangle->setObjectName(QStringLiteral("btnDrawRectangle"));
        btnDrawRectangle->setMaximumSize(QSize(180, 16777215));

        verticalLayout->addWidget(btnDrawRectangle);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout);

        cameraFeed = new QGraphicsView(centralwidget);
        cameraFeed->setObjectName(QStringLiteral("cameraFeed"));

        horizontalLayout->addWidget(cameraFeed);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 866, 28));
        menu_File = new QMenu(menubar);
        menu_File->setObjectName(QStringLiteral("menu_File"));
        menu_Connect = new QMenu(menubar);
        menu_Connect->setObjectName(QStringLiteral("menu_Connect"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menu_File->menuAction());
        menubar->addAction(menu_Connect->menuAction());
        menu_File->addAction(actionE_xit);
        menu_Connect->addAction(actionSerialConfig);
        menu_Connect->addAction(actionCameraConfig);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        actionE_xit->setText(QApplication::translate("MainWindow", "E&xit", 0));
        actionSerialConfig->setText(QApplication::translate("MainWindow", "Serial Port...", 0));
        actionCameraConfig->setText(QApplication::translate("MainWindow", "Camera...", 0));
        btnConnect->setText(QApplication::translate("MainWindow", "Connect", 0));
        labelMove->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-weight:600;\">Move</span></p></body></html>", 0));
        radioRight->setText(QApplication::translate("MainWindow", ">", 0));
        radioLeft->setText(QApplication::translate("MainWindow", "<", 0));
        radioDown->setText(QApplication::translate("MainWindow", "\\/", 0));
        radioUp->setText(QApplication::translate("MainWindow", "/\\", 0));
        label->setText(QApplication::translate("MainWindow", "Steps", 0));
        btnDoMove->setText(QApplication::translate("MainWindow", "Move", 0));
        labelDraw->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-weight:600;\">Draw</span></p></body></html>", 0));
        btnDrawLine->setText(QApplication::translate("MainWindow", "Line", 0));
        btnDrawRectangle->setText(QApplication::translate("MainWindow", "Rectangle", 0));
        menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0));
        menu_Connect->setTitle(QApplication::translate("MainWindow", "&Config", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
