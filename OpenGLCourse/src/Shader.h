#pragma once

#include <cstdint>
#include <string>
#include <filesystem>
#include <glm/fwd.hpp>

class Shader
{
public:
	Shader() = default;
	~Shader() = default;

	void CreateFromString(const std::string& vertexString, const std::string& fragmentString);
	void CreateFromFile(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);

	void Bind() const;


	void UploadUniformInt(const std::string& name, int value) const;
	void UploadUniformFloat(const std::string& name, float value) const;
	void UploadUniformFloat3(const std::string& name, const glm::vec3& vec) const;
	void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const;

private:
	static uint32_t AddShader(uint32_t program, const std::string& shaderCode, uint32_t shaderType);
	void CompileShader(const std::string& vertexString, const std::string& fragmentString);

private:
	uint32_t m_ShaderId = 0;
};
