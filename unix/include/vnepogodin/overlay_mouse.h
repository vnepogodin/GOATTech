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

#ifndef OVERLAY_MOUSE_H
#define OVERLAY_MOUSE_H

#include <ui_overlay_mouse.h>
#include <vnepogodin/logger.hpp>

#include <thread>

#include <QWidget>

namespace Ui {
class OverlayMouse;
}

namespace vnepogodin {
class OverlayMouse final : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(OverlayMouse)
 public:
    explicit OverlayMouse(QWidget* parent = nullptr);
    virtual ~OverlayMouse();

 protected:
   /**
   * @fn paintEvent
   * ----------
   * @description: Overloads default paint constructor in order to render
   * mouse overlay svgs.
   */
    void paintEvent(QPaintEvent*) override;

 private:
    /** Private Members */
    bool mouseConnected    = false;
    const int refresh_rate = 500;  // Frequency of mouse input checking in hertz
    std::thread poll;

    std::unique_ptr<Ui::OverlayMouse> ui = std::make_unique<Ui::OverlayMouse>();

   /**
   * @fn connectMouse
   * ----------
   * @description: Trys to connect to mouse.
   * @returns: true if mouse is connected and false if no connection can be
   * made
   */
    bool connectMouse();

   /**
   * @fn paintButtons
   * ----------
   * @description: Helper function for paintEvent that paints buttons that are
   * on.
   */
    void paintButtons(QPaintDevice* device, QPoint corner, double scale);

   /**
   * @fn paintFeatures
   * ----------
   * @description: Helper function for paintEvent that paints mouse features
   * that are on.
   */
    void paintFeatures(QPaintDevice* device, QPoint corner, double scale);

   /**
   * @fn getScale
   * ----------
   * @returns: Returns the scale of the base mouse svg.
   * @p defaultSize is provided by a member function of the svg renderer
   * @p viewBox is the size of the widget the svg is drawn on
   */
    double getScale(QSize defaultSize, QSize viewBox);

   /**
   * @fn locateCorner
   * ----------
   * @returns: Locates the corner point of the base mouse svg on the widget.
   * @p defaultSize is provided by a member function of the svg renderer
   * @p viewBox is the size of the widget the svg is drawn on
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
    inline void paintAsset(std::string name, const QPoint& place, QPaintDevice* device, const double& scale);

   /**
   * @fn paintTouch
   * ----------
   * @description: Paints cursor onto touch points.
   */
    void paintTouch(QPaintDevice* device, QPoint corner, double scale);
};
}  // namespace vnepogodin

#endif  // OVERLAY_MOUSE_H
