#include <iostream>
#include <string>
#include <tuple>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using BufferIds = std::tuple<uint32_t, uint32_t>;

struct RenderData
{
	uint32_t VertexArray;
	uint32_t IndexBuffer;
	uint32_t ShaderProgram;
	int32_t ModelUniformId;

	explicit RenderData(const BufferIds& ids, uint32_t shaderId)
	{
		auto& [vao, ibo] = ids;

		VertexArray = vao;
		IndexBuffer = ibo;
		ShaderProgram = shaderId;
		ModelUniformId = glGetUniformLocation(shaderId, "model");
	}
};

static std::string s_VertexShader = R"(
#version 450

layout (location = 0) in vec3 pos;

uniform mat4 model;

layout (location = 0) out vec4 o_Color;

void main()
{
	gl_Position = model * vec4(pos, 1.0);
	o_Color = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
}
)";

static std::string s_FragShader = R"(
#version 450

out vec4 o_Color;

layout (location = 0) in vec4 col;

void main()
{
	o_Color = col;
}
)";

static void AddShader(uint32_t program, const std::string& shaderCode, GLenum shaderType)
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
		return;
	}

	glAttachShader(program, shader);
}

static uint32_t CompileShaders()
{
	const uint32_t program = glCreateProgram();
	AddShader(program, s_VertexShader, GL_VERTEX_SHADER);
	AddShader(program, s_FragShader, GL_FRAGMENT_SHADER);

	int32_t result;

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &result);

	if (!result)
	{
		std::cerr << "Error linking shader program.";
		return 0;
	}

	glValidateProgram(program);
	glGetProgramiv(program, GL_VALIDATE_STATUS, &result);

	if (!result)
	{
		std::cerr << "Error validating shader program.";
		return 0;
	}

	return program;
}

static BufferIds CreateTriangle()
{
	constexpr float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f,
	};

	uint32_t vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	uint32_t vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return { vao, 0 };
}

int main()
{
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW. Exiting application...\n";
		return -1;
	}

	glfwSetErrorCallback([](int32_t error, const char* description)
	{
		std::cerr << "GLFW Error (" << error << "): " << description << "\n";
	});

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1400, 1050, "OpenGLApp", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Failed to create GLFW window. Exiting application...\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize Glad. Existing application...\n";
		glfwTerminate();
		return -1;
	}

	std::cout << "OpenGL Info:\n";
	std::cout << "\tVendor: " << glGetString(GL_VENDOR) << "\n";
	std::cout << "\tRenderer: " << glGetString(GL_RENDERER) << "\n";
	std::cout << "\tVersion: " << glGetString(GL_VERSION) << "\n";

	const RenderData data(CreateTriangle(), CompileShaders());
	glUseProgram(data.ShaderProgram);
	glBindVertexArray(data.VertexArray);

	static float offset = 0.0f;
	static bool direction = false;
	constexpr float increment = 0.00075f;

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		offset += direction ? increment : -increment;
		if (std::abs(offset) >= 0.5f)
			direction = !direction;

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(offset, 0.0f, 0.0f));
		glUniformMatrix4fv(data.ModelUniformId, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}