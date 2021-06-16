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

#include <charconv>
#include <chrono>
#include <iostream>
#include <sstream>

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
//static HANDLE pipe          = nullptr;
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

#include <QDesktopWidget>
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
    m_quitAction = std::make_unique<QAction>("&Quit", this);
    connect(m_quitAction.get(), &QAction::triggered, qApp, &QCoreApplication::quit);

    // Run settings
    m_settingsAction = std::make_unique<QAction>("&Settings", this);
    connect(m_settingsAction.get(), &QAction::triggered,
        [&]() {
            if (m_process_settings->state() == QProcess::NotRunning)
                m_process_settings->open();
        });

    m_trayMenu = std::make_unique<QMenu>(this);
    m_trayMenu->addAction(m_settingsAction.get());
    m_trayMenu->addAction(m_quitAction.get());
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
        proc->waitForFinished();
        if (!proc->waitForFinished()) {
            std::cerr << "Process failed to terminate\n";
        }
    }
}

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

    static inline void load_key(nlohmann::json& json, QWidget* obj, const std::string& key) {
        if (json.contains(key)) {
            if (json[key].is_string()) {
                const auto& value = json[key].get<std::string>();
                int result        = 0;
                std::from_chars(value.data(), value.data() + value.size(), result);
                obj->setVisible(!result);
            } else {
                obj->setVisible(!json[key].get<int>());
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
        [[maybe_unused]]
        const auto& response = request.send("POST", json.dump(),
            {"Content-Type: application/json"});
    }
}  // namespace utils
}  // namespace vnepogodin

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent) {
    m_ui->setupUi(this);
    m_process_settings = std::make_unique<QProcess>(this);
    //m_process_charts   = std::make_unique<QProcess>(this);

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NativeWindow);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::BypassWindowManagerHint | Qt::SplashScreen);
#ifdef _WIN32
    m_process_settings->setProgram("GOATTech-settings.exe");
    //m_process_charts->setProgram("GOATTech-charts.exe");
    m_hwnd = (HWND)winId();
    SetForegroundWindow(m_hwnd);
    SetWindowPos(m_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    SetTimer(m_hwnd,  // handle to main window
        IDT_TIMER,    // timer identifier
        100,          // 100ms interval
        (TIMERPROC)NULL);

    // Create a pipe to send data
    /*pipe = CreateNamedPipeW(
        L"\\\\.\\pipe\\GOATTech",  // name of the pipe
        PIPE_ACCESS_OUTBOUND,      // 1-way pipe -- send only
        PIPE_TYPE_BYTE,            // send data as a byte stream
        1,                         // only allow 1 instance of this pipe
        0,                         // no outbound buffer
        0,                         // no inbound buffer
        0,                         // use default wait time
        NULL                       // use default security attributes
    );*/

    SetHook();

    nid.hWnd = m_hwnd;

    nid.uID = 1;

    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

    nid.uCallbackMessage = IDT_TRAY;

    ExtractIconExW(L"icon.ico", 0, NULL, &(nid.hIcon), 1);

    wcscpy_s(nid.szTip, L"GOATTech");

    Shell_NotifyIconW(NIM_ADD, &nid);
#else
    m_trayIcon = std::make_unique<QSystemTrayIcon>(this);
    m_process_settings->setProgram("GOATTech-settings");
    //m_process_charts->setProgram("GOATTech-charts");
    m_timer = startTimer(100);
    setMouseTracking(true);

    const auto& rec         = QApplication::desktop()->geometry();
    const auto& window_size = this->size();
    move(-(rec.width() - window_size.width() - 900), rec.height() - window_size.height() + 100);

    // Tray icon menu
    createMenu();
    m_trayIcon->setContextMenu(m_trayMenu.get());
    m_trayIcon->setToolTip("GOATTech");

    // App icon
    const auto& appIcon = QIcon("icon.png");
    m_trayIcon->setIcon(appIcon);
    setWindowIcon(appIcon);

    // Displaying the tray icon
    m_trayIcon->show();

    // Interaction
    connect(m_trayIcon.get(), &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
#endif
    const int& size = qMin(this->size().height(), this->size().width()) - 300;
    m_ui->keyboard->setFixedSize(size, size);
    m_ui->mouse->setFixedSize(size - 150, size - 150);
    m_ui->eye->setFixedSize(size - 100, size - 100);

    QSettings settings(QSettings::UserScope);
    nlohmann::json json;
    utils::toObject(&settings, json);
    utils::load_key(json, m_ui->keyboard, "hideKeyboard");
    utils::load_key(json, m_ui->mouse, "hideMouse");

    m_activated[0] = (m_ui->keyboard->isHidden()) ? 2 : 0;
    m_activated[1] = (m_ui->mouse->isHidden()) ? 2 : 0;

    //m_process_charts->open();

    //if (poll.joinable())
    //    poll.join();

    //poll = std::thread(&MainWindow::loadToMemory, this);
}

MainWindow::~MainWindow() {
#ifdef _WIN32
    UnhookWindowsHookEx(_hook_keyboard);
    UnhookWindowsHookEx(_hook_mouse);
    KillTimer(m_hwnd, IDT_TIMER);
    Shell_NotifyIconW(NIM_DELETE, &nid);

    // Close the pipe (automatically disconnects client too)
    //CloseHandle(pipe);
#else
    killTimer(m_timer);
#endif
    //poll.join();
    stop_process(m_process_settings.get());
    //stop_process(m_process_charts.get());

    logger.close();

    utils::send_json();
}

/*
void MainWindow::loadToMemory() {
    if (m_process_charts->state() == QProcess::Running) {
#ifdef _WIN32
        // This call blocks until a client process connects to the pipe
        const BOOL result = ConnectNamedPipe(pipe, NULL);
        if (!result) {
            CloseHandle(pipe);  // close the pipe
            DestroyWindow(m_hwnd);
        }

        // This call blocks until a client process reads all the data
        const wchar_t* data   = L"{ \"a_button\": 89, \"ctrl_button\": 72, \"d_button\": 5, \"e_button\": 2, \"q_button\": 2, \"s_button\": 4, \"shift_button\": 300, \"space_button\": 20, \"w_button\": 135 }";
        DWORD numBytesWritten = 0;
        WriteFile(
            pipe,                            // handle to our outbound pipe
            data,                            // data to send
            wcslen(data) * sizeof(wchar_t),  // length of data to send (bytes)
            &numBytesWritten,                // will store actual amount of data sent
            NULL                             // not using overlapped IO
        );

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / refresh_rate));
#endif
    }
}*/
