#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct LightBase
{
	glm::vec3 Color{ 1.0f }; // 0
	float AmbientIntensity = 0.0f; // 12
	float DiffuseIntensity = 0.0f; // 16
	glm::vec3 Padding{ 0.0f }; // 20 - FILL TO 32
};

struct DirectionalLight : LightBase
{
	glm::vec3 Direction{ 0.0f }; // 32
	float DirPadding = 0.0f; // 44 - FILL TO 48
};

struct PointLight : LightBase
{
	glm::vec3 Position{ 0.0f }; // 32
	float Constant = 0.0f; // 44
	float Linear = 0.0f; // 48
	float Exponent = 0.0f; // 52
	glm::vec2 PointPadding{ 0.0f }; // 56 - FILL TO 64
};

struct SpotLight : PointLight
{
	glm::vec3 Direction{ 0.0f }; // 64
	float Edge = 0.0f; // 76 - 80
};

inline float SpotLightEdge(float edge)
{
	return cosf(glm::radians(edge));
}

inline glm::vec3 SpotLightDirection(const glm::vec3& direction)
{
	return glm::normalize(direction);
}

inline glm::mat4 CalculateLightTransform(const DirectionalLight& light, const glm::mat4& lightProjection)
{
	return lightProjection * glm::lookAt(-light.Direction, glm::vec3(0), glm::vec3(0, 1, 0));
}

inline std::vector<glm::mat4> CalculateLightTransform(const PointLight& light, const glm::mat4& lightProjection)
{
	std::vector<glm::mat4> lightMatrices;

	// +X -X
	lightMatrices.push_back(lightProjection * glm::lookAt(light.Position, light.Position + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)));
	lightMatrices.push_back(lightProjection * glm::lookAt(light.Position, light.Position + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)));

	// +Y -Y
	lightMatrices.push_back(lightProjection * glm::lookAt(light.Position, light.Position + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)));
	lightMatrices.push_back(lightProjection * glm::lookAt(light.Position, light.Position + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)));

	// +Z -Z
	lightMatrices.push_back(lightProjection * glm::lookAt(light.Position, light.Position + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)));
	lightMatrices.push_back(lightProjection * glm::lookAt(light.Position, light.Position + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0)));

	return lightMatrices;
}