// Copyright (C) 2021 Vladislav Nepogodin
//
// This file is part of SportTech overlay project.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <vnepogodin/logger.hpp>
#include <vnepogodin/overlay.h>
#include <vnepogodin/overlay_mouse.h>

#include <memory>
#ifdef _WIN32
#include <Windows.h>
#endif
#include <QMainWindow>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE
class Ui_MainWindow {
 public:
    QWidget* centralwidget;
    QHBoxLayout* horizontalLayout;
    QWidget* widget;
    QVBoxLayout* verticalLayout;
    QSpacerItem* verticalSpacer;
    vnepogodin::Overlay* keyboard;
    vnepogodin::Logger* log;
    vnepogodin::Overlay_mouse* mouse;
    QSpacerItem* horizontalSpacer;
    QMenuBar* menubar;
    QStatusBar* statusbar;

    void setupUi(QMainWindow* MainWindow) {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("SportTech-overlay"));
        MainWindow->resize(923, 657);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        widget = new QWidget(centralwidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        log      = new vnepogodin::Logger();
        keyboard = new vnepogodin::Overlay(widget, log);
        mouse    = new vnepogodin::Overlay_mouse(widget, log);
        keyboard->setObjectName(QString::fromUtf8("keyboard"));
        mouse->setObjectName(QString::fromUtf8("mouse"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(keyboard->sizePolicy().hasHeightForWidth());

        keyboard->setSizePolicy(sizePolicy);
        keyboard->setMinimumSize(QSize(0, 0));

        QSizePolicy mouse_sizepolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        mouse_sizepolicy.setHorizontalStretch(0);
        mouse_sizepolicy.setVerticalStretch(0);
        mouse_sizepolicy.setHeightForWidth(mouse->sizePolicy().hasHeightForWidth());
        mouse->setSizePolicy(mouse_sizepolicy);
        mouse->setMinimumSize(QSize(0, 0));

        verticalLayout->addWidget(keyboard);
        horizontalLayout->addWidget(widget);
        verticalLayout->addWidget(mouse);

        horizontalLayout->addWidget(widget);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 923, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    }  // setupUi

    void retranslateUi(QMainWindow* MainWindow) {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
    }  // retranslateUi

    ~Ui_MainWindow() {
        delete centralwidget;
        delete horizontalLayout;
        delete widget;
        delete verticalLayout;
        delete verticalSpacer;
        delete keyboard;
        delete mouse;
        //delete log;
        delete horizontalSpacer;
        delete menubar;
        delete statusbar;
    }
};

namespace Ui {
class MainWindow : public Ui_MainWindow { };
}  // namespace Ui

QT_END_NAMESPACE

namespace vnepogodin {
class MainWindow : public QMainWindow {
    Q_OBJECT

 public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        ui.get()->setupUi(this);

        setAttribute(Qt::WA_TranslucentBackground);
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::BypassWindowManagerHint);  // | Qt::SplashScreen);
#ifdef _WIN32
        SetForegroundWindow((HWND)winId());
        SetWindowPos((HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
#endif
        const int& size = qMin(this->size().height(), this->size().width()) - 300;
        ui.get()->keyboard->setFixedSize(size, size);
        //ui.get()->mouse->setFixedSize(size - 150, size - 150);
    }

 private:
    std::unique_ptr<Ui::MainWindow> ui = std::make_unique<Ui::MainWindow>();
};
}  // namespace vnepogodin

#endif  // MAINWINDOW_H_
