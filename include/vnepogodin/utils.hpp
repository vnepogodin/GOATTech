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

#ifndef UTILS_HPP
#define UTILS_HPP

#include <vnepogodin/logger.hpp>
#include <array>
#include <string_view>
#include <unordered_map>

namespace utils {
namespace key_code {
    constexpr std::uint8_t LBUTTON  = 0x01;
    constexpr std::uint8_t RBUTTON  = 0x02;
    constexpr std::uint8_t MBUTTON  = 0x04;
    constexpr std::uint8_t X1BUTTON = 0x05;
    constexpr std::uint8_t X2BUTTON = 0x06;

    constexpr std::uint32_t W         = 0x57;
    constexpr std::uint32_t A         = 0x41;
    constexpr std::uint32_t S         = 0x53;
    constexpr std::uint32_t D         = 0x44;
    constexpr std::uint32_t Q         = 0x51;
    constexpr std::uint32_t E         = 0x45;
    constexpr std::uint32_t SHIFT     = 0xA0;
    constexpr std::uint32_t CONTROL   = 0xA2;
    constexpr std::uint32_t SPACEBAR  = 0x20;
    constexpr std::uint32_t UNDEFINED = 0x07;
};  // namespace key_code

inline int round(const double& val) {
    return (val < 0) ? static_cast<int>(std::ceil(val - 0.5)) : static_cast<int>(std::floor(val + 0.5));
}

inline std::uint32_t get_key(Logger* logger) {
    const std::array<std::uint32_t, 14> code_list = {
        key_code::W,
        key_code::A,
        key_code::S,
        key_code::D,
        key_code::Q,
        key_code::E,
        key_code::SHIFT,
        key_code::CONTROL,
        key_code::SPACEBAR,
        key_code::LBUTTON,
        key_code::RBUTTON,
        key_code::MBUTTON,
        key_code::X1BUTTON,
        key_code::X2BUTTON};
    for (const auto& code : code_list) {
#ifdef _WIN32
        if (GetAsyncKeyState(code)) {
            auto& j = logger->get("numOfTouch");
            switch (code) {
            case key_code::X1BUTTON:
                j.at("x1_button") = j["x1_button"].get<int>() + 1;
                break;
            case key_code::X2BUTTON:
                j.at("x2_button") = j["x2_button"].get<int>() + 1;
                break;
            case key_code::LBUTTON:
                j.at("left_button") = j["left_button"].get<int>() + 1;
                break;
            case key_code::RBUTTON:
                j.at("right_button") = j["right_button"].get<int>() + 1;
                break;
            case key_code::MBUTTON:
                j.at("middle_button") = j["middle_button"].get<int>() + 1;
                break;
            default:
                break;
            }
            return code;
        }
#endif
    }
    return key_code::UNDEFINED;
}
};  // namespace utils

#endif  // UTILS_HPP
