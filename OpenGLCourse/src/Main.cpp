#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Lights.h"
#include "Input.h"
#include "Material.h"
#include "Mesh.h"
#include "Model.h"
#include "OmniShadowMap.h"
#include "Shader.h"
#include "ShadowMap.h"
#include "Skybox.h"
#include "Texture2D.h"
#include "UniformBuffer.h"
#include "Window.h"

#include "OpenGLContext.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define LIGHT_MATRICES_BINDING 0

#define DIRECTIONAL_LIGHT_BINDING 0
#define POINT_LIGHT_ARRAY_BINDING 1
#define SPOT_LIGHT_ARRAY_BINDING 2
#define MATERIAL_BINDING 3

#define MAX_POINT_LIGHTS 3

Window* g_Window;

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
Shader g_OmniDirectionalShadowShader;

UniformBuffer* g_MaterialUB;
UniformBuffer* g_SpotLightUB;

Model* g_xWingModel;
Model* g_BlackHawkModel;

std::vector<PointLight> g_PointLights;
std::vector<OmniShadowMap> g_PointLightOmniShadowMaps;

std::vector<SpotLight> g_SpotLights;
std::vector<OmniShadowMap> g_SpotLightOmniShadowMaps;

#define ASPECT_RATIO ((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT)
static glm::mat4 g_CameraProjection = glm::perspective(glm::radians(60.0f), ASPECT_RATIO, 0.1f, 100.0f);

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
	g_MaterialUB->SetData(&g_Materials[SHINY_MATERIAL]);
	g_Meshes[0].RenderMesh();

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.0f, -2.5f));
	shader.UploadUniformMat4("u_Model", model);
	g_Textures[DIRT_TEXTURE].Bind();
	g_MaterialUB->SetData(&g_Materials[DULL_MATERIAL]);
	g_Meshes[0].RenderMesh();

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f));
	shader.UploadUniformMat4("u_Model", model);
	g_Textures[DIRT_TEXTURE].Bind();
	g_MaterialUB->SetData(&g_Materials[SHINY_MATERIAL]);
	g_Meshes[1].RenderMesh();

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-7.0f, 0.0f, 10.0f))
		* glm::scale(glm::mat4(1.0f), glm::vec3(0.006f, 0.006f, 0.006f));
	shader.UploadUniformMat4("u_Model", model);
	g_MaterialUB->SetData(&g_Materials[SHINY_MATERIAL]);
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
	g_MaterialUB->SetData(&g_Materials[SHINY_MATERIAL]);
	g_BlackHawkModel->Render();
}

static void DirectionalShadowMapPass(const ShadowMap& shadowMap)
{
	OpenGLContext::SetViewport(shadowMap.GetWidth(), shadowMap.GetHeight());

	shadowMap.BeginWrite();
	OpenGLContext::ClearDepthOnly();

	g_DirectionalShadowShader.Bind();
	g_DirectionalShadowShader.Validate();
	RenderScene(g_DirectionalShadowShader);
	shadowMap.EndWrite();
}

static void OmniShadowMapPass(const PointLight& light, const OmniShadowMap& shadowMap)
{
	OpenGLContext::SetViewport(shadowMap.GetWidth(), shadowMap.GetHeight());

	shadowMap.BeginWrite();
	OpenGLContext::ClearDepthOnly();

	g_OmniDirectionalShadowShader.Bind();
	g_OmniDirectionalShadowShader.UploadUniformFloat3("u_LightPos", light.Position);
	g_OmniDirectionalShadowShader.UploadUniformFloat("u_FarPlane", 100.0f);

	static glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.01f, 100.0f);
	std::vector<glm::mat4> lightMatrices = CalculateLightTransform(light, lightProjection);

	g_OmniDirectionalShadowShader.UploadUniformMat4("u_LightMatrices[0]", lightMatrices[0]);
	g_OmniDirectionalShadowShader.UploadUniformMat4("u_LightMatrices[1]", lightMatrices[1]);
	g_OmniDirectionalShadowShader.UploadUniformMat4("u_LightMatrices[2]", lightMatrices[2]);
	g_OmniDirectionalShadowShader.UploadUniformMat4("u_LightMatrices[3]", lightMatrices[3]);
	g_OmniDirectionalShadowShader.UploadUniformMat4("u_LightMatrices[4]", lightMatrices[4]);
	g_OmniDirectionalShadowShader.UploadUniformMat4("u_LightMatrices[5]", lightMatrices[5]);

	g_OmniDirectionalShadowShader.Validate();
	RenderScene(g_OmniDirectionalShadowShader);
	shadowMap.EndWrite();
}

