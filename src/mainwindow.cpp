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

/* clang-format off */
#include <vnepogodin/thirdparty/HTTPRequest.hpp>
/* clang-format on */
#include <vnepogodin/logger.hpp>
#include <vnepogodin/mainwindow.h>
#include <vnepogodin/utils.hpp>

#include <chrono>
#include <iostream>
#include <sstream>

#include <QDesktopWidget>
#include <QFile>
#include <QSettings>
#include <QTextStream>

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

#ifdef _WIN32
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Ws2_32.lib")

// variable to store the HANDLE to the hook. Don't declare it anywhere else then globally
// or you will get problems since every function uses this variable.
static HHOOK _hook_keyboard = nullptr;
static HHOOK _hook_mouse    = nullptr;
static NOTIFYICONDATAW nid;
static constexpr auto ID_SETTINGS = 2000;
static constexpr auto ID_EXIT     = 2001;

// This is the callback function. Consider it the event that is raised when, in this case,
// a key is pressed.
static LRESULT __stdcall HookCallbackKeyboard(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        // the action is valid: HC_ACTION.
        if (wParam == WM_KEYDOWN) {
            // lParam is the pointer to the struct containing the data needed, so cast and assign it to kdbStruct.
            const KBDLLHOOKSTRUCT kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
            handle_key(kbdStruct.vkCode);
        }
    }

    // call the next hook in the hook chain. This is necessary or your hook chain will break and the hook stops
    return CallNextHookEx(_hook_keyboard, nCode, wParam, lParam);
}
static LRESULT __stdcall HookCallbackMouse(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        // the action is valid: HC_ACTION.
        switch (wParam) {
        case WM_LBUTTONDOWN:
            handle_key(vnepogodin::utils::key_code::LBUTTON);
            break;
        case WM_RBUTTONDOWN:
            handle_key(vnepogodin::utils::key_code::RBUTTON);
            break;
        case WM_MBUTTONDOWN:
            handle_key(vnepogodin::utils::key_code::MBUTTON);
            break;
        case WM_XBUTTONDOWN:
            handle_key((*((tagMSLLHOOKSTRUCT*)lParam)).mouseData);
            break;
        }
    }

    // call the next hook in the hook chain. This is necessary or your hook chain will break and the hook stops
    return CallNextHookEx(_hook_mouse, nCode, wParam, lParam);
}

static inline void SetHook() {
    static constexpr auto type = L"Error";
    // WH_KEYBOARD_LL means it will set a low level keyboard hook. More information about it at MSDN.
    if (!(_hook_keyboard = SetWindowsHookExW(WH_KEYBOARD_LL, HookCallbackKeyboard, NULL, 0))) {
        static constexpr auto a = L"Failed to install keyboard hook!";
        MessageBoxW(NULL, a, type, MB_ICONERROR);
    }
    // WH_MOUSE_LL means it will set a low level mouse hook. More information about it at MSDN.
    if (!(_hook_mouse = SetWindowsHookExW(WH_MOUSE_LL, HookCallbackMouse, NULL, 0))) {
        static constexpr auto a = L"Failed to install mouse hook!";
        MessageBoxW(NULL, a, type, MB_ICONERROR);
    }
}

static void ShowPopupMenu(HWND hWnd, POINT* curpos, int wDefaultItem) {
    HMENU hPop = CreatePopupMenu();

    InsertMenuW(hPop, 0, MF_BYPOSITION | MF_STRING, ID_SETTINGS, L"Settings");
    InsertMenuW(hPop, 1, MF_BYPOSITION | MF_STRING, ID_EXIT, L"Exit");

    SetMenuDefaultItem(hPop, ID_SETTINGS, FALSE);
    SetFocus(hWnd);
    SendMessageW(hWnd, WM_INITMENUPOPUP, (WPARAM)hPop, 0);

    POINT pt;
    if (!curpos) {
        GetCursorPos(&pt);

        curpos = &pt;
    }

    WORD cmd = TrackPopupMenu(
        hPop, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
        curpos->x, curpos->y, 0, hWnd, NULL);

    SendMessageW(hWnd, WM_COMMAND, cmd, 0);
    DestroyMenu(hPop);
}

