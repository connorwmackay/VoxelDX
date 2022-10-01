#pragma once
#include <string>
#include <windows.h>
#include <ctime>

#include "dxRenderer.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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

