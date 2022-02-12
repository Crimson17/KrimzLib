#pragma once

#include <iostream>
#include <vector>
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>

#include "KrimzLib/dx/gpu.h"
#include "KrimzLib/dx/fbuffer.h"
#include "KrimzLib/dx/raster.h"
#include "KrimzLib/dx/shaders.h"
#include "KrimzLib/dx/cbuffer.h"
#include "KrimzLib/dx/mesh.h"
#include "KrimzLib/dx/texture.h"
#include "KrimzLib/dx/sampler.h"
#include "KrimzLib/vecmath/ivec2.h"
#include "KrimzLib/vecmath/vec4.h"
#include "KrimzLib/renderer/vertex.h"
#include "KrimzLib/graphics/image.h"

#pragma comment (lib, "d3d11.lib")


namespace kl {
	class gpu {
	private:
		// DirectX pointers
		IDXGISwapChain* chain = nullptr;
		ID3D11Device* dev = nullptr;
		ID3D11DeviceContext* devcon = nullptr;
		kl::fbuffer* frameBuff = nullptr;

	public:
		// Constructor
		gpu(HWND hwnd, int msaa = 2);

		// Destructor
		~gpu();

		// Getters
		ID3D11Device* getDev();
		ID3D11DeviceContext* getDevCon();

		// Sets the viewport
		void setViewport(const kl::ivec2& pos, const kl::ivec2& size);

		// Clears the buffer
		void clearColor(const kl::vec4& color);
		void clearDepth();
		void clear(const kl::vec4& color);

		// Swaps the buffers
		void swap(bool vSync);

		// Sets the depth testing state
		void setDepthTest(bool enabled);

		// Raster
		kl::raster* newRaster(bool wireframe, bool cull, bool cullBack);

		// Shaders
		kl::shaders* newShaders(const std::string& filePath, uint32_t vertBuffSize, uint32_t pixlBuffSize);

		// Constant buffer
		kl::cbuffer* newCBuffer(int byteSize);

		// Mesh
		kl::mesh* newMesh(const std::vector<kl::vertex>& vertexData);
		kl::mesh* newMesh(const std::string& filePath, bool flipZ);

		// Texture
		kl::texture* newTexture(const kl::image& img);

		// Sampler
		kl::sampler* newSampler(bool linear, bool mirror);
	};
}