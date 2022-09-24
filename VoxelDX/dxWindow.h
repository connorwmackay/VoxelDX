#pragma once
#include <string>
#include <windows.h>

#include "dxRenderer.h"

class DXWindow
{
protected:
	HINSTANCE instance;
	WNDCLASS wndClass;
	HWND window;
	DXRenderer renderer;

	static LRESULT CALLBACK WindowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	DXWindow(HINSTANCE instance, int showWindow);
	void run();
};

