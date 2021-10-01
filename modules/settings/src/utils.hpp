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

#include "thirdparty/json.hpp"

#include <charconv>
#include <exception>
#include <string_view>

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QMetaType>
#include <QSettings>
#include <QVariant>

namespace vnepogodin {
namespace detail {
    void to_object(const QSettings* const settings, nlohmann::json& obj) {
        for (const auto& _ : settings->childKeys()) {
            if (!_.size()) {
                return;
            }

            const auto& value = settings->value(_);
            const auto& key   = _.toStdString();
            switch ((QMetaType::Type)value.typeId()) {
            case QMetaType::Bool:
                obj[key] = value.toBool();
                break;
            case QMetaType::Int:
                obj[key] = value.toInt();
                break;
            case QMetaType::QString:
                obj[key] = value.toString().toStdString();
                break;
            case QMetaType::QByteArray:
                obj[key] = QString::fromUtf8(value.toByteArray().toBase64()).toStdString();
                break;
            default:
                break;
            }
        }
    }

    inline void from_object(const nlohmann::json& obj, QSettings* settings) {
        for (const auto& [key, value] : obj.items()) {
            if (value.is_string()) {
                settings->setValue(key.c_str(), value.get<std::string>().c_str());
                continue;
            }
            settings->setValue(key.c_str(), value.get<int>());
        }
    }
}  // namespace detail

namespace utils {
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

    template <class T>
    static inline void load_key(nlohmann::json& json, T* object, const std::string& key) {
        if (json.contains(key)) {
            if constexpr (std::is_same<T, QCheckBox>::value) {
                object->setChecked(get_proper_value(json[key]));
                return;
            } else if constexpr (std::is_same<T, QComboBox>::value) {
                const auto& value = json[key].get<std::string>();

                const int& index = object->findText(value.c_str());
                if (index != -1) {
                    object->setCurrentIndex(index);
                }
                return;
            }
            throw std::runtime_error("Unknown type");
        }
        if constexpr (std::is_same<T, QCheckBox>::value) {
            json[key] = static_cast<int>(key == "hideKeyboard");
            return;
        } else if constexpr (std::is_same<T, QComboBox>::value) {
            // no-op
            return;
        }
        throw std::runtime_error("Unknown type");
    }
    //    static inline void load_macaddress(nlohmann::json& json) {
    //        if (json.contains("mac")) {
    //
    //            return;
    //        }
    //        json[key] = 0;
    //        if (key == "hideMouse") {
    //            json[key] = 1;
    //        }
    //    }
}  // namespace utils
}  // namespace vnepogodin

#endif  // UTILS_HPP
