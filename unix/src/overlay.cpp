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
#include <vnepogodin/utils.hpp>

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

Overlay::Overlay(QWidget* parent) : QWidget(parent) {
    ui->setupUi(this);

    setAttribute(Qt::WA_NativeWindow);
    connectKeyboard();
}

void Overlay::paintEvent(QPaintEvent*) {
    // Initialize renderer with base asset
    QSvgRenderer renderer;

    if (keyboardConnected)
        renderer.load(QString(":/keyboard/base.svg"));
    else
        renderer.load(QString(":/keyboard/disconnected.svg"));

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
    const auto& button = utils::get_key();

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
            const QPoint& location = QPoint(std::round((double)asset.second.x() * scale) + corner.x(),
                std::round((double)asset.second.y() * scale) + corner.y());
            paintAsset(asset.first.data(), location, device, scale);
        }
    }
}

void Overlay::paintAsset(std::string name, const QPoint& place, QPaintDevice* device, const double& scale) {
    name = ":/keyboard/" + name + ".svg";
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
