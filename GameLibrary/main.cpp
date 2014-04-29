#include <Windows.h>
#include <memory>
#include "test.h"
#include "game_controller.h"

namespace gfw {
bool initLib(HINSTANCE h_inst, int cmd_show, int width = 640, int height = 480,
    bool fullscreen = false, float fps = 60.0f);
bool createWindow(HWND &hwnd, HINSTANCE h_inst, int cmd_show,
    int width, int height, bool fullscreen);
bool peekMessage();
LRESULT WINAPI winProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);
}

int WINAPI WinMain(HINSTANCE h_inst, HINSTANCE h_prev_inst,
    LPSTR cmd_line, int cmd_show) {

    if (!gfw::initLib(h_inst, cmd_show)) {
        return 1;
    }

    auto game = std::make_shared<gfw::Test>();
    game->initialize();
    gfw::GameController::getInstance()->setGame(game);

    try {
        while (true) {
            if (!gfw::peekMessage()) {
                break;
            } else {
                gfw::GameController::getInstance()->run();
            }
        }
    }
    catch (const gfw::GameError& e) {
        MessageBox(NULL, e.what(), "Error", MB_OK);
    }
    catch (...) {
        MessageBox(NULL, "Unknown error occured in game.", "Error", MB_OK);
    }

    return 0;
}

namespace gfw {
bool initLib(HINSTANCE h_inst, int cmd_show, int width, int height,
             bool fullscreen, float fps) {
    HWND hwnd;
    if (!createWindow(hwnd, h_inst, cmd_show, width, height, fullscreen)) {
        return false;
    }
    auto* gc = GameController::getInstance();
    try {
        gc->initialize(hwnd, width, height, fullscreen, fps);
    }
    catch (const GameError& e) {
        MessageBox(NULL, e.what(), "Error", MB_OK);
        return false;
    }
    catch (...) {
        MessageBox(NULL, "Unknown error occured in game.", "Error", MB_OK);
        return false;
    }

    return true;
}

bool createWindow(HWND &hwnd, HINSTANCE h_inst, int cmd_show,
                  int width, int height, bool fullscreen) {
    WNDCLASSEX wcx;

    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW;     // redraw if size changes 
    wcx.lpfnWndProc = gfw::winProc;          // points to window procedure 
    wcx.cbClsExtra = 0;
    wcx.cbWndExtra = 0;
    wcx.hInstance = h_inst;
    wcx.hIcon = NULL;
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);   // predefined arrow 
    wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);    // black background 
    wcx.lpszMenuName = NULL;            // name of menu resource 
    wcx.lpszClassName = "test";         // name of window class 
    wcx.hIconSm = NULL;                 // small class icon 

    if (RegisterClassEx(&wcx) == 0)
        return false;

    //set up the screen
    DWORD style;
    if (fullscreen)
        style = WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP;
    else
        style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    // Create window
    hwnd = CreateWindow(
        "test",                // name of the window class
        "test_title",          // title bar text
        style,                 // window style
        CW_USEDEFAULT,         // default horizontal position of window
        CW_USEDEFAULT,         // default vertical position of window
        width,                 // width of window
        height,                // height of the window
        (HWND)NULL,            // no parent window
        (HMENU)NULL,           // no menu
        h_inst,                // handle to application instance
        (LPVOID)NULL);         // no window parameters

    // if there was an error creating the window
    if (!hwnd)
        return false;

    if (!fullscreen)
    {
        // Adjust window size so client area is width * height
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);   // get size of client area of window
        MoveWindow(hwnd,
            0,
            0,
            width + (width - clientRect.right),
            height + (height - clientRect.bottom),
            TRUE);
    }

    // Show the window
    ShowWindow(hwnd, cmd_show);

    return true;
}

bool peekMessage() {
    MSG msg;
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}

LRESULT WINAPI winProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
    return (GameController::getInstance()->messageHandler(hwnd, msg, w_param, l_param));
}

}