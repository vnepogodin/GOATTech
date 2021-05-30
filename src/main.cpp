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

#ifndef _WIN32
#include <sys/file.h>
#endif
#include <QApplication>

auto main(int argc, char** argv) -> std::int32_t {
#ifdef _WIN32
    static constexpr auto NAME = L"overlay";
    HANDLE mutex               = CreateMutexW(NULL, TRUE, NAME);
    if ((mutex == nullptr) || (GetLastError() == ERROR_ALREADY_EXISTS))
        return 0;
#else
    static constexpr auto NAME = "/tmp/overlay.pid";
    int fd                     = openat(0, NAME, O_CREAT | O_EXCL);
    if (fd == -1) {
        close(fd);
        return 0;
    }
    flock(fd, LOCK_EX);
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
    auto res = a.exec();
#ifndef _WIN32
    close(fd);
    unlink(NAME);
    flock(fd, LOCK_UN);
#endif
    return res;
}
