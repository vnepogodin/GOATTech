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

#include <vnepogodin/logger.hpp>
#include <vnepogodin/mainwindow.hpp>
#include <vnepogodin/utils.hpp>

#include <iostream>

#include <QByteArray>
#include <QMetaType>
#include <QScreen>
#include <QSettings>
#include <QString>

using namespace vnepogodin;

void MainWindow::createMenu() noexcept {
    m_tray_menu = std::make_unique<QMenu>(this);

    m_tray_menu->addAction("Settings", [&] {
        if (m_process_settings->state() == QProcess::NotRunning) {
            m_process_settings->open();
        }
    });
    m_tray_menu->addAction("Quit", [&] {
        QApplication::quit();
    });
}

void MainWindow::iconActivated(const QSystemTrayIcon::ActivationReason& reason) {
    if (reason != QSystemTrayIcon::Trigger) {
        return;
    }
    if (m_activated[0] != 2) {
        m_ui->keyboard->setVisible(m_activated[0]);
        m_activated[0] = !m_activated[0];
    }
    if (m_activated[1] != 2) {
        m_ui->mouse->setVisible(m_activated[1]);
        m_activated[1] = !m_activated[1];
    }
}

static inline void stop_process(QProcess* proc) {
    if (proc->state() == QProcess::Running) {
        proc->terminate();
        if (!proc->waitForFinished()) {
            std::cerr << "Process failed to terminate\n";
        }
    }
}

namespace vnepogodin {
namespace detail {
    static void to_object(const QSettings* const settings, nlohmann::json& obj) {
        for (const auto& _ : settings->childKeys()) {
            if (!_.size()) {
                return;
            }

            const auto& value = settings->value(_);
            const auto& key   = _.toStdString();
            switch ((QMetaType::Type)value.type()) {
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
}  // namespace detail
}  // namespace vnepogodin

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent) {
    m_ui->setupUi(this);
    m_process_settings = std::make_unique<QProcess>(this);
    m_uiohock          = std::thread(uiohook::start);

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NativeWindow);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::BypassWindowManagerHint | Qt::SplashScreen);

#ifdef _WIN32
    const auto& m_hwnd = (HWND)winId();
    SetForegroundWindow(m_hwnd);
    SetWindowPos(m_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
#endif

    m_tray_icon = std::make_unique<QSystemTrayIcon>(this);
    m_process_settings->setProgram("GOATTech-settings");
    setMouseTracking(true);

    // Set window position
    const auto& rec         = QApplication::primaryScreen()->geometry();
    const auto& window_size = this->size();
    move(0, rec.height() - window_size.height());

    // Calculate overlay percentage of the window
    static constexpr float perc_of_window = 0.18F;
    const auto& perc_height               = static_cast<float>(rec.height()) * perc_of_window;
    const auto& perc_width                = static_cast<float>(rec.width()) * perc_of_window;

    // Set proper widget size
    static constexpr float fixed_scale = 1.5F;
    const int& size                    = static_cast<int>(qMin(perc_height, perc_width));

    m_ui->keyboard->setFixedSize(size, size);
    const int& mouse_fixed_size = static_cast<int>(static_cast<float>(size) / fixed_scale);
    m_ui->mouse->setFixedSize(mouse_fixed_size, mouse_fixed_size);

    // Tray icon menu
    createMenu();
    m_tray_icon->setContextMenu(m_tray_menu.get());
    m_tray_icon->setToolTip("GOATTech");
    m_tray_icon->setIcon(QIcon("icon.png"));
    m_tray_icon->show();

    QSettings settings(QSettings::UserScope);
    nlohmann::json json;
    detail::to_object(&settings, json);
    utils::load_key(json, m_ui->keyboard, "hideKeyboard");
    utils::load_key(json, m_ui->mouse, "hideMouse");

    if (json.contains("inputDevice")) {
        m_recorder = std::make_unique<vnepogodin::Recorder>(json["inputDevice"].get<std::string>());
        m_recorder->record();
    }

    m_activated[0] = (m_ui->keyboard->isHidden()) ? 2 : 0;
    m_activated[1] = (m_ui->mouse->isHidden()) ? 2 : 0;

    connect(m_tray_icon.get(), &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
    connect(QCoreApplication::instance(), &QApplication::aboutToQuit, this, &MainWindow::close);
}

void MainWindow::closeEvent(QCloseEvent* event) {
    stop_process(m_process_settings.get());
    if (m_recorder) {
        m_recorder->stop();
    }

    utils::send_json();
    if (m_uiohock.joinable()) {
        uiohook::stop();
        m_uiohock.join();
    }

    QWidget::closeEvent(event);
}
