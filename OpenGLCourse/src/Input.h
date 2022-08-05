#pragma once

#include "Window.h"
#include "KeyCodes.h"
#include "MouseCodes.h"

class Input
{
public:
	Input() = delete;
	~Input() = delete;

	static void SetContext(Window* currentWindow);
	static bool IsKeyPressed(KeyCode key);
	static bool IsMouseButtonPressed(MouseCode button);
	static glm::vec2 GetMousePosition();
};

