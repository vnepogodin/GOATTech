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

#include "settings.hpp"
#include "utils.hpp"

#include <QAudioRecorder>

using namespace vnepogodin;

Settings::Settings(QWidget* parent)
  : QWidget(parent),
    m_settings(new QSettings(QSettings::UserScope)),
    m_ui(new Ui::Settings) {
    QSettings::setDefaultFormat(QSettings::NativeFormat);

    m_ui->setupUi(this);

    // get audio devices
    QAudioRecorder audioRecorder;
    const auto& input_list = audioRecorder.audioInputs();
    std::for_each(input_list.cbegin(), input_list.cend(), [&](const auto& device) {
        m_ui->inputDevice->addItem(device, QVariant(device));
    });

    connect(m_ui->cancel, SIGNAL(clicked()), this, SLOT(on_cancel()));
    connect(m_ui->ok, SIGNAL(clicked()), this, SLOT(on_apply()));

    connect(m_ui->isRun, SIGNAL(clicked()), this, SLOT(on_run()));
    connect(m_ui->hideKeyboard, SIGNAL(clicked()), this, SLOT(on_hideKeyboard()));
    connect(m_ui->hideMouse, SIGNAL(clicked()), this, SLOT(on_hideMouse()));
    connect(m_ui->inputDevice, SIGNAL(activated(QString)), this, SLOT(on_inputDevice(QString)));

    utils::toObject(m_settings, json);
    utils::load_key(json, m_ui->isRun, "isRun");
    utils::load_key(json, m_ui->hideKeyboard, "hideKeyboard");
    utils::load_key(json, m_ui->hideMouse, "hideMouse");
    utils::load_key(json, m_ui->inputDevice, "inputDevice");

    // set window size
    this->resize(size().width() * 0.8, size().height() * 0.7);
}

Settings::~Settings() {
    delete m_settings;
    delete m_ui;
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

void Settings::on_inputDevice(const QString& text) {
    json["inputDevice"] = text.toStdString();
}
