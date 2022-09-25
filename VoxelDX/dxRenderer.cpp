#include "dxRenderer.h"

#include <cassert>

DXRenderer::DXRenderer() {}

DXRenderer::DXRenderer(HINSTANCE instance, HWND window)
	: instance(instance) {
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_11_1
	};

	UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(DEBUG) || defined(_DEBUG)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hRes = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		deviceFlags,
		levels,
		ARRAYSIZE(levels),
		D3D11_SDK_VERSION,
		&device,
		&featureLevel,
		&context
	);

	assert(SUCCEEDED(hRes));

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.OutputWindow = window;

	Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
	Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
	Microsoft::WRL::ComPtr<IDXGIFactory> factory;

	//device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	device.As(&dxgiDevice);
	hRes = dxgiDevice->GetAdapter(&adapter);

	assert(SUCCEEDED(hRes));

	adapter->GetParent(IID_PPV_ARGS(&factory));
	hRes = factory->CreateSwapChain(device.Get(), &swapChainDesc, &swapChain);

	assert(SUCCEEDED(hRes));

	hRes = swapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		(void**)&backBuffer
	);

	assert(SUCCEEDED(hRes));

	hRes = device->CreateRenderTargetView(
		backBuffer.Get(),
		nullptr,
		&renderTargetView
	);

	assert(SUCCEEDED(hRes));

	D3D11_TEXTURE2D_DESC backBufferDesc;
	backBuffer->GetDesc(&backBufferDesc);

	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		static_cast<UINT> (backBufferDesc.Width),
		static_cast<UINT> (backBufferDesc.Height),
		1,
		1,
		D3D11_BIND_DEPTH_STENCIL
	);

	assert(SUCCEEDED(device->CreateTexture2D(
		&depthStencilDesc,
		nullptr,
		&depthStencil
	)));

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

	assert(SUCCEEDED(device->CreateDepthStencilView(
		depthStencil.Get(), 
		&depthStencilViewDesc, 
		&depthStencilView
	)));

	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.Height = (float) backBufferDesc.Height;
	viewport.Width = (float) backBufferDesc.Width;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 0;

	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(
		1,
		&renderTargetView,
		depthStencilView.Get()
	);

	// Setup the quad buffers
	DirectX::XMFLOAT3 colour = DirectX::XMFLOAT3{ 0, 1, 0 };
	quadVertices[0] = { DirectX::XMFLOAT3{-0.5f, 0.5f, 0.0f}, colour };
	quadVertices[1] = { DirectX::XMFLOAT3{0.5f, 0.5f, 0.0f}, colour };
	quadVertices[2] = { DirectX::XMFLOAT3{-0.5f, -0.5f, 0.0f}, colour };
	quadVertices[3] = { DirectX::XMFLOAT3{0.5f, -0.5f, 0.0f}, colour };

	quadIndices[0] = 0;
	quadIndices[1] = 1;
	quadIndices[2] = 2;
	quadIndices[3] = 1;
	quadIndices[4] = 3;
	quadIndices[5] = 2;

	CD3D11_BUFFER_DESC quadVBDesc(sizeof(quadVertices), D3D11_BIND_VERTEX_BUFFER);

	D3D11_SUBRESOURCE_DATA quadVBData;
	ZeroMemory(&quadVBData, sizeof(D3D11_SUBRESOURCE_DATA));
	quadVBData.pSysMem = quadVertices;
	quadVBData.SysMemSlicePitch = 0;
	quadVBData.SysMemSlicePitch = 0;

	hRes = device->CreateBuffer(&quadVBDesc, &quadVBData, &quadVB);
	assert(SUCCEEDED(hRes));

	quadIndicesCount = ARRAYSIZE(quadIndices);

	CD3D11_BUFFER_DESC quadIBDesc(sizeof(quadIndices), D3D11_BIND_INDEX_BUFFER);

	D3D11_SUBRESOURCE_DATA quadIBData;
	ZeroMemory(&quadIBData, sizeof(D3D11_SUBRESOURCE_DATA));
	quadIBData.pSysMem = quadIndices;
	quadIBData.SysMemPitch = 0;
	quadIBData.SysMemSlicePitch = 0;

	hRes = device->CreateBuffer(&quadIBDesc, &quadIBData, &quadIB);
	assert(SUCCEEDED(hRes));

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
	{
			"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
			0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0
		},
	{
			"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0
		}
	};

	quadShader = DXShader(device.Get(), quadVertexShader, ARRAYSIZE(quadVertexShader), quadPixelShader,ARRAYSIZE(quadPixelShader), inputElementDesc, ARRAYSIZE(inputElementDesc));
}

void DXRenderer::update()
{
	
}

void DXRenderer::render() {

	const float teal[] = { 0.098f, 0.439f, 0.439f, 1.000f };

	context->ClearRenderTargetView(renderTargetView, teal);
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);

	context->OMSetRenderTargets(
		1,
		&renderTargetView,
		depthStencilView.Get()
	);

	// Perform setting of buffers, draw calls etc...
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &quadVB, &stride, &offset);
	context->IASetIndexBuffer(quadIB, DXGI_FORMAT_R16_UINT, 0);
	context->IASetInputLayout(quadShader.getInputLayout());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->VSSetShader(quadShader.getVertexShader(), nullptr, 0);
	context->PSSetShader(quadShader.getPixelShader(), nullptr, 0);
	context->DrawIndexed(quadIndicesCount, 0, 0);

	swapChain->Present(1, 0);
}