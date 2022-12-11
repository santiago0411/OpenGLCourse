#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct LightBase
{
	glm::vec3 Color{ 1.0f };
	float AmbientIntensity = 0.0f;
	float DiffuseIntensity = 0.0f;
	glm::vec3 Padding{ 0.0f };
};

struct DirectionalLight : LightBase
{
	glm::vec3 Direction{ 0.0f };
	float DirPadding = 0.0f;
};

struct PointLight : LightBase
{
	glm::vec3 Position{ 0.0f };
	float Constant = 0.0f;
	float Linear = 0.0f;
	float Exponent = 0.0f;
	glm::vec2 PointPadding{ 0.0f };
};

inline glm::mat4 CalculateLightTransform(const DirectionalLight& light, const glm::mat4& lightProjection)
{
	return lightProjection * glm::lookAt(-light.Direction, glm::vec3(0), glm::vec3(0, 1, 0));
}