#include "Utils.h"

#include <fstream>
#include <iostream>

std::string Utils::ReadFileToString(const std::filesystem::path& filepath)
{
	std::string result;
	std::ifstream in(filepath, std::ios::in | std::ios::binary);

	if (in)
	{
		in.seekg(0, std::ios::end);
		const size_t size = in.tellg();

		if (size != -1)
		{
			result.resize(size);
			in.seekg(0, std::ios::beg);
			in.read(&result[0], size);
		}
		else
		{
			std::cerr << "Could not read from file '" << filepath << "'\n";
		}
	}
	else
	{
		std::cerr << "Could not open shader file '" << filepath << "'\n";
	}

	return result;
}
