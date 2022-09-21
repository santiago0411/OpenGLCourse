#include "UniformBuffer.h"

#include <glad/glad.h>

UniformBuffer::UniformBuffer(size_t size, uint32_t binding)
	: m_Size(size)
{
	glCreateBuffers(1, &m_RendererId);
	glNamedBufferData(m_RendererId, size, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererId);
}

UniformBuffer::~UniformBuffer()
{
	glDeleteBuffers(1, &m_RendererId);
}

void UniformBuffer::SetData(const void* data, uint32_t offset) const
{
	glNamedBufferSubData(m_RendererId, offset, (GLsizeiptr)m_Size, data);
}
