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

#include "settings.hpp"

#include <charconv>
#include <iostream>

using namespace vnepogodin;

namespace vnepogodin {
namespace utils {
    static std::vector<std::string> fromStringList(const QStringList& string_list) {
        const auto& len = string_list.size();
        std::vector<std::string> keys(len);

        for (int i = 0; i < len; ++i) {
            keys[i] = string_list[i].toStdString();
        }

        return keys;
    }

    static void toObject(const QSettings* const settings, nlohmann::json& obj) {
        for (const auto& _ : settings->childKeys()) {
            if (!_.size()) {
                return;
            }
            QVariant value        = settings->value(_);
            const std::string key = _.toStdString();
            switch ((QMetaType::Type)value.type()) {
            case QMetaType::Bool:
                obj[key] = value.toBool();
                break;
            case QMetaType::Int:
                obj[key] = value.toInt();
                break;
            case QMetaType::Double:
                obj[key] = value.toDouble();
                break;
            case QMetaType::QString:
                obj[key] = value.toString().toStdString();
                break;
            case QMetaType::QStringList:
                obj[key] = fromStringList(value.toStringList());
                break;
            case QMetaType::QByteArray:
                obj[key] = QString::fromUtf8(value.toByteArray().toBase64()).toStdString();
                break;
            default:
                break;
            }
        }
    }

    static inline int get_propervalue(const nlohmann::json& value) {
        if (value.is_string()) {
            const auto& str = value.get<std::string>();
            int result      = 0;
            std::from_chars(str.data(), str.data() + str.size(), result);
            return result;
        } else {
            return value.get<int>();
        }
    }

    static inline void fromObject(const nlohmann::json& obj, QSettings* settings) {
        for (const auto& [key, value] : obj.items()) {
            settings->setValue(key.c_str(), get_propervalue(value));
        }
    }

    static inline int parse_int(const std::string_view& str) {
        int result = 0;
        std::from_chars(str.data(), str.data() + str.size(), result);
        return result;
    }

    static inline std::string parse_string(const int& value) {
        return std::to_string(value);
    }

    static inline void load_key(nlohmann::json& json, QCheckBox* box, const std::string& key) {
        if (json.contains(key)) {
            box->setChecked(get_propervalue(json[key]));
            return;
        }
        json[key] = 0;
        if (key == "hideMouse") {
            json[key] = 1;
        }
    }

    static inline void load_text(nlohmann::json& json, QLineEdit* text, const std::string& key) {
        if (json.contains(key)) {
            text->setText(parse_string(get_propervalue(json[key])).c_str());
            return;
        }
        json[key] = 50;
        if (key == "radius") {
            json[key] = 20;
        }
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

Settings::Settings(QWidget* parent)
  : QWidget(parent),
    m_settings(new QSettings(QSettings::UserScope)),
    m_ui(new Ui::Settings) {
    QSettings::setDefaultFormat(QSettings::NativeFormat);

    m_ui->setupUi(this);
    connect(m_ui->cancel, SIGNAL(clicked()), this, SLOT(on_cancel()));
    connect(m_ui->ok, SIGNAL(clicked()), this, SLOT(on_apply()));

    connect(m_ui->isRun, SIGNAL(clicked()), this, SLOT(on_run()));
    connect(m_ui->hideKeyboard, SIGNAL(clicked()), this, SLOT(on_hideKeyboard()));
    connect(m_ui->hideMouse, SIGNAL(clicked()), this, SLOT(on_hideMouse()));

    utils::toObject(m_settings, json);
    utils::load_key(json, m_ui->isRun, "isRun");
    utils::load_key(json, m_ui->hideKeyboard, "hideKeyboard");
    utils::load_key(json, m_ui->hideMouse, "hideMouse");
    utils::load_text(json, m_ui->lineEdit, "radius");
    utils::load_text(json, m_ui->lineEdit_2, "centerX");
    utils::load_text(json, m_ui->lineEdit_3, "centerY");

    // set window size
    this->resize(size().width() * 0.8, size().height() * 0.7);
}

Settings::~Settings() {
    delete m_settings;
    delete m_ui;
}

void Settings::on_lineEdit_3_editingFinished() {
    const auto& value = utils::parse_int(m_ui->lineEdit_3->text().toStdString());
    json["centerY"]   = value;
}

void Settings::on_lineEdit_2_editingFinished() {
    const auto& value = utils::parse_int(m_ui->lineEdit_2->text().toStdString());
    json["centerX"]   = value;
}

void Settings::on_lineEdit_editingFinished() {
    json["radius"] = m_ui->lineEdit->text().toStdString();
}

void Settings::on_cancel() {
    close();
}

void Settings::on_apply() {
    utils::fromObject(json, m_settings);
    close();
}

void Settings::on_run() {
    json["isRun"] = m_ui->isRun->isChecked();
}

void Settings::on_hideKeyboard() {
    json["hideKeyboard"] = m_ui->hideKeyboard->isChecked();
}

void Settings::on_hideMouse() {
    json["hideMouse"] = m_ui->hideMouse->isChecked();
}
