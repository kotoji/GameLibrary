#include "game_controller.h"
#include "utilities.h"
#include "win32api.h" // 

namespace gfw {

GameController* GameController::game_controller_ = nullptr;

GameController::GameController() :
graphics_(nullptr),
paused_(false),
width_(640),
height_(480),
fps_(60.f),
fullscreen_(false),
initialized_(false) {
    input_ = new Input();
}

GameController::~GameController() {
    releaseAll();
    safeDelete(graphics_);
    safeDelete(input_);
    initialized_ = false;

    ShowCursor(true);   //  for a case the game hide cursor
}

GameController* GameController::getInstance() {
    if (!game_controller_) {
        game_controller_ = new GameController();
    }
    return game_controller_;
}

// set variable for initarization.
void GameController::setWidth(int width) {
    width_ = width;
}
void GameController::setHeight(int height) {
    height_ = height;
}
void GameController::setFps(float fps) {
    fps_ = fps;
}
void GameController::setIsFullscreen(bool fullscreen) {
    fullscreen_ = fullscreen;
}

// initialze and run the game.
// throws GameError
void GameController::run(HINSTANCE h_inst, std::shared_ptr<Game> game) {
    // initialize win32 API
    if (!createWindow(hwnd_, h_inst, width_, height_, fullscreen_)) {
        throw(GameError(ErrorCode::FatalError, "Failed in initializing win32 API"));
    }

    // initialize GameController
    graphics_ = new Graphics();
    graphics_->initialize(hwnd_, width_, height_, fullscreen_);  // throws GameError

    input_->initialize(hwnd_, false); // throws GameError

    frame_time_ = 1.0f / fps_;

    // set up timer
    if (QueryPerformanceFrequency(&timer_freq_) == false) {
        throw(GameError(ErrorCode::FatalError, "Failed in initializing timer"));
    }
    QueryPerformanceCounter(&time_start_);

    initialized_ = true;

    // run game
    while (true) {
        if (!peekMessage()) {
            break;
        }
        else {
            main_loop();
        }
    }
}

// render items. (leave most processing to draw())
void GameController::render() {
    if (SUCCEEDED(graphics_->beginScene())) {
        draw();
    }
    graphics_->endScene();

    // check and handle lost device
    handleLostGraphicsDevice();

    graphics_->showBackBuffer();
}

// handle lost graphic device
void GameController::handleLostGraphicsDevice() {
    HRESULT hr;

    hr = graphics_->getDeviceState();
    if (FAILED(hr)) {
        // the device is lost and not available for reset
        if (hr == D3DERR_DEVICELOST) {
            Sleep(100); // yield cpu time
            return;
        }
        // the device is lost and available for reset
        else if (hr == D3DERR_DEVICENOTRESET) {
            releaseAll();
            hr = graphics_->reset();
            if (FAILED(hr)) {   // reset faild
                return;
            }
            resetAll();
        }
        else {
            return;
        }
    }
}

// main loop
void GameController::main_loop() {
    // adjust fps
    QueryPerformanceCounter(&time_end_);
    const float elapsed_time = static_cast<float>(time_end_.QuadPart - time_start_.QuadPart) /
                               static_cast<float>(timer_freq_.QuadPart);
    if (elapsed_time < frame_time_) {
        const DWORD sleep_time = static_cast<DWORD>((frame_time_ - elapsed_time) * 1000);
        timeBeginPeriod(1);
        Sleep(sleep_time);
        timeEndPeriod(1);
        return;
    }
    // calculate average fps
    if (elapsed_time > 0.0) {
        fps_average_ = fps_average_*0.99f + 0.01f / elapsed_time;
    }

    time_start_ = time_end_;

    // 
    if (!paused_) {
        update();
    }
    render();

    //
    input_->clear();
}

void GameController::update() {
    if (game_ && game_->isInitialized()) {
        game_->update();
    }
}

void GameController::draw() {
    if (game_ && game_->isInitialized()) {
        game_->draw();
    }
}

//
void GameController::releaseAll() {}

//
void GameController::resetAll() {}

//
LRESULT GameController::messageHandler(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
    if (initialized_) {
        switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_KEYDOWN: case WM_SYSKEYDOWN:
            input_->keyDown(w_param);
            return 0;
        case WM_KEYUP: case WM_SYSKEYUP:
            input_->keyUp(w_param);
            return 0;
        case WM_CHAR:
            // input_->keyInput(w_param);
            return 0;
        case WM_DEVICECHANGE:
            // input_->checkControllers();
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, w_param, l_param);
}

} // namespace gfw