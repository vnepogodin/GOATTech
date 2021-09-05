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

#include <vnepogodin/input_data.hpp>
#include <vnepogodin/overlay_keyboard.hpp>
#include <vnepogodin/overlay_mouse.hpp>
#include <vnepogodin/uiohook_helper.hpp>

#include <charconv>
#include <chrono>
#include <string_view>

#ifndef _WIN32
#include <frozen/unordered_map.h>
#include <vnepogodin/thirdparty/HTTPRequest.hpp>
#endif
#include <vnepogodin/thirdparty/json.hpp>

namespace vnepogodin {
namespace utils {
    namespace key_code {
        static constexpr std::uint8_t LBUTTON = MOUSE_BUTTON1;
        /* clang-format off */
#ifndef _WIN32
        static constexpr std::uint8_t RBUTTON = MOUSE_BUTTON3;
        static constexpr std::uint8_t MBUTTON = MOUSE_BUTTON2;
#else
        static constexpr std::uint8_t RBUTTON = MOUSE_BUTTON2;
        static constexpr std::uint8_t MBUTTON = MOUSE_BUTTON3;
#endif
        /* clang-format on */
        static constexpr std::uint8_t X1BUTTON = MOUSE_BUTTON4;
        static constexpr std::uint8_t X2BUTTON = MOUSE_BUTTON5;

        static constexpr std::uint32_t W         = VC_W;
        static constexpr std::uint32_t A         = VC_A;
        static constexpr std::uint32_t S         = VC_S;
        static constexpr std::uint32_t D         = VC_D;
        static constexpr std::uint32_t Q         = VC_Q;
        static constexpr std::uint32_t E         = VC_E;
        static constexpr std::uint32_t SHIFT     = VC_SHIFT_L;
        static constexpr std::uint32_t CONTROL   = VC_CONTROL_L;
        static constexpr std::uint32_t SPACEBAR  = VC_SPACE;
        static constexpr std::uint32_t UNDEFINED = VC_UNDEFINED;
    };  // namespace key_code

    namespace {
#ifndef _WIN32
        static constexpr frozen::unordered_map<std::uint32_t, std::string_view, 14> code_list = {
#else
        static std::unordered_map<std::uint32_t, std::string_view> code_list = {
#endif
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

        static inline int parse_int(const std::string_view& str) {
            int result = 0;
            std::from_chars(str.data(), str.data() + str.size(), result);
            return result;
        }

        static inline int get_proper_value(const nlohmann::json& value) {
            if (value.is_string()) {
                const auto& str = value.get<std::string>();
                return parse_int(str);
            }
            return value.get<int>();
        }
    }  // namespace
    template <class T>
    constexpr void load_key(const nlohmann::json& json, T* object, const std::string& key) noexcept {
        [[maybe_unused]] constexpr bool is_valid = std::is_same<T, OverlayKeyboard>::value || std::is_same<T, OverlayMouse>::value;
        static_assert(is_valid, "Unknown type");

        if (json.contains(key)) {
            object->setVisible(!get_proper_value(json[key]));
            return;
        }

        if constexpr (std::is_same<T, OverlayKeyboard>::value) {
            object->setVisible(0);
        }
    }

    constexpr bool handle_event(input_data* handler) noexcept {
        uiohook_event* event = uiohook::buf.read<uiohook_event>();
        if (event) {
            handler->dispatch_uiohook_event(event);
            return true;
        }

        return false;
    }

    inline void send_json() noexcept {
#ifndef _WIN32
        static constexpr auto URL = "http://torrenttor.ru/api1/post/";
        http::Request request(URL);

        nlohmann::json json{{"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())}};
        [[maybe_unused]] const auto& response = request.send("POST", json.dump(),
            {"Content-Type: application/json"});
#endif
    }
};  // namespace utils
}  // namespace vnepogodin

#endif  // UTILS_HPP
