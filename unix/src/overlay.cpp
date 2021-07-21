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

#include <vnepogodin/overlay.hpp>

#include <cmath>

#include <QPainter>
#include <QString>
#include <QSvgRenderer>

using namespace vnepogodin;

Overlay::Overlay(QWidget* parent) : QWidget(parent) {
    ui->setupUi(this);

    setAttribute(Qt::WA_NativeWindow);
    connect();
}

void Overlay::paintEvent(QPaintEvent*) {
    // Initialize renderer with base asset
    QSvgRenderer renderer;

    if (connected)
        renderer.load(QString(getSvgPath()) + "base.svg");
    else
        renderer.load(QString(getSvgPath()) + "disconnected.svg");

    renderer.setAspectRatioMode(Qt::KeepAspectRatio);

    // Paint base svg on widget
    QPainter painter(this);
    renderer.render(&painter);
    renderer.setViewBox(QRectF(0, 0, this->width(), this->height()));

    if (connected) {
        const auto& corner  = locateCorner(renderer.defaultSize(), renderer.viewBox().size());
        const double& scale = getScale(renderer.defaultSize(), renderer.viewBox().size());
        paintFeatures(this, corner, scale);
    }
}

void Overlay::paintFeatures(QPaintDevice* device, const QPoint& corner, const double& scale) {
    paintButtons(device, corner, scale);
}

QPoint Overlay::locateCorner(const QSize& defaultSize, const QSize& viewBox) {
    const double& defaultAR = static_cast<double>(defaultSize.width()) / defaultSize.height();
    const double& viewAR    = static_cast<double>(viewBox.width()) / viewBox.height();

    // lets us know which side of the svg borders the widget
    const double& arRatio = viewAR / defaultAR;

    int x{};
    int y{};

    if (arRatio > 1) {
        const double& height = viewBox.height();
        const double& width  = height * defaultAR;
        x                    = 0.5 + ((double)viewBox.width() - width) / 2;
    } else if (arRatio < 1) {
        const double& width  = viewBox.width();
        const double& height = width * 1 / defaultAR;
        y                    = 0.5 + ((double)viewBox.height() - height) / 2;
    }

    return {x, y};
}

double Overlay::getScale(const QSize& defaultSize, const QSize& viewBox) {
    const auto& corner = locateCorner(defaultSize, viewBox);

    // Since svg maintains aspect ratio, only height or width is needed
    const double& width = (double)viewBox.width() - 2 * corner.x();
    return width / (double)defaultSize.width();
}

bool Overlay::connect() noexcept {
    if (poll.joinable())
        poll.join();

    connected = true;
    poll      = std::thread(&Overlay::paintLoop, this);

    return true;
}

void Overlay::paintLoop() {
    while (connected) {
        update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / refresh_rate));
    }
}

Overlay::~Overlay() {
    connected = false;
    poll.join();
}

void Overlay::paintAsset(std::string name, const QPoint& place, QPaintDevice* device, const double& scale) {
    name = getSvgPath() + name + ".svg";
    QSvgRenderer renderer;
    renderer.load(QString(name.c_str()));

    const int& width  = std::round((double)renderer.defaultSize().width() * scale);
    const int& height = std::round((double)renderer.defaultSize().height() * scale);

    QImage image(width, height, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter imagePainter(&image);
    renderer.render(&imagePainter);

    QPainter painter(device);
    painter.drawImage(place, image);
}