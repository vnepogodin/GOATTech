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

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "thirdparty/json.hpp"
#include "ui_settings.h"

#include <QSettings>
#include <QWidget>

namespace Ui {
class Settings;
}

namespace vnepogodin {
class Settings : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(Settings)

 public:
    explicit Settings(QWidget* parent = nullptr);
    virtual ~Settings();

 private slots:
    void on_cancel();
    void on_apply();

    void on_run();
    void on_hideKeyboard();
    void on_hideMouse();

    void on_lineEdit_editingFinished();

    void on_lineEdit_2_editingFinished();

    void on_lineEdit_3_editingFinished();

 private:
    QSettings* m_settings;
    Ui::Settings* m_ui;

    nlohmann::json json;
};
}  // namespace vnepogodin
#endif  // SETTINGS_HPP
