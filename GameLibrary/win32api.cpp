#include "win32api.h"
#include "game_controller.h"

namespace gfw {
bool createWindow(HWND &hwnd, HINSTANCE h_inst, int width, int height, bool fullscreen) {
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
    ShowWindow(hwnd, SW_SHOW);

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