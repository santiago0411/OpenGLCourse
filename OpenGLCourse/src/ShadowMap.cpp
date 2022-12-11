#include "ShadowMap.h"

#include <cstdio>
#include <glad/glad.h>

ShadowMap::ShadowMap(uint32_t width, uint32_t height)
	: m_Width(width), m_Height(height)
{
	glGenFramebuffers(1, &m_FramebufferId);

	glGenTextures(1, &m_ShadowMapId);
	glBindTexture(GL_TEXTURE_2D, m_ShadowMapId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (int)m_Width, (int)m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glTextureParameteri(m_ShadowMapId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_ShadowMapId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(m_ShadowMapId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTextureParameteri(m_ShadowMapId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	constexpr float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTextureParameterfv(m_ShadowMapId, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowMapId, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	const auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer error: %i\n", status);
}

ShadowMap::~ShadowMap()
{
	if (m_FramebufferId)
		glDeleteFramebuffers(1, &m_FramebufferId);
	if (m_ShadowMapId)
		glDeleteTextures(1, &m_ShadowMapId);
}

void ShadowMap::BeginWrite() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
}

void ShadowMap::EndWrite() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::Read() const
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_ShadowMapId);
}
