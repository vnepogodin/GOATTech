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

#include <vnepogodin/input_data.hpp>
#include <vnepogodin/overlay_mouse.hpp>
#include <vnepogodin/utils.hpp>

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

using namespace vnepogodin;

OverlayMouse::OverlayMouse(QWidget* parent) : QWidget(parent) {
    ui->setupUi(this);

    setAttribute(Qt::WA_NativeWindow);
    connectMouse();
}

void OverlayMouse::paintEvent(QPaintEvent*) {
    // Initialize renderer with base asset
    QSvgRenderer renderer;

    if (mouseConnected)
        renderer.load(QString(":/mouse/base.svg"));
    else
        renderer.load(QString(":/mouse/disconnected.svg"));

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

void OverlayMouse::paintFeatures(QPaintDevice* device, QPoint corner, double scale) {
    paintButtons(this, corner, scale);
    //paintTouch(this, corner, scale);
}

QPoint OverlayMouse::locateCorner(QSize defaultSize, QSize viewBox) {
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

double OverlayMouse::getScale(QSize defaultSize, QSize viewBox) {
    double result;
    QPoint corner = locateCorner(defaultSize, viewBox);

    // Since svg maintains aspect ratio, only height or width is needed
    double width = (double)viewBox.width() - 2 * corner.x();
    result       = width / (double)defaultSize.width();

    return result;
}

bool OverlayMouse::connectMouse() {
    // Start polling
    mouseConnected = false;

    if (poll.joinable())
        poll.join();

    mouseConnected = true;
    poll           = std::thread(&OverlayMouse::paintLoop, this);

    return true;
}

void OverlayMouse::paintLoop() {
    while (mouseConnected) {
        update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / refresh_rate));
    }
}

OverlayMouse::~OverlayMouse() {
    mouseConnected = false;
    poll.join();
}

void OverlayMouse::paintButtons(QPaintDevice* device, QPoint corner, double scale) {
    static constexpr frozen::unordered_map<uint8_t, std::pair<std::string_view, QPoint>, 5> button_map = {
        {utils::key_code::LBUTTON, {"left_button", {10, 0}}},
        {utils::key_code::RBUTTON, {"right_button", {512, 0}}},
        {utils::key_code::MBUTTON, {"middle_button", {415, 273}}},
        {utils::key_code::X1BUTTON, {"x_button", {2, 735}}},
        {utils::key_code::X2BUTTON, {"x_button", {41, 960}}}};

    std::lock_guard<std::mutex> lock(data_mutex);
    (void)utils::handle_event(handler.get());
    for (const auto& [button, value] : handler->mouse) {
        if (!value) {
            continue;
        }
        for (const auto& [mask, asset] : button_map) {
            if (mask == button) {
                const QPoint& location = QPoint(std::round((double)asset.second.x() * scale) + corner.x(),
                    std::round((double)asset.second.y() * scale) + corner.y());
                paintAsset(asset.first.data(), location, device, scale);
            }
        }
    }
}

void OverlayMouse::paintAsset(std::string name, const QPoint& place, QPaintDevice* device, const double& scale) {
    name = ":/mouse/" + name + ".svg";
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

void OverlayMouse::paintTouch(QPaintDevice* device, QPoint corner, double scale) {
    QPoint tl{50, 50};
    float height = 139, width = 139;

#if 0
    POINT ptOld;
    GetCursorPos(&ptOld);

    const QPoint& location = QPoint(
        (int)utils::round(((double)tl.x() + (double)((unsigned)ptOld.x % (unsigned)width)) * scale) + corner.rx(),
        (int)utils::round(((double)tl.y() + (double)((unsigned)ptOld.y % (unsigned)height)) * scale) + corner.ry());
#ifndef NDEBUG
    std::cout << "\nX: " << location.x() << " Y:" << location.y() << '\n';
#endif
    paintAsset("cursor", location, device, scale);
#endif
}
