#include "dxRenderer.h"

#include <cassert>
#include <chrono>

DXRenderer::DXRenderer() {}

DXRenderer::DXRenderer(HINSTANCE instance, HWND window)
	: instance(instance), window(window), camera({8.0f, 8.0f, 40}, {45.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0}),
	totalFrameTime(0.0f), totalFrameUpdateTime(0.0f), totalFrameRenderTime(0.0f) {
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
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.OutputWindow = window;

	Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
	Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
	Microsoft::WRL::ComPtr<IDXGIFactory> factory;

	device.As(&dxgiDevice);
	hRes = dxgiDevice->GetAdapter(&adapter);

	assert(SUCCEEDED(hRes));

	adapter->GetParent(IID_PPV_ARGS(&factory));
	hRes = factory->CreateSwapChain(device.Get(), &swapChainDesc, &swapChain);

	assert(SUCCEEDED(hRes));

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

	hRes = swapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		(void**)&backBuffer
	);

	assert(SUCCEEDED(hRes));

	hRes = device->CreateRenderTargetView(
		backBuffer.Get(),
		nullptr,
		renderTargetView.GetAddressOf()
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

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.AntialiasedLineEnable = TRUE;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
	context->RSSetState(rasterizerState.Get());

	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.Height = (float) backBufferDesc.Height;
	viewport.Width = (float) backBufferDesc.Width;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(
		1,
		renderTargetView.GetAddressOf(),
		depthStencilView.Get()
	);

	// Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device.Get(), context.Get());


	// Initial draw call...
	const float teal[] = { 0.098f, 0.439f, 0.439f, 1.000f };

	assert(renderTargetView.Get());
	context->ClearRenderTargetView(renderTargetView.Get(), teal);
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);

	context->OMSetRenderTargets(
		1,
		renderTargetView.GetAddressOf(),
		depthStencilView.Get()
	);

	int vsyncOn = 0; // 0 for off, 1 for on
	swapChain->Present(vsyncOn, 0);

	chunk = Chunk(device.Get(), context.Get(), { 0.0f, 0.0f, 0.0f });
	hasBeenCreated = true;
}

void DXRenderer::update()
{
	auto updateStartTime = std::chrono::high_resolution_clock::now();
	// Get the Window Size
	RECT windowRect;
	GetWindowRect(window, &windowRect);
	int windowWidth = abs(windowRect.right - windowRect.left);
	int windowHeight = abs(windowRect.bottom - windowRect.top);

	// Get the View Project Matrix
	XMFLOAT4X4 projection = camera.getProjectionMatrix(45.0f, windowWidth, windowHeight);
	XMFLOAT4X4 view = camera.getViewMatrix();
	chunk.update(context.Get(), view, projection);

	// ImGui logic
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Performance");
	ImGui::Text("Frame Time: %.5fms", averageFrameTime * 1000);
	ImGui::Text("Render loop time: %.5fms", averageFrameRenderTime * 1000);
	ImGui::Text("Update loop time: %.5fms", averageFrameUpdateTime * 1000);
	ImGui::Text("FPS: %d", (int)(1.0f / averageFrameTime));
	ImGui::End();

	auto updateEndTime = std::chrono::high_resolution_clock::now();
	totalFrameUpdateTime += std::chrono::duration<double>(updateEndTime - updateStartTime).count();
}

void DXRenderer::render() {
	auto renderStartTime = std::chrono::high_resolution_clock::now();
	const float teal[] = { 0.098f, 0.439f, 0.439f, 1.000f };

	static auto gamePerformanceStartTime = std::chrono::high_resolution_clock::now();

	assert(renderTargetView.Get());
	context->ClearRenderTargetView(renderTargetView.Get(), teal);
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);

	context->OMSetRenderTargets(
		1,
		renderTargetView.GetAddressOf(),
		depthStencilView.Get()
	);

	chunk.draw(context.Get());

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	int vsyncOn = 0; // 0 for off, 1 for on
	swapChain->Present(vsyncOn, DXGI_PRESENT_ALLOW_TEARING);

	auto renderEndTime = std::chrono::high_resolution_clock::now();
	double gamePerformanceTime = std::chrono::duration<double>(renderEndTime - gamePerformanceStartTime).count();

	totalFrameRenderTime += std::chrono::duration<double>(renderEndTime - renderStartTime).count();

	numFrames++;

	if (gamePerformanceTime > 0.2f) {
		totalFrameTime = totalFrameRenderTime + totalFrameUpdateTime;

		averageFrameTime = totalFrameTime / numFrames;
		averageFrameRenderTime = totalFrameRenderTime / numFrames;
		averageFrameUpdateTime = totalFrameUpdateTime / numFrames;

		numFrames = 0;
		totalFrameTime = 0.0f;
		totalFrameRenderTime = 0.0f;
		totalFrameUpdateTime = 0.0f;

		gamePerformanceStartTime = std::chrono::high_resolution_clock::now();
	}
}

void DXRenderer::cleanup()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void DXRenderer::handleResize(int width, int height) {
	if (width != 0 && height != 0 && hasBeenCreated) {
		context->OMSetRenderTargets(0, 0, 0);
		context->ClearState();
		renderTargetView->Release();
		depthStencilView->Release();
		depthStencil->Release();

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

		HRESULT result;
		result = swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);

		result = swapChain->GetBuffer(
			0,
			__uuidof(ID3D11Texture2D),
			(void**)&backBuffer
		);

		assert(SUCCEEDED(result));

		result = device->CreateRenderTargetView(
			backBuffer.Get(),
			nullptr,
			renderTargetView.GetAddressOf()
		);

		assert(SUCCEEDED(result));

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
			depthStencil.GetAddressOf()
		)));

		CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

		assert(SUCCEEDED(device->CreateDepthStencilView(
			depthStencil.Get(),
			&depthStencilViewDesc,
			depthStencilView.GetAddressOf()
		)));

		context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

		viewport.Width = width;
		viewport.Height = height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		context->RSSetViewports(1, &viewport);
	}
}