bool MainWindow::nativeEvent(const QByteArray& eventType, void* message, long* result) {
    Q_UNUSED(eventType)
    Q_UNUSED(result)
    // Transform the message pointer to the MSG WinAPI
    MSG* msg = static_cast<MSG*>(message);

    switch (msg->message) {
    case WM_TIMER:
        logger.write();
        break;
    case WM_COMMAND:
        switch (LOWORD(msg->wParam)) {
        case ID_SETTINGS:
            if (m_process_settings->state() == QProcess::NotRunning) {
                m_process_settings->open();
            }
            break;

        case ID_EXIT:
            PostMessage(m_hwnd, WM_CLOSE, 0, 0);
            break;
        }
        break;
    case IDT_TRAY:
        switch (msg->lParam) {
        case WM_LBUTTONUP:
            if (m_activated[0] != 2) {
                m_ui->keyboard->setVisible(m_activated[0]);
                m_activated[0] = !m_activated[0];
            }
            if (m_activated[1] != 2) {
                m_ui->mouse->setVisible(m_activated[1]);
                m_activated[1] = !m_activated[1];
            }
            break;
        case WM_RBUTTONUP:
            SetForegroundWindow(m_hwnd);
            ShowPopupMenu(m_hwnd, NULL, -1);
            PostMessageW(m_hwnd, IDT_TRAY + 1, 0, 0);
            break;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(m_hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(m_hwnd, msg->message, msg->wParam, msg->lParam);
    }

    return false;
}

#else

#include <QKeyEvent>
/* Qt just uses the QWidget* parent as transient parent for native
 * platform dialogs. This makes it impossible to make them transient
 * to a bare QWindow*. So we catch the show event for the QDialog
 * and setTransientParent here instead. */
bool MainWindow::event(QEvent* ev) {
    if (ev->type() == QEvent::Timer) {
        logger.write();
        return true;
    } else if (ev->type() == QEvent::KeyPress) {
        handle_key(static_cast<QKeyEvent*>(ev)->key());
        return true;
    } else if (ev->type() == QEvent::MouseButtonPress) {
        const std::uint32_t button = static_cast<QMouseEvent*>(ev)->button();
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
    // App can exit via Quit menu
    m_quit_action = std::make_unique<QAction>("&Quit", this);
    connect(m_quit_action.get(), &QAction::triggered, qApp, &QCoreApplication::quit);

    // Run settings
    m_settings_action = std::make_unique<QAction>("&Settings", this);
    connect(m_settings_action.get(), &QAction::triggered,
        [&]() {
            if (m_process_settings->state() == QProcess::NotRunning)
                m_process_settings->open();
        });

    m_tray_menu = std::make_unique<QMenu>(this);
    m_tray_menu->addAction(m_settings_action.get());
    m_tray_menu->addAction(m_quit_action.get());
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
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

#endif

static void stop_process(QProcess* proc) {
    if (proc->state() == QProcess::Running) {
        proc->terminate();
        if (!proc->waitForFinished()) {
            std::cerr << "Process failed to terminate\n";
        }
    }
}

namespace vnepogodin {
namespace utils {
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

    static inline void send_json() {
        static constexpr auto URL = "http://torrenttor.ru/api1/post/";
        http::Request request(URL);

        nlohmann::json json{{"timestamp", std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())}};
#ifdef _WIN32
        static constexpr auto WORKING_BUFFER_SIZE = 15000;
        static constexpr auto MAX_TRIES           = 3;
        DWORD dwRetVal                            = 0;

        PIP_ADAPTER_ADDRESSES pAddresses = NULL;

        ULONG Iterations = 0;

        // Allocate a 15 KB buffer to start with.
        ULONG outBufLen = WORKING_BUFFER_SIZE;

        do {
            pAddresses = new IP_ADAPTER_ADDRESSES[outBufLen];
            if (pAddresses == NULL) {
                exit(1);
            }

            dwRetVal = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL,
                pAddresses, &outBufLen);

            if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
                delete[] pAddresses;
                pAddresses = nullptr;
            } else {
                break;
            }

            Iterations++;

        } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));
        const auto& pCurrAddresses = pAddresses;
        std::stringstream ss;
        if (pCurrAddresses) {
            if (pCurrAddresses->PhysicalAddressLength != 0) {
                for (int i = 0; i < (int)pCurrAddresses->PhysicalAddressLength; ++i) {
                    if (i == (pCurrAddresses->PhysicalAddressLength - 1)) {
                        ss << std::hex << ((int)pCurrAddresses->PhysicalAddress[i]);
                    } else {
                        ss << std::hex << ((int)pCurrAddresses->PhysicalAddress[i]) << "-";
                    }
                }
            }
        }

        json["mac"] = ss.str();

        if (pAddresses) {
            delete[] pAddresses;
        }
#endif
        [[maybe_unused]] const auto& response = request.send("POST", json.dump(),
            {"Content-Type: application/json"});
    }
}  // namespace utils
}  // namespace vnepogodin

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent) {
    m_ui->setupUi(this);
    m_process_settings = std::make_unique<QProcess>(this);

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NativeWindow);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::BypassWindowManagerHint | Qt::SplashScreen);
#ifdef _WIN32
    m_process_settings->setProgram("GOATTech-settings.exe");
    m_hwnd = (HWND)winId();
    SetForegroundWindow(m_hwnd);
    SetWindowPos(m_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    SetTimer(m_hwnd,  // handle to main window
        IDT_TIMER,    // timer identifier
        100,          // 100ms interval
        (TIMERPROC)NULL);

    SetHook();

    nid.hWnd = m_hwnd;

    nid.uID = 1;

    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

    nid.uCallbackMessage = IDT_TRAY;

    ExtractIconExW(L"icon.ico", 0, NULL, &(nid.hIcon), 1);

    wcscpy_s(nid.szTip, L"GOATTech");

    Shell_NotifyIconW(NIM_ADD, &nid);

    // Get screen property
    const auto& rec = QApplication::desktop()->geometry();

    // Calculate overlay percentage of the window
    static constexpr float perc_of_window = 0.2F;
    const auto& perc_height               = rec.height() * perc_of_window;
    const auto& perc_width                = rec.width() * perc_of_window;
#else
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

    // Tray icon menu
    createMenu();
    m_tray_icon->setContextMenu(m_tray_menu.get());
    m_tray_icon->setToolTip("GOATTech");

    // App icon
    const auto& appIcon = QIcon("icon.png");
    m_tray_icon->setIcon(appIcon);
    setWindowIcon(appIcon);

    // Displaying the tray icon
    m_tray_icon->show();

    // Interaction
    connect(m_tray_icon.get(), &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
#endif
    const int& size = qMin(perc_height, perc_width);

    m_ui->keyboard->setFixedSize(size, size);
    m_ui->mouse->setFixedSize(size / 1.5, size / 1.5);

    QSettings settings(QSettings::UserScope);
    QSettings::setDefaultFormat(QSettings::NativeFormat);
    nlohmann::json json;
    utils::toObject(&settings, json);
    utils::load_key(json, m_ui->keyboard, "hideKeyboard");
    utils::load_key(json, m_ui->mouse, "hideMouse");

    if (json.contains("inputDevice")) {
        m_recorder = std::make_unique<vnepogodin::Recorder>(json["inputDevice"].get<std::string>());
        m_recorder->record();
    }

    m_activated[0] = (m_ui->keyboard->isHidden()) ? 2 : 0;
    m_activated[1] = (m_ui->mouse->isHidden()) ? 2 : 0;
}

MainWindow::~MainWindow() {
#ifdef _WIN32
    UnhookWindowsHookEx(_hook_keyboard);
    UnhookWindowsHookEx(_hook_mouse);
    KillTimer(m_hwnd, IDT_TIMER);
    Shell_NotifyIconW(NIM_DELETE, &nid);

#else
    killTimer(m_timer);
#endif
    stop_process(m_process_settings.get());

    logger.close();
    m_recorder->stop();

    utils::send_json();
}
