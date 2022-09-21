#pragma once

struct Material
{
	float SpecularIntensity = 0.0f;
	float Shininess = 0.0f;

	static constexpr uint8_t GPU_SIZE = 16;
};
