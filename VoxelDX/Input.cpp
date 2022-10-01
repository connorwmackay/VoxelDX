#include "Input.h"

Input::Input()
	: keyMap(std::unordered_map<WPARAM, INPUT_CALLBACK>())
{}

void Input::AddKeyCallback(WPARAM key, void(*func)(void))
{
	keyMap.insert({ key, func });
}

void Input::HandleKeyUpEvent(WPARAM key) {
	for (auto& mapItem : keyMap)
	{
		if (mapItem.first == key) {
			mapItem.second();
		}
	}
}