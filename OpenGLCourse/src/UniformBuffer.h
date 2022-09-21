#pragma once

#include <cstdint>

class UniformBuffer
{
public:
	UniformBuffer(size_t size, uint32_t binding);
	~UniformBuffer();

	void SetData(const void* data, uint32_t offset = 0) const;

private:
	size_t m_Size;
	uint32_t m_RendererId = 0;
};
