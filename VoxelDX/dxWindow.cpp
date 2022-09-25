#include "dxWindow.h"

#include <future>

DXWindow::DXWindow(HINSTANCE instance, int showWindow)
	: instance(instance), wndClass({}) {
	const wchar_t CLASS_NAME[] = L"VoxelDX";

	// Setup the Window Class
	wndClass.lpfnWndProc = WindowProc;
	wndClass.hInstance = instance;
	wndClass.lpszClassName = CLASS_NAME;
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
	bool isGameRunning = true;
	MSG msg = {};

	while (isGameRunning) {
		if (PeekMessage(&msg, window, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				isGameRunning = false;
			}
		}

		renderer.update();
		renderer.render();
	}

	DestroyWindow(window);
}

LRESULT CALLBACK DXWindow::WindowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(window);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT paintStruct;
		HDC hdc = BeginPaint(window, &paintStruct);
		FillRect(hdc, &paintStruct.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(window, &paintStruct);
	}
	return 0;
	}

	return DefWindowProc(window, msg, wParam, lParam);
}