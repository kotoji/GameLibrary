#include "game_controller.h"
#include "utilities.h"

namespace gfw {

GameController* GameController::game_controller_ = nullptr;

GameController::GameController() :
graphics_(nullptr),
paused_(false),
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

// initialze the game.
// throws GameError
void GameController::initialize(HWND hwnd, int width, int height, 
                                bool fullscreen, float fps) {
    hwnd_ = hwnd;

    graphics_ = new Graphics();
    graphics_->initialize(hwnd_, width, height, fullscreen);  // throws GameError

    input_->initialize(hwnd_, false); // throws GameError

    frame_time_ = 1.0f / fps;

    // set up timer
    if (QueryPerformanceFrequency(&timer_freq_) == false) {
        throw(GameError(ErrorCode::FatalError, "Failed in initializing timer"));
    }
    QueryPerformanceCounter(&time_start_);

    initialized_ = true;
}

void GameController::setGame(std::shared_ptr<Game> game) {
    game_ = game;
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

// call in the windows main loop
void GameController::run() {
    if (graphics_ == nullptr) {
        return;
    }

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