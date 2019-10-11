/********************************************************************************
** Form generated from reading UI file 'minecraft.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MINECRAFT_H
#define UI_MINECRAFT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include "gl_widget.h"
#include "root_widget.h"
#include "utils.h"

QT_BEGIN_NAMESPACE

class Ui_MinecraftClass
{
public:
    QAction *action_Open;
    QAction *action_Save;
    ROOT_WIDGET *centralWidget;
    GL_WIDGET *widget;
    UTILS *widget_2;
    QMenuBar *menuBar;
    QMenu *menu;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MinecraftClass)
    {
        if (MinecraftClass->objectName().isEmpty())
            MinecraftClass->setObjectName(QStringLiteral("MinecraftClass"));
        MinecraftClass->resize(800, 600);
        action_Open = new QAction(MinecraftClass);
        action_Open->setObjectName(QStringLiteral("action_Open"));
        action_Save = new QAction(MinecraftClass);
        action_Save->setObjectName(QStringLiteral("action_Save"));
        centralWidget = new ROOT_WIDGET(MinecraftClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        widget = new GL_WIDGET(centralWidget);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(0, 0, 800, 600));
        widget_2 = new UTILS(centralWidget);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        widget_2->setGeometry(QRect(540, 60, 50, 50));
        MinecraftClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MinecraftClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 800, 26));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        MinecraftClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(MinecraftClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MinecraftClass->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());
        menu->addAction(action_Open);
        menu->addSeparator();
        menu->addAction(action_Save);

        retranslateUi(MinecraftClass);
        QObject::connect(widget_2, SIGNAL(_logd(QString)), statusBar, SLOT(showMessage(QString)));
        QObject::connect(action_Open, SIGNAL(triggered()), widget_2, SLOT(open_file()));
        QObject::connect(centralWidget, SIGNAL(change_size(QSize)), widget, SLOT(on_resize(QSize)));
        QObject::connect(action_Save, SIGNAL(triggered()), widget_2, SLOT(save_file()));
        QObject::connect(widget_2, SIGNAL(_open_file(QString)), widget, SLOT(on_game_profile_change(QString)));
        QObject::connect(widget_2, SIGNAL(_save_file(QString)), widget, SLOT(on_game_profile_save(QString)));

        QMetaObject::connectSlotsByName(MinecraftClass);
    } // setupUi

    void retranslateUi(QMainWindow *MinecraftClass)
    {
        MinecraftClass->setWindowTitle(QApplication::translate("MinecraftClass", "Minecraft", Q_NULLPTR));
        action_Open->setText(QApplication::translate("MinecraftClass", "&Open", Q_NULLPTR));
        action_Save->setText(QApplication::translate("MinecraftClass", "&Save", Q_NULLPTR));
        menu->setTitle(QApplication::translate("MinecraftClass", "File", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MinecraftClass: public Ui_MinecraftClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MINECRAFT_H
