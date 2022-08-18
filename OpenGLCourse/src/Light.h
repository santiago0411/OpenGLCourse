#pragma once

#include <glm/glm.hpp>

#include "Shader.h"

struct LightSpecification
{
	glm::vec3 Color;
	float AmbientIntensity;
	glm::vec3 Direction;
	float DiffuseIntensity;
};

class Light
{
public:
	Light() = default;
	Light(const LightSpecification& spec);
	virtual ~Light() = default;

	virtual void UploadLight(const Shader& shader);

private:
	glm::vec3 m_Color{ 1.0f };
	float m_AmbientIntensity = 1.0f;

	glm::vec3 m_Direction{ 0.0f, -1.0f, 0.0f };
	float m_DiffuseIntensity = 0.0f;
};

