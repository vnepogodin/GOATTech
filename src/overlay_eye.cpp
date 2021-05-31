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

#include <vnepogodin/overlay_eye.h>
#include <vnepogodin/utils.hpp>

#include <vnepogodin/thirdparty/HTTPRequest.hpp>

#include <array>
#include <charconv>
#include <cmath>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>

#include <QDesktopWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QString>
#include <QSvgRenderer>

using namespace vnepogodin;

static inline QPoint parse_fromString(std::string& str) {
    std::regex regexp("\\d+%");

    // flag type for determining the matching behavior (in this case on string objects)
    std::smatch match;

    std::array<int, 3> coors;

    int i = 0;
    while (std::regex_search(str, match, regexp)) {
        std::string tmp   = match.str(0);
        const auto& value = tmp.erase((tmp.size() == 3) ? 2 : 1);
        int result        = 0;
        std::from_chars(value.data(), value.data() + value.size(), result);
        coors[i] = result;
        ++i;

        // suffix to find the rest of the string.
        str = match.suffix().str();
    }
    //  const auto& rec = QApplication::desktop()->geometry();

    //return {(rec.width() * coors[0] / 100), (rec.height() * coors[1] / 100)};
    return {coors[0], coors[1]};
};

Overlay_Eye::Overlay_Eye(QWidget* parent) : QWidget(parent) {
    ui->setupUi(this);

    setAttribute(Qt::WA_NativeWindow);
    //startTimer(100);
    connectMouse();
}

void Overlay_Eye::paintEvent(QPaintEvent*) {
    // Initialize renderer with base asset
    QSvgRenderer renderer;

    if (mouseConnected)
        renderer.load(QString(":/assets/dualshock_black/touchpad.svg"));
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

void Overlay_Eye::paintFeatures(QPaintDevice* device, QPoint corner, double scale) {
    paintTouch(this, corner, scale);
}

QPoint Overlay_Eye::locateCorner(QSize defaultSize, QSize viewBox) {
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

double Overlay_Eye::getScale(QSize defaultSize, QSize viewBox) {
    double result;
    QPoint corner = locateCorner(defaultSize, viewBox);

    // Since svg maintains aspect ratio, only height or width is needed
    double width = (double)viewBox.width() - 2 * corner.x();
    result       = width / (double)defaultSize.width();

    return result;
}

bool Overlay_Eye::connectMouse() {
    // Start polling
    mouseConnected = false;

    if (poll.joinable())
        poll.join();

    mouseConnected = true;
    poll           = std::thread(&Overlay_Eye::paintLoop, this);

    return true;
}

void Overlay_Eye::paintLoop() {
    while (mouseConnected) {
        update();
        std::this_thread::sleep_for(std::chrono::milliseconds(500 / refresh_rate));
 //       paintTouch(this, _corner, _scale);
    }
}

Overlay_Eye::~Overlay_Eye() {
    mouseConnected = false;
    poll.join();
}

void Overlay_Eye::paintAsset(std::string name, const QPoint& place, QPaintDevice* device, const double& scale) {
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

void Overlay_Eye::paintTouch(QPaintDevice* device, QPoint corner, double scale) {
    static constexpr auto URL = "http://www.headkraken.gg/soft/SkolkovoJuniorChallenge/9-10/test/";
    QPoint tl{0, 0};
    float height = 151, width = 262;

    http::Request request(URL);

    // send a get request
    const auto response = request.send("GET");
    std::string str(response.body.begin(), response.body.end());
    const auto& res = parse_fromString(str);

    const QPoint coors{static_cast<int>((width * res.x()/100 - 20)), static_cast<int>((height * res.y()/100 - 20))};

    const QPoint& location = QPoint(
        (int)utils::round(((double)tl.x() + (double)((unsigned)coors.x() % (unsigned)width)) * scale) + corner.rx(),
        (int)utils::round(((double)tl.y() + (double)((unsigned)coors.y() % (unsigned)height)) * scale) + corner.ry());

    paintAsset("cursor", location, device, scale - 0.3);
}
