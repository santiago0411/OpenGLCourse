#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Window.h"

#include "OpenGLContext.h"

static Mesh CreatePyramid()
{
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.0f, -0.5f, 0.5f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f,
	};

	uint32_t indices[] = {
		0, 1, 3,
		1, 2, 3,
		0, 2, 3,
		0, 1, 2,
	};

	Mesh mesh;
	mesh.CreateMesh(vertices, indices, sizeof vertices, sizeof indices);
	return mesh;
}

int main()
{
	WindowProps props;
	props.Title = "OpenGLApp";
	props.Width = 1400;
	props.Height = 1050;

	Window* window = new Window(props);

	if (!window->Init())
		return -1;

	std::vector<Mesh> meshes;
	meshes.emplace_back(CreatePyramid());

	Shader shader;
	shader.CreateFromFile("./shaders/VertexShader.glsl", "./shaders/FragmentShader.glsl");
	shader.Bind();

	float aspectRatio = (float)window->GetWindowWidth() / (float)window->GetWindowHeight();
	const glm::mat4 projection = glm::perspective(45.0f, aspectRatio, 0.1f, 100.0f);

	static float angle = 0.0f;

	while (!window->ShouldClose())
	{
		OpenGLContext::Clear();

		angle += 0.5f;

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

		shader.UploadUniformMat4("model", model);
		shader.UploadUniformMat4("projection", projection);

		meshes[0].RenderMesh();

		window->OnUpdate();
	}

	delete window;
	return 0;
}