#ifndef _GFW_GRAPHICS_H
#define _GFW_GRAPHICS_H

#include <d3d9.h>
#include <d3dx9.h>
#include "game_error.h"

#define LP_3DDEVICE LPDIRECT3DDEVICE9
#define LP_3D       LPDIRECT3D9
#define COLOR_ARGB  D3DCOLOR

namespace gfw {

class Graphics
{
    // DirectX
    LP_3D       direct3d_;
    LP_3DDEVICE device3d_;
    D3DPRESENT_PARAMETERS d3dpp_;
    D3DDISPLAYMODE display_mode_;

    // Others
    HWND hwnd_;
    bool fullscreen_;
    int width_;
    int height_;
    COLOR_ARGB bg_color_;

    void initD3Dpp();

public:
    Graphics();
    Graphics(const Graphics&) = delete;
    Graphics(const Graphics&&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    virtual ~Graphics();

    void releaseAll();

    void initialize(HWND hwnd, int width, int height, bool fullscreen);

    HRESULT showBackBuffer();

    bool isAdapterCompatible(); // display_mode_を書き換える(副作用がある)。名前を変えたい。

    HRESULT reset();

    HRESULT getDeviceState() const;

    void setBackColor(COLOR_ARGB color) { bg_color_ = color; }

    HRESULT beginScene();

    HRESULT endScene();
};

} // namespace gfw

#endif