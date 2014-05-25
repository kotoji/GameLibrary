#ifndef _GFW_GAME_CONTROLLER_H
#define _GFW_GAME_CONTROLLER_H

#include <windows.h>
#include <Mmsystem.h>
#include "game.h"
#include "graphics.h"
#include "input.h"
#include "game_error.h"
#include <memory>

namespace gfw {

class GameController final{
    std::shared_ptr<Game> game_;
    Graphics*   graphics_;
    Input*      input_;
    HWND        hwnd_;
    float       frame_time_;
    float       fps_average_;
    bool        paused_;
    bool        initialized_;

    // use those variables when call run method(initialization).
    int width_;
    int height_;
    float fps_;
    bool fullscreen_;

    LARGE_INTEGER time_start_;
    LARGE_INTEGER time_end_;
    LARGE_INTEGER timer_freq_;

    static GameController* game_controller_;

    GameController();
    GameController(const GameController&) = delete;
    GameController(const GameController&&) = delete;
    GameController& operator=(const GameController&) = delete;
    virtual ~GameController();

public:
    static GameController* getInstance();

    // window message handler
    LRESULT messageHandler(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);

    // set initial parameters. those methods is useless after run method is called
    void setWidth(int width);
    void setHeight(int height);
    void setFps(float fps);
    void setIsFullscreen(bool fullscreen);

    // initialize and run the game.
    void run(HINSTANCE h_inst, std::shared_ptr<Game> game);

    // call in the windows main loop
    void main_loop();
    // call when the graphics device was lost.
    // release all video memory to reset graphics device
    void releaseAll();
    // recreate all surfaces and reset all entities
    void resetAll();
    // render items.
    void render();
    // handle lost graphics device
    void handleLostGraphicsDevice();
    // update items.
    void update();
    // draw items. call in render() method
    void draw();

    // getter Graphics*
    Graphics* getGraphics() {return graphics_;}
    // getter Input*
    Input* getInput()       {return input_;}

    // exit the game
    void exit()             {PostMessage(hwnd_, WM_DESTROY, 0, 0);}
};

} // namespace gfw

#endif