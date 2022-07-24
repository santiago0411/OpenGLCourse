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
	int32_t ProjectionUniformId;

	explicit RenderData(const BufferIds& ids, uint32_t shaderId)
	{
		auto& [vao, ibo] = ids;

		VertexArray = vao;
		IndexBuffer = ibo;
		ShaderProgram = shaderId;
		ModelUniformId = glGetUniformLocation(shaderId, "model");
		ProjectionUniformId = glGetUniformLocation(shaderId, "projection");
	}
};

static std::string s_VertexShader = R"(
#version 450

layout (location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 projection;

layout (location = 0) out vec4 o_Color;

void main()
{
	gl_Position = projection * model * vec4(pos, 1.0);
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

static BufferIds CreatePyramid()
{
	constexpr float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.0f, -0.5f, 0.5f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f,
	};

	constexpr uint32_t indices[] = {
		0, 1, 3,
		1, 2, 3,
		0, 2, 3,
		0, 1, 2,
	};

	uint32_t vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	uint32_t vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

	uint32_t ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return { vao, ibo };
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

	constexpr int32_t width = 1400, height = 1050;
	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGLApp", nullptr, nullptr);
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

	glEnable(GL_DEPTH_TEST);

	const RenderData data(CreatePyramid(), CompileShaders());
	glUseProgram(data.ShaderProgram);
	glBindVertexArray(data.VertexArray);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.IndexBuffer);

	const glm::mat4 projection = glm::perspective(45.0f, (float)width/height, 0.1f, 100.0f);

	static float angle = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		angle += 0.5f;

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(data.ModelUniformId, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(data.ProjectionUniformId, 1, GL_FALSE, glm::value_ptr(projection));

		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}