static void RenderPass(const Camera& camera, const ShadowMap& shadowMap, const Skybox& skybox)
{
	OpenGLContext::SetViewport(WINDOW_WIDTH, WINDOW_HEIGHT);
	OpenGLContext::Clear();

	skybox.Draw(camera.CalculateViewMatrix(), g_CameraProjection);

	g_Shader.Bind();
	g_Shader.UploadUniformFloat3("u_EyePosition", camera.GetPosition());
	g_Shader.UploadUniformMat4("u_View", camera.CalculateViewMatrix());

	shadowMap.Read(2);
	g_Shader.UploadUniformInt("u_Texture", 1);
	g_Shader.UploadUniformInt("u_DirectionalShadowMap", 2);

	for (size_t i = 0; i < g_PointLights.size(); i++)
	{
		const int textureUnit = 3;
		g_PointLightOmniShadowMaps[i].Read(textureUnit + i);
		std::string uniformName = std::format("u_OmniShadowMaps[{}]", i);
		g_Shader.UploadUniformInt(uniformName + std::string(".ShadowMap"), textureUnit + i);
		g_Shader.UploadUniformFloat(uniformName + std::string(".FarPlane"), 100.0f);
	}

	for (size_t i = 0; i < g_SpotLights.size(); i++)
	{
		const int textureUnit = 3 + g_PointLights.size();
		g_SpotLightOmniShadowMaps[i].Read(textureUnit + i);
		std::string uniformName = std::format("u_OmniShadowMaps[{}]", i + g_PointLights.size());
		g_Shader.UploadUniformInt(uniformName + std::string(".ShadowMap"), textureUnit + i);
		g_Shader.UploadUniformFloat(uniformName + std::string(".FarPlane"), 100.0f);
	}

	if (!g_SpotLights.empty())
	{
		glm::vec3 lowerLight = camera.GetPosition();
		lowerLight.y -= 0.3f;
		g_SpotLights[0].Position = lowerLight;
		g_SpotLights[0].Direction = camera.GetDirection();
		g_SpotLightUB->SetData(g_SpotLights.data());
	}
	g_Shader.Validate();
	RenderScene(g_Shader);
}

