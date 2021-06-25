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
#include <sstream>

#include <QByteArray>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QMetaType>
#include <QSettings>
#include <QString>
#include <QVariant>

static vnepogodin::Logger logger;

using namespace vnepogodin;
static inline std::uint32_t handle_key(std::uint32_t key_stroke) {
    for (const auto& code : utils::code_list) {
        if (code.first == key_stroke) {
            logger.add_key(code.second);
            return code.first;
        }
    }

    return utils::key_code::UNDEFINED;
}

/* Qt just uses the QWidget* parent as transient parent for native
 * platform dialogs. This makes it impossible to make them transient
 * to a bare QWindow*. So we catch the show event for the QDialog
 * and setTransientParent here instead. */
bool MainWindow::event(QEvent* ev) {
    if (ev->type() == QEvent::Timer) {
        logger.write();
        return true;
    }
    if (ev->type() == QEvent::KeyPress) {
        handle_key(static_cast<QKeyEvent*>(ev)->key());
        return true;
    }
    if (ev->type() == QEvent::MouseButtonPress) {
        const auto& button = static_cast<QMouseEvent*>(ev)->button();
        switch (button) {
        case Qt::XButton1:
            handle_key(vnepogodin::utils::key_code::X1BUTTON);
            break;
        case Qt::XButton2:
            handle_key(vnepogodin::utils::key_code::X2BUTTON);
            break;
        default:
            handle_key(button);
            break;
        }
        return true;
    }

    return QObject::event(ev);
}

void MainWindow::createMenu() noexcept {
    m_tray_menu = std::make_unique<QMenu>(this);

    m_tray_menu->addAction("Settings", [&] {
        if (m_process_settings->state() == QProcess::NotRunning)
            m_process_settings->open();
    });
    m_tray_menu->addAction("Quit", [&] {
        QApplication::quit();
    });
}

void MainWindow::iconActivated(const QSystemTrayIcon::ActivationReason& reason) {
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        if (m_activated[0] != 2) {
            m_ui->keyboard->setVisible(m_activated[0]);
            m_activated[0] = !m_activated[0];
        }
        if (m_activated[1] != 2) {
            m_ui->mouse->setVisible(m_activated[1]);
            m_activated[1] = !m_activated[1];
        }
        break;
    default:
        break;
    }
}

static void stop_process(QProcess* proc) {
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

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NativeWindow);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::BypassWindowManagerHint | Qt::SplashScreen);

    m_tray_icon = std::make_unique<QSystemTrayIcon>(this);
    m_process_settings->setProgram("GOATTech-settings");
    m_timer = startTimer(100);
    setMouseTracking(true);

    // Set window position
    const auto& rec         = QApplication::desktop()->geometry();
    const auto& window_size = this->size();
    move(0, rec.height() - window_size.height());

    // Calculate overlay percentage of the window
    static constexpr float perc_of_window = 0.18F;
    const auto& perc_height               = rec.height() * perc_of_window;
    const auto& perc_width                = rec.width() * perc_of_window;

    // Set proper widget size
    const int& size = qMin(perc_height, perc_width);

    m_ui->keyboard->setFixedSize(size, size);
    m_ui->mouse->setFixedSize(size / 1.5, size / 1.5);

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
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(aboutToQuit()));
}

void MainWindow::aboutToQuit() {
    killTimer(m_timer);
    stop_process(m_process_settings.get());

    logger.close();
    m_recorder->stop();

    utils::send_json();

    close();
}
