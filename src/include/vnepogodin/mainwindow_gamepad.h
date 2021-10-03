// Copyright (C) 2021 Vladislav Nepogodin
//
// This file is part of GOATTech project.
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

#ifndef MAINWINDOW_GAMEPAD_H_
#define MAINWINDOW_GAMEPAD_H_

#include <vnepogodin/overlay_gamepad.h>
#include <ui_mainwindow_gamepad.h>

#include <memory>
#ifdef _WIN32
#include <Windows.h>
#else
#include <QScreen>
#endif
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow_gamepad;
}
QT_END_NAMESPACE

class MainWindow_gamepad : public QMainWindow {
    Q_OBJECT

 public:
    MainWindow_gamepad(QWidget* parent = nullptr) : QMainWindow(parent) {
        ui.get()->setupUi(this);

        setAttribute(Qt::WA_TranslucentBackground);
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowTransparentForInput);
#ifdef _WIN32
        SetForegroundWindow((HWND)winId());
        SetWindowPos((HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
#else
        {
            // QSize size = qApp->screens()[0]->size();
            // setGeometry(0, 0, size.width(), size.height());
        }
        // setAutoFillBackground(true);
        // setAttribute(Qt::WA_NoSystemBackground);
        // setAttribute(Qt::WA_PaintOnScreen);
        // setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint |
        // Qt::WindowTransparentForInput | Qt::X11BypassWindowManagerHint);
#endif
        const int& size = qMin(this->size().height(), this->size().width()) - 300;
        ui.get()->gamepad->setFixedSize(size, size);
    }

 private:
    std::unique_ptr<Ui::MainWindow_gamepad> ui = std::make_unique<Ui::MainWindow_gamepad>();
};
#endif  // MAINWINDOW_GAMEPAD_H_
