#pragma once

#include <glm/vec4.hpp>

struct GLFWwindow;

class OpenGLContext
{
public:
	static bool Init(GLFWwindow* window);
	static void SetClearColor(const glm::vec4& color);
	static void Clear();
};
