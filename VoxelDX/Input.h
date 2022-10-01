#pragma once
#include <unordered_map>
#include <windows.h>

#define INPUT_CALLBACK void(*)(void)

class Input
{
protected:
	std::unordered_map<WPARAM, INPUT_CALLBACK> keyMap;
public:
	Input();

	void AddKeyCallback(WPARAM key, void(*func)(void));
	void HandleKeyUpEvent(WPARAM key);
};

static Input inputManager = Input();