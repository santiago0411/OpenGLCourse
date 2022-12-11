#include "OmniShadowMap.h"

#include <cstdio>
#include <glad/glad.h>

OmniShadowMap::OmniShadowMap(uint32_t width, uint32_t height)
	: m_Width(width), m_Height(height)
{
	glGenFramebuffers(1, &m_FramebufferId);

	glGenTextures(1, &m_ShadowMapId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_ShadowMapId);

	for (size_t i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, (int)m_Width, (int)m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glTextureParameteri(m_ShadowMapId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_ShadowMapId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(m_ShadowMapId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_ShadowMapId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_ShadowMapId, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_ShadowMapId, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	const auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer error: %i\n", status);
}

OmniShadowMap::OmniShadowMap(OmniShadowMap&& other)
{
	m_Width = other.m_Width;
	m_Height = other.m_Height;

	m_FramebufferId = other.m_FramebufferId;
	other.m_FramebufferId = 0;
	m_ShadowMapId = other.m_ShadowMapId;
	other.m_ShadowMapId = 0;
}

OmniShadowMap::~OmniShadowMap()
{
	if (m_FramebufferId)
		glDeleteFramebuffers(1, &m_FramebufferId);
	if (m_ShadowMapId)
		glDeleteTextures(1, &m_ShadowMapId);
}

void OmniShadowMap::BeginWrite() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
}

void OmniShadowMap::EndWrite() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OmniShadowMap::Read(uint32_t offset) const
{
	glActiveTexture(GL_TEXTURE0 + offset);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_ShadowMapId);
}
