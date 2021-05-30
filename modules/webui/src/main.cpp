#include <vnepogodin/webui.hpp>

#include <QApplication>
#include <QWebEngineUrlScheme>

int main(int argc, char** argv) {
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    Webui webui;

    webui.show();
    return app.exec();
}
