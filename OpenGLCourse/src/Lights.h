#pragma once

#include <glm/glm.hpp>

struct DirectionalLight
{
	glm::vec3 Color{ 1.0f };
	float AmbientIntensity = 0.0f;
	glm::vec3 Direction{ 0.0f };
	float DiffuseIntensity = 0.0f;
};

struct PointLight
{
	glm::vec3 Color{ 1.0f };
	float AmbientIntensity = 0.0f;
	glm::vec3 Position{ 0.0f };
	float DiffuseIntensity = 0.0f;
	float Constant = 0.0f;
	float Linear = 0.0f;
	float Exponent = 0.0f;
	float Pad = 0.0f;
};

struct SpotLight
{
	
};