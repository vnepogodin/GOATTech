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

#include <vnepogodin/mainwindow.h>

#include <QApplication>

#ifndef _WIN32
#include <QSharedMemory>
bool IsInstanceAlreadyRunning(QSharedMemory &memoryLock) {
    if (!memoryLock.create(1)) {
        memoryLock.attach();
        memoryLock.detach();

        if (!memoryLock.create(1)) {
            return true;
        }
    }

    return false;
}
#endif


auto main(int argc, char** argv) -> std::int32_t {
#ifdef _WIN32
    static constexpr auto NAME = L"overlay";
    HANDLE mutex               = CreateMutexW(NULL, TRUE, NAME);
    if ((mutex == nullptr) || (GetLastError() == ERROR_ALREADY_EXISTS))
        return 0;
#else
    QSharedMemory sharedMemoryLock("SportTech-overlay-lock");
    if (IsInstanceAlreadyRunning(sharedMemoryLock))
        return -1;
#endif

    // Set application info
    QCoreApplication::setOrganizationName("torrenttor");
    QCoreApplication::setApplicationName("SportTech");

    // Set application attributes
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    QApplication a(argc, argv);
    vnepogodin::MainWindow w;
    w.showFullScreen();
    return a.exec();
}
