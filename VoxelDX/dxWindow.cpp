#include "dxWindow.h"

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
	while (msg.message != WM_QUIT) {
		
		if (GetMessage(&msg, NULL, 0U, 0U) > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		renderer.render();
	}
}

LRESULT CALLBACK DXWindow::WindowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(window);
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