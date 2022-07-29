#include "Window.h"

#include <iostream>
#include <GLFW/glfw3.h>

#include "OpenGLContext.h"

static void GlfwErrorCallback(int32_t error, const char* description)
{
	std::cerr << "GLFW Error (" << error << "): " << description << '\n';
}

Window::Window(WindowProps props)
	: m_WindowProps(std::move(props))
{
}

Window::~Window()
{
	Shutdown();
}

bool Window::Init()
{
	std::cout << "Creating window '" << m_WindowProps.Title << "' (" << m_WindowProps.Width << ", " << m_WindowProps.Height << ")\n";

	if (s_GLFWWindowCount == 0)
	{
		if (!glfwInit())
		{
			std::cerr << "Failed to initialize GLFW!\n";
			return false;
		}

		glfwSetErrorCallback(GlfwErrorCallback);
	}

#if defined(APP_DEBUG)
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

	CreateGlfwWindow();

	if (!OpenGLContext::Init(m_Window))
	{
		Shutdown();
		return false;
	}

	glfwSwapInterval(1);

	return true;
}

bool Window::ShouldClose() const
{
	return glfwWindowShouldClose(m_Window);
}

void Window::OnUpdate() const
{
	glfwPollEvents();
	glfwSwapBuffers(m_Window);
}

glm::vec2 Window::GetBufferSize() const
{
	int32_t width, height;
	glfwGetFramebufferSize(m_Window, &width, &height);
	return { width, height };
}

void Window::Shutdown() const
{
	glfwDestroyWindow(m_Window);
	if (--s_GLFWWindowCount == 0)
		glfwTerminate();
}

void Window::CreateGlfwWindow()
{
	int32_t count;
	int32_t monitorX, monitorY;

	// Get the monitor viewport and the video mode spec
	GLFWmonitor** monitors = glfwGetMonitors(&count);
	glfwGetMonitorPos(monitors[0], &monitorX, &monitorY);
	const GLFWvidmode* videoMode = glfwGetVideoMode(monitors[0]);

	// Set the visibility hint to to false for the window creation
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	// Create the glfw window
	m_Window = glfwCreateWindow((int32_t)m_WindowProps.Width, (int32_t)m_WindowProps.Height, m_WindowProps.Title.c_str(), nullptr, nullptr);

	// Set its position to be centered based on the width and height of the monitor
	glfwSetWindowPos(m_Window,
		monitorX + (int32_t)((videoMode->width - m_WindowProps.Width) / 2),
		monitorY + (int32_t)((videoMode->height - m_WindowProps.Height) / 2));

	// Finally show the window and increase the window count
	glfwShowWindow(m_Window);
	s_GLFWWindowCount++;
}
