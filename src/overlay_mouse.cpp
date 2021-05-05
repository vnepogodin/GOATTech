#include <vnepogodin/overlay_mouse.h>
#include <vnepogodin/utils.hpp>

#ifdef _WIN32
#include <Windows.h>
#endif
#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

#include <QMouseEvent>
#include <QPainter>
#include <QString>
#include <QSvgRenderer>

Overlay_mouse::Overlay_mouse(QWidget* parent, Logger* log) : QWidget(parent), logger(log) {
    ui->setupUi(this);
    connectMouse();
}

void Overlay_mouse::paintEvent(QPaintEvent*) {
    // Initialize renderer with base asset
    QSvgRenderer renderer;

    if (mouseConnected)
        renderer.load(QString(":/assets/mouse/base.svg"));
    else
        renderer.load(QString(":/assets/mouse/disconnected.svg"));

    renderer.setAspectRatioMode(Qt::KeepAspectRatio);

    // Paint base svg on widget
    QPainter painter(this);
    renderer.render(&painter);
    renderer.setViewBox(QRectF(0, 0, this->width(), this->height()));

    if (mouseConnected) {
        QPoint corner = locateCorner(renderer.defaultSize(), renderer.viewBox().size());
        double scale  = getScale(renderer.defaultSize(), renderer.viewBox().size());
        paintFeatures(this, corner, scale);
    }
}

void Overlay_mouse::paintFeatures(QPaintDevice* device, QPoint corner, double scale) {
    paintButtons(this, corner, scale);
    //paintTouch(this, corner, scale);
}

QPoint Overlay_mouse::locateCorner(QSize defaultSize, QSize viewBox) {
    double defaultAR = (double)defaultSize.width() / (double)defaultSize.height();
    double viewAR    = (double)viewBox.width() / (double)viewBox.height();

    // @var arCoeff lets us know which side of the svg borders the widget
    double arCoeff = viewAR / defaultAR;
    double width, height;
    int x, y;

    if (arCoeff > 1) {
        height = (double)viewBox.height();
        width  = height * defaultAR;
        y      = 0;
        x      = (int)(0.5 + ((double)viewBox.width() - width) / 2);
    } else if (arCoeff < 1) {
        width  = (double)viewBox.width();
        height = width * 1 / defaultAR;
        x      = 0;
        y      = (int)(0.5 + ((double)viewBox.height() - height) / 2);
    } else {
        x = 0;
        y = 0;
    }

    return QPoint(x, y);
}

double Overlay_mouse::getScale(QSize defaultSize, QSize viewBox) {
    double result;
    QPoint corner = locateCorner(defaultSize, viewBox);

    // Since svg maintains aspect ratio, only height or width is needed
    double width = (double)viewBox.width() - 2 * corner.x();
    result       = width / (double)defaultSize.width();

    return result;
}

bool Overlay_mouse::connectMouse() {
    // Start polling
    mouseConnected = false;

    if (poll.joinable())
        poll.join();

    mouseConnected = true;
    poll           = std::thread(&Overlay_mouse::paintLoop, this);

    return true;
}

void Overlay_mouse::paintLoop() {
    while (mouseConnected) {
        update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / refresh_rate));
    }
}

Overlay_mouse::~Overlay_mouse() {
    mouseConnected = false;
    poll.join();

    delete logger;
}

void Overlay_mouse::paintButtons(QPaintDevice* device, QPoint corner, double scale) {
    const auto& button                                                                = utils::get_key(this->logger);
    const std::unordered_map<uint8_t, std::pair<std::string_view, QPoint>> button_map = {
        {utils::key_code::LBUTTON, {"left_button", {10, 0}}},
        {utils::key_code::RBUTTON, {"right_button", {512, 0}}},
        {utils::key_code::MBUTTON, {"middle_button", {415, 273}}},
        {utils::key_code::X1BUTTON, {"x_button", {2, 735}}},
        {utils::key_code::X2BUTTON, {"x_button", {41, 960}}}};
    for (const auto& [mask, asset] : button_map) {
        if (mask == button) {
            const QPoint& location = QPoint(utils::round((double)asset.second.x() * scale) + corner.x(),
                utils::round((double)asset.second.y() * scale) + corner.y());
            paintAsset(asset.first.data(), location, device, scale);
        }
    }
}

void Overlay_mouse::paintAsset(std::string name, const QPoint& place, QPaintDevice* device, const double& scale) {
    name = ":/assets/mouse/" + name + ".svg";
    QSvgRenderer renderer;
    renderer.load(QString(name.c_str()));

    const int& width  = utils::round((double)renderer.defaultSize().width() * scale);
    const int& height = utils::round((double)renderer.defaultSize().height() * scale);

    QImage image(width, height, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter imagePainter(&image);
    renderer.render(&imagePainter);

    QPainter painter(device);
    painter.drawImage(place, image);
}

void Overlay_mouse::paintTouch(QPaintDevice* device, QPoint corner, double scale) {
    QPoint tl{50, 50};
    float height = 139, width = 139;

#ifdef _WIN32
    POINT ptOld;
    GetCursorPos(&ptOld);

    const QPoint& location = QPoint(
        (int)utils::round(((double)tl.x() + (double)((unsigned)ptOld.x % (unsigned)width)) * scale) + corner.rx(),
        (int)utils::round(((double)tl.y() + (double)((unsigned)ptOld.y % (unsigned)height)) * scale) + corner.ry());
#ifndef _NDEBUG
    std::cout << "\nX: " << location.x() << " Y:" << location.y() << '\n';
#endif
    paintAsset("cursor", location, device, scale);
#endif
}
