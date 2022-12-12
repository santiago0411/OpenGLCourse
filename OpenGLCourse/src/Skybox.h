#pragma once

#include <vector>

#include "Mesh.h"
#include "Shader.h"

class Skybox
{
public:
	Skybox(const std::vector<std::string>& faceLocations);
	~Skybox();

	void Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const;

private:
	Mesh* m_Mesh;
	Shader m_Shader;

	uint32_t m_TextureId = 0;
};

