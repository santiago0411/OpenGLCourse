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
#include "Model.h"
#include "Shader.h"
#include "ShadowMap.h"
#include "Texture2D.h"
#include "UniformBuffer.h"
#include "Window.h"

#include "OpenGLContext.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define DIRECTIONAL_LIGHT_BINDING 0
#define POINT_LIGHT_ARRAY_BINDING 1
#define MATERIAL_BINDING 2

#define MAX_POINT_LIGHTS 3

std::vector<Texture2D> g_Textures;
std::vector<Material> g_Materials;
std::vector<Mesh> g_Meshes;

#define BRICK_TEXTURE 0
#define DIRT_TEXTURE 1
#define PLAIN_TEXTURE 2

#define SHINY_MATERIAL 0
#define DULL_MATERIAL 1

Shader g_Shader;
Shader g_DirectionalShadowShader;

UniformBuffer g_MaterialUB;

Model* g_xWingModel;
Model* g_BlackHawkModel;

constexpr float ToRadians(const float& value)
{
	return value * 3.14159265f / 180.0f;
}

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

static void RenderScene(const Shader& shader)
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));
	shader.UploadUniformMat4("u_Model", model);
	g_Textures[BRICK_TEXTURE].Bind();
	g_MaterialUB.SetData(&g_Materials[SHINY_MATERIAL]);
	g_Meshes[0].RenderMesh();

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.0f, -2.5f));
	shader.UploadUniformMat4("u_Model", model);
	g_Textures[DIRT_TEXTURE].Bind();
	g_MaterialUB.SetData(&g_Materials[DULL_MATERIAL]);
	g_Meshes[0].RenderMesh();

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f));
	shader.UploadUniformMat4("u_Model", model);
	g_Textures[DIRT_TEXTURE].Bind();
	g_MaterialUB.SetData(&g_Materials[SHINY_MATERIAL]);
	g_Meshes[1].RenderMesh();

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-7.0f, 0.0f, 10.0f))
		* glm::scale(glm::mat4(1.0f), glm::vec3(0.006f, 0.006f, 0.006f));
	shader.UploadUniformMat4("u_Model", model);
	g_MaterialUB.SetData(&g_Materials[SHINY_MATERIAL]);
	g_xWingModel->Render();

	static float blackHawkAngle = 0.0f;
	blackHawkAngle += 0.1f;
	if (blackHawkAngle > 360.0f)
		blackHawkAngle = 0.1f;

	model = glm::rotate(glm::mat4(1.0f), -ToRadians(blackHawkAngle), glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 2.0f, 0.0f))
		* glm::rotate(glm::mat4(1.0f), -ToRadians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f))
		* glm::rotate(glm::mat4(1.0f), -ToRadians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f))
		* glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
	shader.UploadUniformMat4("u_Model", model);
	g_MaterialUB.SetData(&g_Materials[SHINY_MATERIAL]);
	g_BlackHawkModel->Render();
}

static void DirectionalShadowMapPass(const ShadowMap& shadowMap)
{
	OpenGLContext::SetViewport(shadowMap.GetWidth(), shadowMap.GetHeight());

	shadowMap.BeginWrite();
	OpenGLContext::ClearDepthOnly();

	g_DirectionalShadowShader.Bind();
	RenderScene(g_DirectionalShadowShader);
	shadowMap.EndWrite();
}

static void RenderPass(const Camera& camera, const ShadowMap& shadowMap)
{
	OpenGLContext::SetViewport(WINDOW_WIDTH, WINDOW_HEIGHT);
	OpenGLContext::Clear();

	g_Shader.Bind();
	g_Shader.UploadUniformFloat3("u_EyePosition", camera.GetPosition());
	g_Shader.UploadUniformMat4("u_View", camera.CalculateViewMatrix());

	shadowMap.Read();
	g_Shader.UploadUniformInt("u_Texture", 0);
	g_Shader.UploadUniformInt("u_DirectionalShadowMap", 1);

	RenderScene(g_Shader);
}

