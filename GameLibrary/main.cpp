#include <Windows.h>
#include <memory>
#include "test.h"
#include "game_controller.h"

namespace gfw {
bool createWindow(HWND &hwnd, HINSTANCE h_inst, int width, int height, bool fullscreen);
bool peekMessage();
LRESULT WINAPI winProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);
}

int WINAPI WinMain(HINSTANCE h_inst, HINSTANCE h_prev_inst,
    LPSTR cmd_line, int cmd_show) {

    auto game = std::make_shared<gfw::Test>();
    game->initialize();
    
    try {
        gfw::GameController::getInstance()->run(h_inst, game);
    }
    catch (const gfw::GameError& e) {
        MessageBox(NULL, e.what(), "Error", MB_OK);
    }
    catch (...) {
        MessageBox(NULL, "Unknown error occured in game.", "Error", MB_OK);
    }

    return 0;
}

