#pragma once

#include "Shader.h"

class Material
{
public:
	Material() = default;
	Material(float specularIntensity, float shininess);
	~Material() = default;

	void UploadMaterial(const Shader& shader) const;

private:
	float m_SpecularIntensity = 0.0f;
	float m_Shininess = 0.0f;
};
