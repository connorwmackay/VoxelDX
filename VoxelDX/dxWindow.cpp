#include "dxWindow.h"

#include <future>

DXWindow::DXWindow(HINSTANCE instance, int showWindow)
	: instance(instance), wndClass({}) {
	const wchar_t CLASS_NAME[] = L"VoxelDX";

	// Setup the Window Class
	wndClass.lpfnWndProc = WindowProc;
	wndClass.hInstance = instance;
	wndClass.lpszClassName = CLASS_NAME;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
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

	//renderer.release();
}

LRESULT CALLBACK DXWindow::WindowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_QUIT:
		DestroyWindow(window);
	case WM_CLOSE:
		DestroyWindow(window);
	case WM_DESTROY:
		PostQuitMessage(0);
	}

	return DefWindowProc(window, msg, wParam, lParam);
}