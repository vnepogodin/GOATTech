#include <vnepogodin/mainwindow.h>

#include <vnepogodin/logger.hpp>
#include <vnepogodin/utils.hpp>

using namespace vnepogodin;

// variable to store the HANDLE to the hook. Don't declare it anywhere else then globally
// or you will get problems since every function uses this variable.
static HHOOK _hook_keyboard;
static HHOOK _hook_mouse;
static vnepogodin::Logger logger;

static inline std::uint32_t handle_key(std::uint32_t key_stroke) {
    for (const auto& code : utils::code_list) {
        if (code.first == key_stroke) {
            logger.add_key(code.second);
            return code.first;
        }
    }

    return utils::key_code::UNDEFINED;
}

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

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  ui->setupUi(this);

  setAttribute(Qt::WA_TranslucentBackground);
  setWindowFlags(Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::BypassWindowManagerHint);  // | Qt::SplashScreen);
#ifdef _WIN32
  this->hwnd = (HWND)winId();
  SetForegroundWindow(hwnd);
  SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
      SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

  SetTimer(hwnd,      // handle to main window
           IDT_TIMER, // timer identifier
           100,       // 100ms interval
           (TIMERPROC)NULL);

  SetHook();
#endif
  const int& size = qMin(this->size().height(), this->size().width()) - 300;
  ui.get()->keyboard->setFixedSize(size, size);
  ui.get()->mouse->setFixedSize(size - 150, size - 150);

  //ui.get()->keyboard->hide();
  ui.get()->mouse->hide();
}

MainWindow::~MainWindow() {
  UnhookWindowsHookEx(_hook_keyboard);
  UnhookWindowsHookEx(_hook_mouse);
  KillTimer(hwnd, IDT_TIMER);
  logger.close();
  DestroyWindow(hwnd);
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result) {
    Q_UNUSED(eventType)
    Q_UNUSED(result)
    // Transform the message pointer to the MSG WinAPI
    MSG* msg = static_cast<MSG*>(message);

    switch (msg->message) {
    case WM_TIMER:
        logger.write();
      break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg->message, msg->wParam, msg->lParam);
    }

    return false;
}
