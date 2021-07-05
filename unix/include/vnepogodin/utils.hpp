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

#ifndef UTILS_HPP
#define UTILS_HPP

#include <vnepogodin/overlay.hpp>
#include <vnepogodin/overlay_mouse.hpp>
#include <vnepogodin/thirdparty/HTTPRequest.hpp>
#include <vnepogodin/thirdparty/json.hpp>

#include <charconv>
#include <chrono>
#include <exception>
#include <string_view>
#include <unordered_map>

namespace vnepogodin {
namespace utils {
    namespace key_code {
        static constexpr std::uint8_t LBUTTON  = 0x01;
        static constexpr std::uint8_t RBUTTON  = 0x02;
        static constexpr std::uint8_t MBUTTON  = 0x04;
        static constexpr std::uint8_t X1BUTTON = 0x05;
        static constexpr std::uint8_t X2BUTTON = 0x06;

        static constexpr std::uint32_t W         = 0x57;
        static constexpr std::uint32_t A         = 0x41;
        static constexpr std::uint32_t S         = 0x53;
        static constexpr std::uint32_t D         = 0x44;
        static constexpr std::uint32_t Q         = 0x51;
        static constexpr std::uint32_t E         = 0x45;
        static constexpr std::uint32_t SHIFT     = 0xA0;
        static constexpr std::uint32_t CONTROL   = 0xA2;
        static constexpr std::uint32_t SPACEBAR  = 0x20;
        static constexpr std::uint32_t UNDEFINED = 0x07;
    };  // namespace key_code

    static const std::unordered_map<std::uint32_t, std::string_view> code_list = {
        {vnepogodin::utils::key_code::W, "w_button"},
        {vnepogodin::utils::key_code::A, "a_button"},
        {vnepogodin::utils::key_code::S, "s_button"},
        {vnepogodin::utils::key_code::D, "d_button"},
        {vnepogodin::utils::key_code::Q, "q_button"},
        {vnepogodin::utils::key_code::E, "e_button"},
        {vnepogodin::utils::key_code::SHIFT, "shift_button"},
        {vnepogodin::utils::key_code::CONTROL, "ctrl_button"},
        {vnepogodin::utils::key_code::SPACEBAR, "space_button"},
        {vnepogodin::utils::key_code::LBUTTON, "left_button"},
        {vnepogodin::utils::key_code::RBUTTON, "right_button"},
        {vnepogodin::utils::key_code::MBUTTON, "middle_button"},
        {vnepogodin::utils::key_code::X1BUTTON, "x_button"},
        {vnepogodin::utils::key_code::X2BUTTON, "x_button"}};

    inline int round(const double& val) noexcept {
        return (val < 0) ? static_cast<int>(std::ceil(val - 0.5)) : static_cast<int>(std::floor(val + 0.5));
    }

    static inline int parse_int(const std::string_view& str) {
        int result = 0;
        std::from_chars(str.data(), str.data() + str.size(), result);
        return result;
    }

    static inline int get_propervalue(const nlohmann::json& value) {
        if (value.is_string()) {
            const auto& str = value.get<std::string>();
            return parse_int(str);
        }
        return value.get<int>();
    }

    template <class T>
    inline void load_key(const nlohmann::json& json, T* object, const std::string& key) noexcept(false) {
        constexpr bool is_valid = std::is_same<T, Overlay>::value || std::is_same<T, OverlayMouse>::value;
        if (json.contains(key)) {
            if constexpr (is_valid) {
                object->setVisible(!get_propervalue(json[key]));
                return;
            }
            throw std::runtime_error("Unknown type");
        }

        if constexpr (is_valid) {
            if constexpr (std::is_same<T, Overlay>::value)
                object->setVisible(0);
            return;
        }
        throw std::runtime_error("Unknown type");
    }

    inline std::uint32_t get_key() noexcept {
        //for (const auto& code : code_list) {
        //}
        return key_code::UNDEFINED;
    }

    inline void send_json() noexcept {
        static constexpr auto URL = "http://torrenttor.ru/api1/post/";
        http::Request request(URL);

        nlohmann::json json{{"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())}};
        [[maybe_unused]] const auto& response = request.send("POST", json.dump(),
            {"Content-Type: application/json"});
    }
};  // namespace utils
}  // namespace vnepogodin

#endif  // UTILS_HPP