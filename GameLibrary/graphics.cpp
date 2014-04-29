#include "graphics.h"
#include "utilities.h"

namespace gfw {

Graphics::Graphics() :
direct3d_(nullptr),
device3d_(nullptr),
fullscreen_(false),
width_(400),
height_(600) {
    bg_color_ = D3DCOLOR_ARGB(255, 64, 64, 128);
}

Graphics::~Graphics() {
    releaseAll();
}

void Graphics::releaseAll() {
    safeRelease(device3d_);
    safeRelease(direct3d_);
}

void Graphics::initialize(HWND hwnd, int width, int height, bool fullscreen) {
    hwnd_ = hwnd;
    width_ = width;
    height_ = height;
    fullscreen_ = fullscreen;

    // initialize Direct3D
    direct3d_ = Direct3DCreate9(D3D_SDK_VERSION);
    if (direct3d_ == nullptr) {
        throw(GameError(ErrorCode::FatalError, "Error initializing Direct3D."));
    }
    initD3Dpp();
    if (fullscreen_) {
        if (isAdapterCompatible()) {
            d3dpp_.FullScreen_RefreshRateInHz = display_mode_.RefreshRate;
        }
        else {
            throw(GameError(ErrorCode::FatalError,
                "The graphics device does not support the specified resolution and/or format."));
        }
    }

    // 
    HRESULT hr;
    D3DCAPS9 caps;
    DWORD behavior;
    hr = direct3d_->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
    if ((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
        caps.VertexShaderVersion < D3DVS_VERSION(1, 1)) {
        behavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
    else {
        behavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }
    // create Direct3D device
    hr = direct3d_->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hwnd_,
        behavior,
        &d3dpp_,
        &device3d_);

    if (FAILED(hr)) {
        throw(GameError(ErrorCode::FatalError, "Error creating Direct3D device."));
    }
}

void Graphics::initD3Dpp() {
    try {
        ZeroMemory(&d3dpp_, sizeof(d3dpp_));
        d3dpp_.BackBufferWidth = width_;
        d3dpp_.BackBufferHeight = height_;
        if (fullscreen_) {
            d3dpp_.BackBufferFormat = D3DFMT_X8R8G8B8;
        }
        else {
            d3dpp_.BackBufferFormat = D3DFMT_UNKNOWN;
        }
        d3dpp_.BackBufferCount = 1;
        d3dpp_.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp_.hDeviceWindow = hwnd_;
        d3dpp_.Windowed = !fullscreen_;
        d3dpp_.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    }
    catch (...) {
        throw(GameError(ErrorCode::FatalError,
                "Error initializeing D3D Presentation Parameters"));
    }
}

HRESULT Graphics::showBackBuffer() {
    HRESULT hr = E_FAIL;
    hr = device3d_->Present(NULL, NULL, NULL, NULL);
    return hr;
}

bool Graphics::isAdapterCompatible() {
    UINT modes = direct3d_->GetAdapterModeCount(D3DADAPTER_DEFAULT,
                                                d3dpp_.BackBufferFormat);
    for (unsigned int i = 0; i < modes; ++i) {
        direct3d_->EnumAdapterModes(D3DADAPTER_DEFAULT,
                                    d3dpp_.BackBufferFormat,
                                    i, &display_mode_);
        if (display_mode_.Height == d3dpp_.BackBufferHeight &&
            display_mode_.Width == d3dpp_.BackBufferWidth &&
            display_mode_.RefreshRate >= d3dpp_.FullScreen_RefreshRateInHz) {
            return true;
        }
    }
    return false;
}

HRESULT Graphics::getDeviceState() const {
    HRESULT hr = E_FAIL;
    if (device3d_ == nullptr) {
        return hr;
    }
    hr = device3d_->TestCooperativeLevel();
    return hr;
}

HRESULT Graphics::reset() {
    HRESULT hr = E_FAIL;
    initD3Dpp();
    hr = device3d_->Reset(&d3dpp_);
    return hr;
}

HRESULT Graphics::beginScene() {
    HRESULT hr = E_FAIL;
    if (device3d_) {
        device3d_->Clear(0, NULL, D3DCLEAR_TARGET, bg_color_, 1.0f, 0);
        hr = device3d_->BeginScene();
    }
    return hr;
}

HRESULT Graphics::endScene() {
    HRESULT hr = E_FAIL;
    if (device3d_) {
        hr = device3d_->EndScene();
    }
    return hr;
}

} // namespace gfw