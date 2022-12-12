#include "Skybox.h"

#include <iostream>
#include <stb_image.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

static uint32_t s_SkyboxIndices[] = {
	// Front
	0, 1, 2,
	2, 1, 3,
	// Right
	2, 3, 5,
	5, 3, 7,
	// Back
	5, 7, 4,
	4, 7, 6,
	// Left,
	4, 6, 0,
	0, 6, 1,
	// Top
	4, 0, 5,
	5, 0, 2,
	// Bottom
	1, 6, 3,
	3, 6, 7,
};

static float s_SkyboxVertices[] = {
	-1.0f,  1.0f, -1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	-1.0f, -1.0f, -1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	 1.0f,  1.0f, -1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	 1.0f, -1.0f, -1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,

	-1.0f,  1.0f,  1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	 1.0f,  1.0f,  1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	-1.0f, -1.0f,  1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
};

Skybox::Skybox(const std::vector<std::string>& faceLocations)
{
	m_Shader.CreateFromFile("./assets/shaders/Skybox.vert", "./assets/shaders/Skybox.frag");

	glGenTextures(1, &m_TextureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureId);

	for (size_t i = 0; i < 6; i++)
	{
		int width, height, channels;
		stbi_uc* data = stbi_load(faceLocations[i].c_str(), &width, &height, &channels, 0);

		if (!data)
		{
			std::cerr << "Failed to load texture: '" << faceLocations[i] << "'\n";
			return;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

	glTextureParameteri(m_TextureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_TextureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	m_Mesh = new Mesh(s_SkyboxVertices, s_SkyboxIndices, std::size(s_SkyboxVertices), std::size(s_SkyboxIndices));
}

Skybox::~Skybox()
{
	if (m_TextureId)
		glDeleteTextures(1, &m_TextureId);

	delete m_Mesh;
}

void Skybox::Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const
{
	const glm::mat3 noTranslationView(viewMatrix);
	glDepthMask(GL_FALSE);

	m_Shader.Bind();
	m_Shader.UploadUniformMat4("u_View", glm::mat4(noTranslationView));
	m_Shader.UploadUniformMat4("u_Projection", projectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureId);

	m_Shader.Validate();
	m_Mesh->RenderMesh();

	glDepthMask(GL_TRUE);
}