int main()
{
	WindowProps props;
	props.Title = "OpenGLApp";
	props.Width = WINDOW_WIDTH;
	props.Height = WINDOW_HEIGHT;

	auto* window = new Window(props);

	if (!window->Init())
		return -1;

	Input::SetContext(window);

	g_Textures.emplace_back("./assets/textures/brick.png");
	g_Textures.emplace_back("./assets/textures/dirt.png");
	g_Textures.emplace_back("./assets/textures/plain.png");
	
	g_Materials.emplace_back(4.0f, 256.0f);
	g_Materials.emplace_back(0.3f, 4.0f);

	g_MaterialUB = UniformBuffer(sizeof(Material), MATERIAL_BINDING);

	g_xWingModel = new Model("./assets/models/x-wing.obj");
	g_BlackHawkModel = new Model("./assets/models/uh60.obj");

	g_Meshes.emplace_back(CreatePyramid());
	g_Meshes.emplace_back(CreatePlane());

	g_Shader.CreateFromFile("./assets/shaders/VertexShader.glsl", "./assets/shaders/FragmentShader.glsl");
	g_Shader.Bind();

	g_DirectionalShadowShader.CreateFromFile("./assets/shaders/DirectionalShadowMap.vert");

	ShadowMap shadowMap(2048, 2048);

	DirectionalLight dirLight;
	dirLight.Color = glm::vec3(1.0f);
	dirLight.Direction = glm::vec3(0.0f, -15.0f, -10.0f);
	dirLight.AmbientIntensity = 0.1f;
	dirLight.DiffuseIntensity = 0.6f;

	UniformBuffer dirLightUB(sizeof(DirectionalLight), DIRECTIONAL_LIGHT_BINDING);
	dirLightUB.SetData(&dirLight);

	int pointLightCount = 0;
	auto* pointLight = new PointLight[MAX_POINT_LIGHTS];

	pointLight[0].Color = glm::vec3(0.0f, 0.0f, 1.0f);
	pointLight[0].Position = glm::vec3(0.0f);
	pointLight[0].AmbientIntensity = 0.1f;
	pointLight[0].DiffuseIntensity = 0.3f;
	pointLight[0].Constant = 0.3f;
	pointLight[0].Linear = 0.2f;
	pointLight[0].Exponent = 0.1f;
	pointLightCount++;

	pointLight[1].Color = glm::vec3(0.0f, 1.0f, 0.0f);
	pointLight[1].Position = glm::vec3(-4.0f, 2.0f, 0.0f);
	pointLight[1].AmbientIntensity = 0.0f;
	pointLight[1].DiffuseIntensity = 0.1f;
	pointLight[1].Constant = 0.3f;
	pointLight[1].Linear = 0.1f;
	pointLight[1].Exponent = 0.1f;
	pointLightCount++;

	UniformBuffer pointLightUB(sizeof(PointLight) * pointLightCount, POINT_LIGHT_ARRAY_BINDING);
	pointLightUB.SetData(pointLight);
	delete[] pointLight;

	g_Shader.UploadUniformInt("u_PointLightCount", pointLightCount);

	CameraSpecification cameraSpec;
	cameraSpec.Position = glm::vec3(0.0f, 0.0f, 5.0f);
	cameraSpec.WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraSpec.Yaw = -90.0f;
	cameraSpec.Pitch = -45.0f;
	cameraSpec.Speed = 5.0f;
	cameraSpec.TurnSpeed = 10.0f;

	Camera camera(cameraSpec);

	const float aspectRatio = (float)window->GetWindowWidth() / (float)window->GetWindowHeight();
	static glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	g_Shader.UploadUniformMat4("u_Projection", projection);

	static glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 100.0f);
	const glm::mat4 lightTransform = CalculateLightTransform(dirLight, lightProjection);
	g_Shader.UploadUniformMat4("u_LightSpaceTransform", lightTransform);

	g_DirectionalShadowShader.Bind();
	g_DirectionalShadowShader.UploadUniformMat4("u_LightSpaceTransform", lightTransform);

	static float lastFrameTime = 0.0f;

	while (!window->ShouldClose())
	{
		float time = window->GetCurrentTime();
		float deltaTime = time - lastFrameTime;
		lastFrameTime = time;

		camera.OnUpdate(deltaTime);

		DirectionalShadowMapPass(shadowMap);
		RenderPass(camera, shadowMap);

		window->OnUpdate();
	}

	delete g_xWingModel;
	delete g_BlackHawkModel;
	delete window;

	return 0;
}