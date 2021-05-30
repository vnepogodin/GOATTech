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
#include <QProcess>
#ifndef _WIN32
#include <QSystemTrayIcon>
#endif
#include <memory>

QT_BEGIN_NAMESPACE
class Ui_MainWindow {
 public:
    vnepogodin::Overlay* keyboard;
    vnepogodin::Overlay_mouse* mouse;

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
        keyboard = new vnepogodin::Overlay(widget);
        mouse    = new vnepogodin::Overlay_mouse(widget);
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
        MainWindow->setWindowTitle("MainWindow");
    }  // retranslateUi

    virtual ~Ui_MainWindow() = default;

 private:
    QWidget* centralwidget;
    QHBoxLayout* horizontalLayout;
    QWidget* widget;
    QVBoxLayout* verticalLayout;
    QSpacerItem* verticalSpacer;

    QSpacerItem* horizontalSpacer;
    QMenuBar* menubar;
    QStatusBar* statusbar;
};

namespace Ui {
class MainWindow : public Ui_MainWindow { };
}  // namespace Ui

QT_END_NAMESPACE

namespace vnepogodin {
class MainWindow : public QMainWindow {
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)
 public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();

#ifdef _WIN32
 protected:
    // The method for processing native events from the OS in Qt
    virtual bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
#else
 protected:
    virtual bool event(QEvent* ev) override;

 public slots:
    void iconActivated(QSystemTrayIcon::ActivationReason);
#endif

 private:
#ifdef _WIN32
    static constexpr auto IDT_TIMER = 1001;
    static constexpr auto IDT_TRAY  = WM_APP;
    HWND m_hwnd                       = nullptr;
#else
    std::unique_ptr<QSystemTrayIcon> m_trayIcon;
    std::unique_ptr<QMenu> m_trayMenu;
    std::unique_ptr<QAction> m_quitAction;
    std::unique_ptr<QAction> m_settingsAction;
    int m_timer{};

    void createMenu() noexcept;
#endif
    std::array<std::uint8_t, 2> m_activated;
    //const int refresh_rate = 500;  // Frequency of keyboard input checking in hertz
    //std::thread poll;

    std::unique_ptr<QProcess> m_process_settings;
    //std::unique_ptr<QProcess> m_process_charts;
    std::unique_ptr<Ui::MainWindow> m_ui = std::make_unique<Ui::MainWindow>();

    //void loadToMemory();
};
}  // namespace vnepogodin

#endif  // MAINWINDOW_H_
