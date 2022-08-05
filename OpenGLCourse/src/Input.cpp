#include "Input.h"

#include <GLFW/glfw3.h>

static Window* s_CurrentWindow;

void Input::SetContext(Window* currentWindow)
{
	s_CurrentWindow = currentWindow;
}

bool Input::IsKeyPressed(KeyCode key)
{
	int32_t state = glfwGetKey(s_CurrentWindow->m_Window, key);
	return state == GLFW_PRESS;
}

bool Input::IsMouseButtonPressed(MouseCode button)
{
	int32_t state = glfwGetMouseButton(s_CurrentWindow->m_Window, button);
	return state == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition()
{
	double x, y;
	glfwGetCursorPos(s_CurrentWindow->m_Window, &x, &y);
	return { (float)x, (float)y };
}