int main()
{
	WindowProps props;
	props.Title = "OpenGLApp";
	props.Width = WINDOW_WIDTH;
	props.Height = WINDOW_HEIGHT;

	g_Window = new Window(props);

	if (!g_Window->Init())
		return -1;

	Input::SetContext(g_Window);

	g_Textures.emplace_back("./assets/textures/brick.png");
	g_Textures.emplace_back("./assets/textures/dirt.png");
	g_Textures.emplace_back("./assets/textures/plain.png");
	
	g_Materials.emplace_back(4.0f, 256.0f);
	g_Materials.emplace_back(0.3f, 4.0f);

	g_MaterialUB = new UniformBuffer(sizeof(Material), MATERIAL_BINDING);

	g_xWingModel = new Model("./assets/models/x-wing.obj");
	g_BlackHawkModel = new Model("./assets/models/uh60.obj");

	g_Meshes.emplace_back(CreatePyramid());
	g_Meshes.emplace_back(CreatePlane());

	g_Shader.CreateFromFile("./assets/shaders/VertexShader.glsl", "./assets/shaders/FragmentShader.glsl");
	g_Shader.Bind();

	g_DirectionalShadowShader.CreateFromFile("./assets/shaders/DirectionalShadowMap.vert");
	g_OmniDirectionalShadowShader.CreateFromFile("./assets/shaders/OmniShadowMap.vert", "./assets/shaders/OmniShadowMap.geom", "./assets/shaders/OmniShadowMap.frag");

	DirectionalLight dirLight;
	dirLight.Color = glm::vec3(1.0f, 0.9f, 0.3f);
	dirLight.Direction = glm::vec3(-10.0f, -12.0f, 18.5f);
	dirLight.AmbientIntensity = 0.1f;
	dirLight.DiffuseIntensity = 0.8f;
	ShadowMap shadowMap(2048, 2048);

	UniformBuffer dirLightUB(sizeof(DirectionalLight), DIRECTIONAL_LIGHT_BINDING);
	dirLightUB.SetData(&dirLight);


	PointLight& pointLight1 = g_PointLights.emplace_back();
	pointLight1.Color = glm::vec3(0.0f, 1.0f, 0.0f);
	pointLight1.Position = glm::vec3(-2.0f, 2.0f, 0.0f);
	pointLight1.AmbientIntensity = 0.0f;
	pointLight1.DiffuseIntensity = 0.4f;
	pointLight1.Constant = 0.3f;
	pointLight1.Linear = 0.2f;
	pointLight1.Exponent = 0.1f;
	g_PointLightOmniShadowMaps.emplace_back(1024, 1024);

	PointLight& pointLight2 = g_PointLights.emplace_back();
	pointLight2.Color = glm::vec3(0.0f, 0.0f, 1.0f);
	pointLight2.Position = glm::vec3(2.0f, 2.0f, 0.0f);
	pointLight2.AmbientIntensity = 0.0f;
	pointLight2.DiffuseIntensity = 0.4f;
	pointLight2.Constant = 0.3f;
	pointLight2.Linear = 0.2f;
	pointLight2.Exponent = 0.1f;
	g_PointLightOmniShadowMaps.emplace_back(1024, 1024);

	UniformBuffer pointLightUB(sizeof(PointLight) * g_PointLights.size(), POINT_LIGHT_ARRAY_BINDING);
	pointLightUB.SetData(g_PointLights.data());

	g_Shader.UploadUniformInt("u_PointLightCount", (int)g_PointLights.size());

	SpotLight& spotLight1 = g_SpotLights.emplace_back();
	spotLight1.Color = glm::vec3(1.0f);
	spotLight1.AmbientIntensity = 0.0f;
	spotLight1.DiffuseIntensity = 0.2f;
	spotLight1.Position = glm::vec3(0.0f);
	spotLight1.Direction = SpotLightDirection(glm::vec3(0.0f, -1.0f, 0.0f));
	spotLight1.Constant = 1.0f;
	spotLight1.Linear = 0.0f;
	spotLight1.Exponent = 0.0f;
	spotLight1.Edge = SpotLightEdge(20.0f);
	g_SpotLightOmniShadowMaps.emplace_back(1024, 1024);

	SpotLight& spotLight2 = g_SpotLights.emplace_back();
	spotLight2.Color = glm::vec3(1.0f);
	spotLight2.AmbientIntensity = 0.0f;
	spotLight2.DiffuseIntensity = 1.0f;
	spotLight2.Position = glm::vec3(0.0f, -1.5f, 0.0f);
	spotLight2.Direction = SpotLightDirection(glm::vec3(-100.0f, -1.0f, 0.0f));
	spotLight2.Constant = 1.0f;
	spotLight2.Linear = 0.0f;
	spotLight2.Exponent = 0.0f;
	spotLight2.Edge = SpotLightEdge(20.0f);
	g_SpotLightOmniShadowMaps.emplace_back(1024, 1024);

	SpotLight& spotLight3 = g_SpotLights.emplace_back();
	spotLight3.Color = glm::vec3(1.0f, 0.0f, 0.0f);
	spotLight3.AmbientIntensity = 0.0f;
	spotLight3.DiffuseIntensity = 1.0f;
	spotLight3.Position = glm::vec3(1.5f, 1.5f, 0.0f);
	spotLight3.Direction = SpotLightDirection(glm::vec3(-100.0f, -1.0f, 0.0f));
	spotLight3.Constant = 1.0f;
	spotLight3.Linear = 0.0f;
	spotLight3.Exponent = 0.0f;
	spotLight3.Edge = SpotLightEdge(40.0f);
	g_SpotLightOmniShadowMaps.emplace_back(1024, 1024);

	g_SpotLightUB = new UniformBuffer(sizeof(SpotLight) * g_SpotLights.size(), SPOT_LIGHT_ARRAY_BINDING);
	g_SpotLightUB->SetData(g_SpotLights.data());

	g_Shader.UploadUniformInt("u_SpotLightCount", (int)g_SpotLights.size());

	CameraSpecification cameraSpec;
	cameraSpec.Position = glm::vec3(0.0f, 0.0f, 5.0f);
	cameraSpec.WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraSpec.Yaw = -60.0f;
	cameraSpec.Pitch = 0.0f;
	cameraSpec.Speed = 5.0f;
	cameraSpec.TurnSpeed = 10.0f;

	Camera camera(cameraSpec);
	g_Shader.UploadUniformMat4("u_Projection", g_CameraProjection);

	static glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 100.0f);
	const glm::mat4 lightTransform = CalculateLightTransform(dirLight, lightProjection);
	g_Shader.UploadUniformMat4("u_LightSpaceTransform", lightTransform);

	g_DirectionalShadowShader.Bind();
	g_DirectionalShadowShader.UploadUniformMat4("u_LightSpaceTransform", lightTransform);

	std::vector<std::string> skyboxFaces {
		"./assets/textures/Skybox/cupertin-lake_rt.tga",
		"./assets/textures/Skybox/cupertin-lake_lf.tga",
		"./assets/textures/Skybox/cupertin-lake_up.tga",
		"./assets/textures/Skybox/cupertin-lake_dn.tga",
		"./assets/textures/Skybox/cupertin-lake_bk.tga",
		"./assets/textures/Skybox/cupertin-lake_ft.tga",
	};

	Skybox skybox(skyboxFaces);

	static float lastFrameTime = 0.0f;

	while (!g_Window->ShouldClose())
	{
		float time = g_Window->GetCurrentTime();
		float deltaTime = time - lastFrameTime;
		lastFrameTime = time;

		camera.OnUpdate(deltaTime);

		DirectionalShadowMapPass(shadowMap);
		for (size_t i = 0; i < g_PointLights.size(); i++)
			OmniShadowMapPass(g_PointLights[i], g_PointLightOmniShadowMaps[i]);
		for (size_t i = 0; i < g_SpotLights.size(); i++)
			OmniShadowMapPass(g_SpotLights[i], g_SpotLightOmniShadowMaps[i]);
		RenderPass(camera, shadowMap, skybox);

		g_Window->OnUpdate();
	}

	delete g_xWingModel;
	delete g_BlackHawkModel;
	delete g_MaterialUB;
	delete g_SpotLightUB;
	delete g_Window;

	return 0;
}