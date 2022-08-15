#pragma once

#include <string>

class Texture2D
{
public:
	Texture2D(Texture2D&& other) noexcept;
	Texture2D(const std::string& path);
	~Texture2D();

	void Bind(uint32_t slot = 0) const;

private:
	uint32_t m_TextureId = 0;
	int32_t m_Width = 0, m_Height = 0, m_Channels = 0;
};

