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

#ifndef OVERLAY_MOUSE_HPP
#define OVERLAY_MOUSE_HPP

#include <vnepogodin/input_data.hpp>
#include <vnepogodin/overlay.hpp>

#include <QWidget>

namespace vnepogodin {
class OverlayMouse final : public Overlay {
    Q_OBJECT

 public:
    explicit OverlayMouse(QWidget* parent = nullptr) : Overlay(parent) { }

 private:
    /** Private Members */
    std::mutex data_mutex;

    std::unique_ptr<input_data> handler = std::make_unique<input_data>();

    const char* getSvgPath() const noexcept override;

    /**
    * Helper function for paintEvent that paints buttons that are on.
    */
    void paintButtons(QPaintDevice* device, const QPoint& corner, const double& scale) override;

    /**
    * Paints cursor onto touch points.
    */
    void paintTouch(QPaintDevice* device, QPoint corner, double scale);
};
}  // namespace vnepogodin

#endif  // OVERLAY_MOUSE_HPP
