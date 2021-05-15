#include <vnepogodin/overlay.h>
#include <vnepogodin/utils.hpp>

#ifdef _WIN32
#include <Windows.h>
#endif
#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

#include <QMouseEvent>
#include <QPainter>
#include <QString>
#include <QSvgRenderer>

using namespace vnepogodin;

Overlay::Overlay(QWidget* parent, Logger* log) : QWidget(parent), logger(log) {
    ui->setupUi(this);
    connectKeyboard();
}

void Overlay::paintEvent(QPaintEvent*) {
    // Initialize renderer with base asset
    QSvgRenderer renderer;

    if (keyboardConnected)
        renderer.load(QString(":/assets/keyboard/base.svg"));
    else
        renderer.load(QString(":/assets/keyboard/disconnected.svg"));

    renderer.setAspectRatioMode(Qt::KeepAspectRatio);

    // Paint base svg on widget
    QPainter painter(this);
    renderer.render(&painter);
    renderer.setViewBox(QRectF(0, 0, this->width(), this->height()));

    if (keyboardConnected) {
        QPoint corner = locateCorner(renderer.defaultSize(), renderer.viewBox().size());
        double scale  = getScale(renderer.defaultSize(), renderer.viewBox().size());
        paintFeatures(this, corner, scale);
    }
}

void Overlay::paintFeatures(QPaintDevice* device, QPoint corner, double scale) {
    paintButtons(this, corner, scale);
}

QPoint Overlay::locateCorner(QSize defaultSize, QSize viewBox) {
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

double Overlay::getScale(QSize defaultSize, QSize viewBox) {
    double result;
    QPoint corner = locateCorner(defaultSize, viewBox);

    // Since svg maintains aspect ratio, only height or width is needed
    double width = (double)viewBox.width() - 2 * corner.x();
    result       = width / (double)defaultSize.width();

    return result;
}

bool Overlay::connectKeyboard() {
    // Start polling
    keyboardConnected = false;

    if (poll.joinable())
        poll.join();

    keyboardConnected = true;
    poll              = std::thread(&Overlay::paintLoop, this);

    return true;
}

void Overlay::paintLoop() {
    while (keyboardConnected) {
        update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / refresh_rate));
    }
}

Overlay::~Overlay() {
    keyboardConnected = false;
    poll.join();
}

void Overlay::paintButtons(QPaintDevice* device, QPoint corner, double scale) {
    const auto& button = utils::get_key(logger);

    const std::unordered_map<uint32_t, std::pair<std::string_view, QPoint>> button_map = {
        {utils::key_code::W, {"w_button", {384, 0}}},
        {utils::key_code::A, {"a_button", {169, 182}}},
        {utils::key_code::S, {"s_button", {338, 182}}},
        {utils::key_code::D, {"d_button", {508, 182}}},
        {utils::key_code::Q, {"q_button", {210, 0}}},
        {utils::key_code::E, {"e_button", {552, 0}}},
        {utils::key_code::SHIFT, {"shift_button", {0, 182}}},
        {utils::key_code::CONTROL, {"ctrl_button", {23, 360}}},
        {utils::key_code::SPACEBAR, {"space_button", {192, 360}}}};
    for (const auto& [mask, asset] : button_map) {
        if (mask == button) {
            const QPoint& location = QPoint(utils::round((double)asset.second.x() * scale) + corner.x(),
                utils::round((double)asset.second.y() * scale) + corner.y());
            paintAsset(asset.first.data(), location, device, scale);

            auto& j                  = logger->get("numOfTouch");
            j.at(asset.first.data()) = j[asset.first.data()].get<int>() + 1;
        }
    }
}

void Overlay::paintAsset(std::string name, const QPoint& place, QPaintDevice* device, const double& scale) {
    name = ":/assets/keyboard/" + name + ".svg";
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
