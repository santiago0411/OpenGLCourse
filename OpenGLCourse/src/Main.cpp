#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Lights.h"
#include "Input.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture2D.h"
#include "UniformBuffer.h"
#include "Window.h"

#include "OpenGLContext.h"

#define DIRECTIONAL_LIGHT_BINDING 0
#define POINT_LIGHT_ARRAY_BINDING 1
#define MATERIAL_BINDING 2

#define MAX_POINT_LIGHTS 3

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

	return { vertices, indices, std::size(vertices), std::size(indices) };
}

static Mesh CreatePlane()
{
	float vertices[] = {
		-10.0f, 0.0f, -10.0f,	 0.0f,  0.0f,	0.0f, -1.0f, 0.0f,
		 10.0f, 0.0f, -10.0f,	10.0f,  0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f,  10.0f,	 0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		 10.0f, 0.0f,  10.0f,	10.0f, 10.0f,	0.0f, -1.0f, 0.0f,
	};

	uint32_t indices[] = {
		0, 2, 1,
		1, 2, 3
	};

	return { vertices, indices, std::size(vertices), std::size(indices) };
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
	textures.emplace_back("textures/plain.png");

	std::vector<Material> materials;
	materials.emplace_back(2.5f, 32.0f);
	materials.emplace_back(0.3f, 4.0f);

	UniformBuffer materialUB(sizeof(Material), MATERIAL_BINDING);

	std::vector<Mesh> meshes;
	meshes.emplace_back(CreatePyramid());
	meshes.emplace_back(CreatePlane());

	Shader shader;
	shader.CreateFromFile("./shaders/VertexShader.glsl", "./shaders/FragmentShader.glsl");
	shader.Bind();

	auto* dirLight = new DirectionalLight;
	dirLight->Color = glm::vec3(1.0f);
	dirLight->Direction = glm::vec3(2.0f, -1.0f, -2.0f);
	dirLight->AmbientIntensity = 0.3f;
	dirLight->DiffuseIntensity = 0.5f;

	UniformBuffer dirLightUB(sizeof(DirectionalLight), DIRECTIONAL_LIGHT_BINDING);
	dirLightUB.SetData(dirLight);

	delete dirLight;

	int pointLightCount = 0;
	auto* pointLight = new PointLight[MAX_POINT_LIGHTS];

	pointLight[0].Color = glm::vec3(0.0f, 0.0f, 1.0f);
	pointLight[0].Position = glm::vec3(0.0f, 0.0f, 0.0f);
	pointLight[0].AmbientIntensity = 0.3f;
	pointLight[0].DiffuseIntensity = 1.0f;
	pointLight[0].Constant = 1.0f;
	pointLight[0].Linear = 0.7f;
	pointLight[0].Exponent = 1.8f;
	pointLightCount++;

	pointLight[1].Color = glm::vec3(0.0f, 1.0f, 0.0f);
	pointLight[1].Position = glm::vec3(-4.0f, 2.0f, 0.0f);
	pointLight[1].AmbientIntensity = 0.5f;
	pointLight[1].DiffuseIntensity = 0.2f;
	pointLight[1].Constant = 1.0f;
	pointLight[1].Linear = 0.9f;
	pointLight[1].Exponent = 0.032f;
	pointLightCount++;

	UniformBuffer pointLightUB(sizeof(PointLight) * pointLightCount, POINT_LIGHT_ARRAY_BINDING);
	pointLightUB.SetData(pointLight);
	delete[] pointLight;

	shader.UploadUniformInt("u_PointLightCount", pointLightCount);

	CameraSpecification cameraSpec;
	cameraSpec.Position = glm::vec3(0.0f, 0.0f, 5.0f);
	cameraSpec.WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraSpec.Yaw = -90.0f;
	cameraSpec.Pitch = -45.0f;
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
		shader.UploadUniformMat4("u_Model", model);
		textures[0].Bind();
		materialUB.SetData(materials.data());
		meshes[0].RenderMesh();

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, -2.5f));
		shader.UploadUniformMat4("u_Model", model);
		textures[1].Bind();
		materialUB.SetData(&materials[1]);
		meshes[0].RenderMesh();

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f));
		shader.UploadUniformMat4("u_Model", model);
		textures[2].Bind();
		materialUB.SetData(materials.data());
		meshes[1].RenderMesh();

		window->OnUpdate();
	}

	delete window;

	return 0;
}