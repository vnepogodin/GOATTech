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

#ifndef OVERLAY_HPP
#define OVERLAY_HPP

#include <ui_overlay.h>
#include <vnepogodin/input_data.hpp>

#include <thread>

#include <QWidget>

namespace Ui {
class Overlay;
}

namespace vnepogodin {
class Overlay : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(Overlay)
 public:
    explicit Overlay(QWidget* parent = nullptr);
    virtual ~Overlay();

 protected:
    /**
     * Overloads default paint constructor in order to render overlay's svgs.
     */
    void paintEvent(QPaintEvent*) override;

    /**
     * Paints a svg asset with a transparent background.
     */
    void paintAsset(std::string name, const QPoint& place, QPaintDevice* device, const double& scale);

 private:
    /** Private Members */
    bool connected                    = false;
    static constexpr int refresh_rate = 600;  // Frequency of input checking in hertz
    std::thread poll;

    std::unique_ptr<Ui::Overlay> ui = std::make_unique<Ui::Overlay>();

    virtual const char* getSvgPath() const noexcept = 0;

    /**
     * Tries to connect to device.
     * @return true if device is connected and false if no connection can be
     * made
     */
    bool connect() noexcept;

    /**
     * Helper function for paintEvent that paints buttons that are
     * on.
     */
    virtual void paintButtons(QPaintDevice* device, const QPoint& corner, const double& scale) = 0;

    /**
     * Helper function for paintEvent that paints device's features
     * that are on.
     */
    virtual void paintFeatures(QPaintDevice* device, const QPoint& corner, const double& scale);

    /**
     * @param defaultSize is provided by a member function of the svg renderer
     * @param viewBox is the size of the widget the svg is drawn on
     *
     * @return Returns the scale of the base svg.
     */
    double getScale(const QSize& defaultSize, const QSize& viewBox);

    /**
     * @param defaultSize is provided by a member function of the svg renderer
     * @param viewBox is the size of the widget the svg is drawn on
     *
     * @return Locates the corner point of the base svg on the widget.
     */
    QPoint locateCorner(const QSize& defaultSize, const QSize& viewBox);

    /**
     * Calls repaint on a thread for the specified refresh rate.
     */
    void paintLoop();
};
}  // namespace vnepogodin

#endif  // OVERLAY_HPP
