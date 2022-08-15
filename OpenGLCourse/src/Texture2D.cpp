#include "Texture2D.h"

#include <iostream>

#include <stb_image.h>
#include <glad/glad.h>

Texture2D::Texture2D(Texture2D&& other) noexcept
{
	m_TextureId = other.m_TextureId;
	other.m_TextureId = 0;
	m_Width = other.m_Width;
	other.m_Width = 0;
	m_Height = other.m_Height;
	other.m_Height = 0;
	m_Channels = other.m_Channels;
	other.m_Channels = 0;
}

Texture2D::Texture2D(const std::string& path)
{
	stbi_set_flip_vertically_on_load(true);
	stbi_uc* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 0);

	if (!data)
	{
		std::cerr << "Failed to load texture: '" << path << "'\n";
		return;
	}

	GLenum internalFormat = 0, dataFormat = 0;
	if (m_Channels == 4)
	{
		internalFormat = GL_RGBA8;
		dataFormat = GL_RGBA;
	}
	else if (m_Channels == 3)
	{
		internalFormat = GL_RGB8;
		dataFormat = GL_RGB;
	}

	if (!(internalFormat & dataFormat))
	{
		std::cerr << "Invalid texture format.\n";
		return;
	}

	glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureId);
	glTextureStorage2D(m_TextureId, 1, internalFormat, m_Width, m_Height);

	glTextureParameteri(m_TextureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_TextureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(m_TextureId, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &m_TextureId);
}

void Texture2D::Bind(uint32_t slot) const
{
	glBindTextureUnit(slot, m_TextureId);
}
