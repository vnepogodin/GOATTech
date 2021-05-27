#include <vnepogodin/mainwindow.h>

#include <vnepogodin/logger.hpp>
#include <vnepogodin/utils.hpp>

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

// variable to store the HANDLE to the hook. Don't declare it anywhere else then globally
// or you will get problems since every function uses this variable.
static HHOOK _hook_keyboard;
static HHOOK _hook_mouse;
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

    // call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
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

    // call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
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

MainWindow::~MainWindow() {
    UnhookWindowsHookEx(_hook_keyboard);
    UnhookWindowsHookEx(_hook_mouse);
    KillTimer(m_hwnd, IDT_TIMER);
    Shell_NotifyIconW(NIM_DELETE, &nid);
    logger.close();
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
            
            break;

        case ID_EXIT:
            PostMessage(m_hwnd, WM_CLOSE, 0, 0);
            break;
        }
        break;
    case IDT_TRAY:
        switch (msg->lParam) {
        case WM_LBUTTONUP:
            //m_ui->mouse->setVisible(m_activated);
            m_ui->keyboard->setVisible(m_activated);
            m_activated = !m_activated;
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
#include <QMouseEvent>
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

#endif

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent) {
    m_ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NativeWindow);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::BypassWindowManagerHint | Qt::SplashScreen);
#ifdef _WIN32
    m_hwnd = (HWND)winId();
    SetForegroundWindow(m_hwnd);
    SetWindowPos(m_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    SetTimer(m_hwnd,  // handle to main window
        IDT_TIMER,  // timer identifier
        100,        // 100ms interval
        (TIMERPROC)NULL);

    SetHook();

    nid.hWnd = m_hwnd;

    nid.uID = 1;

    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

    nid.uCallbackMessage = IDT_TRAY;

    ExtractIconExW(L"icon.ico", 0, NULL, &(nid.hIcon), 1);

    wcscpy_s(nid.szTip, L"Tool Tip");

    Shell_NotifyIconW(NIM_ADD, &nid);
#else
    startTimer(100);
    setMouseTracking(true);
#endif
    const int& size = qMin(this->size().height(), this->size().width()) - 300;
    m_ui->keyboard->setFixedSize(size, size);
    m_ui->mouse->setFixedSize(size - 150, size - 150);

    //m_ui->keyboard->hide();
    m_ui->mouse->hide();
}
