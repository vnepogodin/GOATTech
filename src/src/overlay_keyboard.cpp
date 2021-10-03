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

#include <vnepogodin/overlay_keyboard.hpp>
#include <vnepogodin/utils.hpp>

#include <cmath>

using namespace vnepogodin;

const char* OverlayKeyboard::getSvgPath() const noexcept {
    return ":keyboard/";
}

void OverlayKeyboard::paintButtons(QPaintDevice* device, const QPoint& corner, const double& scale) {
#ifndef _WIN32
    static constexpr frozen::unordered_map<uint32_t, std::pair<std::string_view, QPoint>, 9> button_map = {
#else
    static std::unordered_map<uint32_t, std::pair<std::string_view, QPoint>> button_map = {
#endif
        {utils::key_code::W, {"w_button", {384, 0}}},
        {utils::key_code::A, {"a_button", {169, 182}}},
        {utils::key_code::S, {"s_button", {338, 182}}},
        {utils::key_code::D, {"d_button", {508, 182}}},
        {utils::key_code::Q, {"q_button", {210, 0}}},
        {utils::key_code::E, {"e_button", {552, 0}}},
        {utils::key_code::SHIFT, {"shift_button", {0, 182}}},
        {utils::key_code::CONTROL, {"ctrl_button", {23, 360}}},
        {utils::key_code::SPACEBAR, {"space_button", {192, 360}}}
    };

    std::lock_guard<std::mutex> lock(data_mutex);
    (void)utils::handle_event(handler.get());
    for (const auto& [button, value] : handler->keyboard) {
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
