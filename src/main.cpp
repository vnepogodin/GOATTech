#include <vnepogodin/mainwindow.h>

#include <QApplication>

int main(int argc, char** argv) {
    static constexpr auto NAME = L"overlay";
    HANDLE mutex               = CreateMutexW(NULL, TRUE, NAME);
    if ((mutex == nullptr) || (GetLastError() == ERROR_ALREADY_EXISTS))
        return 0;

    QApplication a(argc, argv);
    vnepogodin::MainWindow w;
    w.showFullScreen();
    return a.exec();
}
