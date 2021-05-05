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

#ifndef OVERLAY_GAMEPAD_H
#define OVERLAY_GAMEPAD_H

#include <JoyShockLibrary.h>
#include <QMouseEvent>
#include <QObject>
#include <QPaintDevice>
#include <QPainter>
#include <QString>
#include <QSvgRenderer>
#include <QWidget>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>

namespace Ui {
class Overlay;
}

class Overlay : public QWidget {
    Q_OBJECT

 public:
    explicit Overlay(QWidget* parent = nullptr);
    ~Overlay();

 protected:
    /**
     * @fn paintEvent
     * ----------
     * @description: Overloads default paint constructor in order to render gamepad overlay svgs.
     */
    void paintEvent(QPaintEvent*) override;

    /** Private Functions */
 private:
    /**
     * @fn getGamepad
     * ----------
     * @description: Searches for dualshock 4 in hid devices, and choose first one.
     * @returns: -1 if no gamepad is found or the device id
     */
    int getGamepad();

    /**
     * @fn connectGamepad
     * ----------
     * @description: Trys to connect to dualshock 4.
     * @returns: true if gamepad is connected and false if no connection can be made
     */
    bool connectGamepad();

    /**
     * @fn paintButtons
     * ----------
     * @description: Helper function for paintEvent that paints buttons that are on.
     */
    void paintButtons(QPaintDevice* device, QPoint corner, double scale);

    /**
     * @fn paintAxes
     * ----------
     * @description: Helper function for paintEvent that paints axes that move.
     */
    void paintAxes(QPaintDevice* device, QPoint corner, double scale);

    /**
     * @fn paintFeatures
     * ----------
     * @description: Helper function for paintEvent that paints dualshock features that are on.
     */
    void paintFeatures(QPaintDevice* device, QPoint corner, double scale);

    /**
     * @fn getScale
     * ----------
     * @returns: Returns the scale of the base dualshock svg.
     * @param defaultSize is provided by a member function of the svg renderer
     * @param viewBox is the size of the widget the svg is drawn on
     */
    double getScale(QSize defaultSize, QSize viewBox);

    /**
     * @fn locateCorner
     * ----------
     * @returns: Locates the corner point of the base dualshock svg on the widget.
     * @param defaultSize is provided by a member function of the svg renderer
     * @param viewBox is the size of the widget the svg is drawn on
     */
    QPoint locateCorner(QSize defaultSize, QSize viewBox);

    /**
     * @fn paintLoop
     * ----------
     * @description: Calls repaint on a thread for the specified refresh rate.
     */
    void paintLoop();

    /**
     * @fn paintAsset
     * ----------
     * @description: Paints an svg asset with a transparent background.
     */
    void paintAsset(std::string name, QPoint place, QPaintDevice* device, QPoint corner,
        double scale);

    /**
     * @fn paintTouch
     * ----------
     * @description: Paints cursor onto touch points.
     */
    void paintTouch(QPaintDevice* device, QPoint corner, double scale);

    /** Private Members */
 private:
    Ui::Overlay* ui;
    int deviceID          = -1;  // -1 signifies no device
    bool gamepadConnected = false;
    int refresh_rate      = 500;  // Frequency of controller input checking in hertz
    std::thread poll;
};

#endif  // OVERLAY_GAMEPAD_H
