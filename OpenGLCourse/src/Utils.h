#pragma once

#include <string>
#include <filesystem>

class Utils
{
public:
	static std::string ReadFileToString(const std::filesystem::path& filepath);
};
