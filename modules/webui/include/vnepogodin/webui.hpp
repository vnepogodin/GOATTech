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

#ifndef WEBUI_HPP
#define WEBUI_HPP

#include <vnepogodin/webuihandler.hpp>

#include <QWebChannel>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineView>
#include <QWidget>

class JsInterface;

class Webui : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(Webui)
 public:
    explicit Webui(QWidget* parent = nullptr);
    virtual ~Webui();

    void paintLoop();
    void show();

 private:
    const int refresh_rate = 500;  // Frequency in hertz
    JsInterface* m_jsinterface;

    std::unique_ptr<QWebChannel> m_channel;
    std::unique_ptr<WebUiHandler> m_handler;
    std::unique_ptr<QWebEngineProfile> m_profile;
    std::unique_ptr<QWebEnginePage> m_page;
    std::unique_ptr<QWebEngineView> m_view;
};

#endif  // WEBUI_HPP
