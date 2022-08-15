#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Input.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture2D.h"
#include "Window.h"

#include "OpenGLContext.h"

static Camera* s_Camera;
static float s_LastFrameTime = 0.0f;

static Mesh CreatePyramid()
{
	// X-Y-Z U-V
	float vertices[] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.0f, -0.5f, 0.5f, 0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f, 0.5f, 1.0f,
	};

	uint32_t indices[] = {
		0, 1, 3,
		1, 2, 3,
		0, 2, 3,
		0, 1, 2,
	};

	Mesh mesh;
	mesh.CreateMesh(vertices, indices, sizeof vertices, sizeof indices);
	return mesh;
}

static bool OnKeyPressed(KeyPressedEvent& e)
{
	std::cout << "Key pressed: " << e.GetKey() << " Repeated: " << e.IsRepeated() << '\n';
	return true;
}

static bool OnKeyReleased(KeyReleasedEvent& e)
{
	std::cout << "Key released: " << e.GetKey() << '\n';
	return true;
}

static bool OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
	std::cout << "Mouse button pressed: " << e.GetMouseButton() << '\n';
	return true;
}

static bool OnMouseButtonReleased(MouseButtonReleasedEvent& e)
{
	std::cout << "Mouse button released: " << e.GetMouseButton() << '\n';
	return true;
}

static bool OnMouseScrolled(MouseScrolledEvent& e)
{
	std::cout << "Mouse scrolled: {" << e.GetOffSetX() << ", " << e.GetOffSetY() << "}\n";
	return true;
}

static bool OnMouseMoved(MouseMovedEvent& e)
{
	std::cout << "Mouse moved: {" << e.GetX() << ", " << e.GetY() << "}\n";
	return true;
}

static void OnEvent(Event& event)
{
	/*EventDispatcher dispatcher(event);
	dispatcher.Dispatch<KeyPressedEvent>(OnKeyPressed);
	dispatcher.Dispatch<KeyReleasedEvent>(OnKeyReleased);
	dispatcher.Dispatch<MouseButtonPressedEvent>(OnMouseButtonPressed);
	dispatcher.Dispatch<MouseButtonReleasedEvent>(OnMouseButtonReleased);
	dispatcher.Dispatch<MouseScrolledEvent>(OnMouseScrolled);
	dispatcher.Dispatch<MouseMovedEvent>(OnMouseMoved);*/
}

int main()
{
	WindowProps props;
	props.Title = "OpenGLApp";
	props.Width = 1400;
	props.Height = 1050;

	Window* window = new Window(props);

	if (!window->Init())
		return -1;

	window->SetEventCallback(&OnEvent);
	Input::SetContext(window);

	std::vector<Texture2D> textures;
	textures.emplace_back(Texture2D("textures/brick.png"));
	textures.emplace_back(Texture2D("textures/dirt.png"));

	textures[1].Bind();

	std::vector<Mesh> meshes;
	meshes.emplace_back(CreatePyramid());

	Shader shader;
	shader.CreateFromFile("./shaders/VertexShader.glsl", "./shaders/FragmentShader.glsl");
	shader.Bind();

	s_Camera = new Camera(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 2.0f);

	float aspectRatio = (float)window->GetWindowWidth() / (float)window->GetWindowHeight();
	const glm::mat4 projection = glm::perspective(45.0f, aspectRatio, 0.1f, 100.0f);

	static float angle = 0.0f;

	while (!window->ShouldClose())
	{
		OpenGLContext::Clear();

		float time = window->GetCurrentTime();
		float deltaTime = time - s_LastFrameTime;
		s_LastFrameTime = time;

		s_Camera->OnUpdate(deltaTime);

		angle += 0.5f;

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

		shader.UploadUniformMat4("model", model);
		shader.UploadUniformMat4("view", s_Camera->CalculateViewMatrix());
		shader.UploadUniformMat4("projection", projection);

		meshes[0].RenderMesh();

		window->OnUpdate();
	}

	delete s_Camera;
	delete window;

	return 0;
}