#pragma once
#include <windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <iostream>
#include <dxgi.h>
#include <dxgi1_3.h>
#include <DirectXMath.h>
#include "quadPixelShader.h"
#include "quadVertexShader.h"

#include "dxShader.h"

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 colour;
};

class DXRenderer
{
protected:
	HINSTANCE instance;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	ID3D11RenderTargetView* renderTargetView;
	
	D3D11_VIEWPORT viewport;

	Vertex quadVertices[4];
	unsigned short quadIndices[6];
	int quadIndicesCount;
	ID3D11Buffer* quadVB;
	ID3D11Buffer* quadIB;
	DXShader quadShader;
public:
	DXRenderer();
	DXRenderer(HINSTANCE instance, HWND window);

	void update();
	void render();
};

