#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Input.h"
#include "Light.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture2D.h"
#include "Window.h"

#include "OpenGLContext.h"

static void CalculateAverageNormals(float* vertices, uint32_t verticesCount, uint32_t stride, uint32_t* indices, uint32_t indicesCount, uint32_t normalsOffset)
{
	for (size_t i = 0; i < indicesCount; i += 3)
	{
		uint32_t in0 = indices[i] * stride;
		uint32_t in1 = indices[i + 1] * stride;
		uint32_t in2 = indices[i + 2] * stride;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		const glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

		in0 += normalsOffset; in1 += normalsOffset; in2 += normalsOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticesCount / stride; i++)
	{
		const uint32_t offset = i * stride * normalsOffset;
		// Nx - Ny - Nz
		const glm::vec3 vec = glm::normalize(glm::vec3(vertices[offset], vertices[offset + 1], vertices[offset + 2]));
		vertices[offset] = vec.x; vertices[offset + 1] = vec.y; vertices[offset + 2] = vec.z;
	}
}

static Mesh CreatePyramid()
{
	// X-Y-Z	U-V		Nx-Ny-Nz
	float vertices[] = {
		-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.0f, -1.0f,  1.0f,	0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, -0.6f,	1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 0.0f,  1.0f,  0.0f,	0.5f, 1.0f,		0.0f, 0.0f, 0.0f,
	};

	uint32_t indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2,
	};

	CalculateAverageNormals(vertices, std::size(vertices), 8, indices, std::size(indices), 5);

	Mesh mesh;
	mesh.CreateMesh(vertices, indices, std::size(vertices), std::size(indices));
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
	props.Width = 1920;
	props.Height = 1080;

	auto* window = new Window(props);

	if (!window->Init())
		return -1;

	window->SetEventCallback(&OnEvent);
	Input::SetContext(window);

	std::vector<Texture2D> textures;
	textures.emplace_back("textures/brick.png");
	textures.emplace_back("textures/dirt.png");

	std::vector<Material> materials;
	materials.emplace_back(1.0f, 32.0f);
	materials.emplace_back(0.3f, 4.0f);

	std::vector<Mesh> meshes;
	meshes.emplace_back(CreatePyramid());

	Shader shader;
	shader.CreateFromFile("./shaders/VertexShader.glsl", "./shaders/FragmentShader.glsl");
	shader.Bind();

	LightSpecification lightSpec;
	lightSpec.Color = glm::vec3(1.0f);
	lightSpec.AmbientIntensity = 0.5f;
	lightSpec.Direction = glm::vec3(2.0f, -1.0f, -2.0f);
	lightSpec.DiffuseIntensity = 0.3f;

 	Light light(lightSpec);
	light.UploadLight(shader);

	CameraSpecification cameraSpec;
	cameraSpec.Position = glm::vec3(0.0f);
	cameraSpec.WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraSpec.Yaw = -90.0f;
	cameraSpec.Pitch = 0.0f;
	cameraSpec.Speed = 5.0f;
	cameraSpec.TurnSpeed = 10.0f;

	Camera camera(cameraSpec);

	const float aspectRatio = (float)window->GetWindowWidth() / (float)window->GetWindowHeight();
	const glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

	static float lastFrameTime = 0.0f;
	static float angle = 0.0f;

	while (!window->ShouldClose())
	{
		OpenGLContext::Clear();

		float time = window->GetCurrentTime();
		float deltaTime = time - lastFrameTime;
		lastFrameTime = time;

		camera.OnUpdate(deltaTime);
		shader.UploadUniformFloat3("u_EyePosition", camera.GetPosition());

		angle += 0.5f;

		shader.UploadUniformMat4("u_View", camera.CalculateViewMatrix());
		shader.UploadUniformMat4("u_Projection", projection);

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));
			/*glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));*/

		shader.UploadUniformMat4("u_Model", model);
		textures[0].Bind();
		materials[0].UploadMaterial(shader);
		meshes[0].RenderMesh();

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, -2.5f));
			/*glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));*/

		shader.UploadUniformMat4("u_Model", model);
		textures[1].Bind();
		materials[1].UploadMaterial(shader);
		meshes[0].RenderMesh();

		window->OnUpdate();
	}

	delete window;

	return 0;
}