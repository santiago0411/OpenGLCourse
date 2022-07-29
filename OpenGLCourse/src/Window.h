#pragma once

#include <string>
#include <glm/vec2.hpp>

struct WindowProps
{
	std::string Title;
	uint32_t Width, Height;

	WindowProps(const std::string& title = "Hazel Engine",
		uint32_t width = 1280,
		uint32_t height = 720)
		: Title(title), Width(width), Height(height) {}
};

struct GLFWwindow;

class Window
{
public:
	explicit Window(WindowProps props);
	~Window();

	bool Init();
	bool ShouldClose() const;
	void OnUpdate() const;

	uint32_t GetWindowWidth() const { return m_WindowProps.Width; }
	uint32_t GetWindowHeight() const { return m_WindowProps.Height; }

	glm::vec2 GetBufferSize() const;

private:
	void Shutdown() const;
	void CreateGlfwWindow();

private:
	GLFWwindow* m_Window = nullptr;

	WindowProps m_WindowProps;
	inline static uint8_t s_GLFWWindowCount = 0;
};

