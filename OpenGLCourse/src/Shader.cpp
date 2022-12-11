#include "Shader.h"

#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#include "Utils.h"

static void DetachAndDeleteShaders(uint32_t programId, uint32_t vertexId, uint32_t fragId)
{
	glDetachShader(programId, vertexId);
	glDeleteShader(vertexId);

	if (fragId)
	{
		glDetachShader(programId, fragId);
		glDeleteShader(fragId);
	}
}

void Shader::CreateFromString(const std::string& vertexString, const std::string& fragmentString)
{
	CompileShader(vertexString, fragmentString);
}

void Shader::CreateFromString(const std::string& vertexString)
{
	CompileShader(vertexString, "");
}

void Shader::CreateFromFile(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath)
{
	const std::string vertexSource(Utils::ReadFileToString(vertexPath));
	const std::string fragmentSource(Utils::ReadFileToString(fragmentPath));
	CompileShader(vertexSource, fragmentSource);
}

void Shader::CreateFromFile(const std::filesystem::path& vertexPath)
{
	const std::string vertexSource(Utils::ReadFileToString(vertexPath));
	CompileShader(vertexSource, "");
}

void Shader::Bind() const
{
	glUseProgram(m_ShaderId);
}

void Shader::UploadUniformInt(const std::string& name, int value) const
{
	auto location = glGetUniformLocation(m_ShaderId, name.c_str());
	glUniform1i(location, value);
}

void Shader::UploadUniformFloat(const std::string& name, float value) const
{
	auto location = glGetUniformLocation(m_ShaderId, name.c_str());
	glUniform1f(location, value);
}

void Shader::UploadUniformFloat3(const std::string& name, const glm::vec3& vec) const
{
	auto location = glGetUniformLocation(m_ShaderId, name.c_str());
	glUniform3f(location, vec.x, vec.y, vec.z);
}

void Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const
{
	auto location = glGetUniformLocation(m_ShaderId, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

uint32_t Shader::AddShader(uint32_t program, const std::string& shaderCode, uint32_t shaderType)
{
	const uint32_t shader = glCreateShader(shaderType);
	const char* src = shaderCode.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	int32_t result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		char log[512] = { 0 };
		glGetShaderInfoLog(shader, sizeof log, nullptr, log);
		std::cerr << "Error compiling " << (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader:\n\t" << log;
		glDeleteShader(shader);
		return 0;
	}

	glAttachShader(program, shader);
	return shader;
}

void Shader::CompileShader(const std::string& vertexString, const std::string& fragmentString)
{
	const uint32_t program = glCreateProgram();
	const uint32_t vertexId = AddShader(program, vertexString, GL_VERTEX_SHADER);
	const uint32_t fragId = fragmentString.empty() ? 0 : AddShader(program, fragmentString, GL_FRAGMENT_SHADER);

	int32_t result;

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &result);

	if (!result)
	{
		std::cerr << "Error linking shader program.\n";
		DetachAndDeleteShaders(program, vertexId, fragId);
		glDeleteProgram(program);
		return;
	}

	glValidateProgram(program);
	glGetProgramiv(program, GL_VALIDATE_STATUS, &result);

	if (!result)
	{
		std::cerr << "Error validating shader program.\n";
		DetachAndDeleteShaders(program, vertexId, fragId);
		glDeleteProgram(program);
		return;
	}

	m_ShaderId = program;

	DetachAndDeleteShaders(program, vertexId, fragId);
}