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

#include <vnepogodin/overlay_mouse.hpp>
#include <vnepogodin/utils.hpp>

#include <cmath>

using namespace vnepogodin;

const char* OverlayMouse::getSvgPath() const noexcept {
    return ":mouse/";
}

void OverlayMouse::paintButtons(QPaintDevice* device, const QPoint& corner, const double& scale) {
#ifndef _WIN32
    static constexpr frozen::unordered_map<uint8_t, std::pair<std::string_view, QPoint>, 5> button_map = {
#else
    static std::unordered_map<uint8_t, std::pair<std::string_view, QPoint>> button_map = {
#endif
        {utils::key_code::LBUTTON, {"left_button", {10, 0}}},
        {utils::key_code::RBUTTON, {"right_button", {512, 0}}},
        {utils::key_code::MBUTTON, {"middle_button", {415, 273}}},
        {utils::key_code::X1BUTTON, {"x_button", {2, 735}}},
        {utils::key_code::X2BUTTON, {"x_button", {41, 960}}}
    };

    std::lock_guard<std::mutex> lock(data_mutex);
    (void)utils::handle_event(handler.get());
    for (const auto& [button, value] : handler->mouse) {
        if (!value) {
            continue;
        }
        for (const auto& [mask, asset] : button_map) {
            if (mask == button) {
                const QPoint& location = QPoint(static_cast<int>(std::round(static_cast<double>(asset.second.x()) * scale)) + corner.x(),
                    static_cast<int>(std::round(static_cast<double>(asset.second.y()) * scale)) + corner.y());
                paintAsset(asset.first.data(), location, device, scale);
            }
        }
    }
}

void OverlayMouse::paintTouch(QPaintDevice* /*device*/, QPoint /*corner*/, double /*scale*/) {
#if 0
    QPoint tl{50, 50};
    float height = 139, width = 139;

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
