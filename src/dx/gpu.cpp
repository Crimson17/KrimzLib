#include "KrimzLib/dx/gpu.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include "KrimzLib/renderer/vertex.h"
#include "KrimzLib/convert.h"


// Constructor
kl::gpu::gpu(HWND hwnd, int msaa) {
    // Getting the window size
    RECT clientArea = {};
    GetClientRect(hwnd, &clientArea);

    // Swapchain info
    DXGI_SWAP_CHAIN_DESC chaindes = {};
    chaindes.BufferCount = 1;                                 // One back buffer
    chaindes.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // 32-bit color
    chaindes.BufferDesc.Width = clientArea.right;             // Setting the backbuffer width
    chaindes.BufferDesc.Height = clientArea.bottom;           // Setting the backbuffer height
    chaindes.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;   // Usage
    chaindes.OutputWindow = hwnd;                             // Window
    chaindes.SampleDesc.Count = msaa;                         // MSAA
    chaindes.Windowed = true;                                 // Windowed/fullscreen
    chaindes.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;  // Allowing fullscreen switching

    // Dev, devcon and chain creation
    D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        NULL,
        nullptr,
        NULL,
        D3D11_SDK_VERSION,
        &chaindes,
        &chain,
        &dev,
        nullptr,
        &devcon
    );
    if (!dev) {
        std::cout << "DirectX: Could not create device!";
        std::cin.get();
        exit(69);
    }
    if (!devcon) {
        std::cout << "DirectX: Could not create device context!";
        std::cin.get();
        exit(69);
    }
    if (!chain) {
        std::cout << "DirectX: Could not create swapchain!";
        std::cin.get();
        exit(69);
    }

    // Frame buffer creation
    frameBuff = new kl::fbuffer(chain, dev, devcon, clientArea.right, clientArea.bottom, msaa);
    frameBuff->bind(true);

    // Viewport setup
    this->setViewport(kl::ivec2(clientArea.left, clientArea.top), kl::ivec2(clientArea.right, clientArea.bottom));

    // Setting the triangle as the main primitive type
    devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// Destructor
kl::gpu::~gpu() {
    // Exiting fullscreen
    chain->SetFullscreenState(false, nullptr);

    // Memory release
    delete frameBuff;
    chain->Release();
    devcon->Release();
    dev->Release();
}

// Getters
ID3D11Device* kl::gpu::getDev() {
    return dev;
}
ID3D11DeviceContext* kl::gpu::getDevCon() {
    return devcon;
}

// Sets the viewport
void kl::gpu::setViewport(const kl::ivec2& pos, const kl::ivec2& size) {
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = float(pos.x);
    viewport.TopLeftY = float(pos.y);
    viewport.Width = float(size.x);
    viewport.Height = float(size.y);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    devcon->RSSetViewports(1, &viewport);
}

// Clears the buffer
void kl::gpu::clearColor(const kl::vec4& color) {
    frameBuff->clearColor(color);
}
void kl::gpu::clearDepth() {
    frameBuff->clearDepth();
}
void kl::gpu::clear(const kl::vec4& color) {
    frameBuff->clearColor(color);
    frameBuff->clearDepth();
}

// Swaps the buffers
void kl::gpu::swap(bool vSync) {
    chain->Present(vSync, NULL);
}

// Sets the depth testing state
void kl::gpu::setDepthTest(bool enabled) {
    frameBuff->bind(enabled);
}

// Creates a new rasterizer state
kl::raster* kl::gpu::newRaster(bool wireframe, bool cull, bool cullBack) {
    return new kl::raster(dev, devcon, wireframe, cull, cullBack);
}

// Compiles and returns shaders
kl::shaders* kl::gpu::newShaders(const std::string& filePath, uint32_t vertBuffSize, uint32_t pixlBuffSize) {
    return new kl::shaders(dev, devcon, filePath, vertBuffSize, pixlBuffSize);
}

// Creates a new constant buffer
kl::cbuffer* kl::gpu::newCBuffer(int byteSize) {
    return new kl::cbuffer(dev, devcon, byteSize);
}

// Creates a new mesh
kl::mesh* kl::gpu::newMesh(const std::vector<kl::vertex>& vertexData) {
    return new kl::mesh(dev, devcon, vertexData);
}
kl::mesh* kl::gpu::newMesh(const std::string& filePath, bool flipZ) {
    return new kl::mesh(dev, devcon, filePath, flipZ);
}

// Creates a new texture
kl::texture* kl::gpu::newTexture(const kl::image& img) {
    return new kl::texture(dev, devcon, img);
}

// Creates a new sampler
kl::sampler* kl::gpu::newSampler(bool linear, bool mirror) {
    return new kl::sampler(dev, devcon, linear, mirror);
}