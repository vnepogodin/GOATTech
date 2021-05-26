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

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
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
