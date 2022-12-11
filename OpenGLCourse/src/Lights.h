#pragma once

#include <glm/glm.hpp>

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