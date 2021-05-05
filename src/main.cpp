#include <vnepogodin/mainwindow.h>

//#include <JoyShockLibrary.h>
#include <QApplication>

auto main(int argc, char** argv) -> std::int32_t {
    QApplication a(argc, argv);
    MainWindow wkey;

    wkey.showFullScreen();
    return a.exec();
}
