#include "dxWindow.h"
#define DEBUG

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE hPrevInstance, PWSTR cmdLine, int showWindow) {
	DXWindow window = DXWindow(instance, showWindow);
	window.run();
	return 0;
}

