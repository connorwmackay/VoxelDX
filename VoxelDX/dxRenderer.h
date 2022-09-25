#pragma once
#include <windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <iostream>
#include <dxgi.h>
#include <dxgi1_3.h>
#include <DirectXMath.h>

#include "block.h"
#include "camera.h"
#include "quadPixelShader.h"
#include "quadVertexShader.h"

#include "dxShader.h"

class DXRenderer
{
protected:
	HINSTANCE instance;
	HWND window;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	ID3D11RenderTargetView* renderTargetView;
	
	D3D11_VIEWPORT viewport;

	Camera camera;
	Block block;
public:
	DXRenderer();
	DXRenderer(HINSTANCE instance, HWND window);

	void update();
	void render();
};

