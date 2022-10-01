#include "dxWindow.h"

#include <future>

void callAssert()
{
	assert(false == true);
}

DXWindow::DXWindow(HINSTANCE instance, int showWindow)
	: instance(instance), wndClass({}) {
	const wchar_t CLASS_NAME[] = L"VoxelDX";

	// Setup the Window Class
	wndClass.lpfnWndProc = WindowProc;
	wndClass.hInstance = instance;
	wndClass.lpszClassName = CLASS_NAME;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.cbWndExtra = sizeof(DXWindow);
	RegisterClass(&wndClass);

	window = CreateWindowEx(
		0,
		CLASS_NAME,
		L"VoxelDX",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
		NULL,
		NULL,
		instance,
		NULL
	);

	ShowWindow(window, showWindow);
	renderer = DXRenderer(instance, window);

	SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)this);

	inputManager.AddKeyCallback(0x57, &callAssert);
}

void DXWindow::run() {
	MSG msg = {};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			
			renderer.update();
			renderer.render();
		}
	}

	renderer.cleanup();
}

DXRenderer* DXWindow::GetRenderer() {
	return &renderer;
}

LRESULT CALLBACK DXWindow::RealWindowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam))
		return true;

	int windowWidth;
	int windowHeight;

	switch (msg)
	{
	case WM_SIZE:
		RECT windowSize;
		GetWindowRect(window, &windowSize);
		windowWidth = abs(windowSize.right - windowSize.left);
		windowHeight = abs(windowSize.bottom - windowSize.top);
		renderer.handleResize(windowWidth, windowHeight);
		break;
	case WM_QUIT:
	case WM_CLOSE:
		DestroyWindow(window);
	case WM_KEYUP:
		inputManager.HandleKeyUpEvent(wParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
	default:
		return DefWindowProc(window, msg, wParam, lParam);
	}

	return DefWindowProc(window, msg, wParam, lParam);
}

LRESULT CALLBACK DXWindow::WindowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
	DXWindow* dxWindow = (DXWindow*) GetWindowLongPtr(window, GWLP_USERDATA);

	if (dxWindow) {
		return dxWindow->RealWindowProc(window, msg, wParam, lParam);
	}

	return DefWindowProc(window, msg, wParam, lParam);
}