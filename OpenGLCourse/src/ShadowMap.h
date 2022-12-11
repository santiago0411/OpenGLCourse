#pragma once
#include <cstdint>

class ShadowMap
{
public:
	ShadowMap(uint32_t width, uint32_t height);
	virtual ~ShadowMap();

	void BeginWrite() const;
	void EndWrite() const;
	void Read(uint32_t offset = 0) const;

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

private:
	uint32_t m_FramebufferId = 0, m_ShadowMapId = 0;
	uint32_t m_Width, m_Height;
};

