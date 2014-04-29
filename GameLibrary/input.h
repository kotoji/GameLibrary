#ifndef _GFW_INPUT_H
#define _GFW_INPUT_H

#include <windows.h>
#include <WindowsX.h>

namespace gfw {

class Input {

public:
    Input();
    virtual ~Input();

    void initialize(HWND hwnd, bool capture);

    void clear();

    void keyUp(WPARAM);

    void keyDown(WPARAM);
};

} // namespace gfw

#endif