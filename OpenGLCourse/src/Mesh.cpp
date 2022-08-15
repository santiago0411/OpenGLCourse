#include "Mesh.h"

#include <glad/glad.h>

Mesh::Mesh(Mesh&& other) noexcept
{
	m_VertexArrayId = other.m_VertexArrayId;
	other.m_VertexArrayId = 0;

	m_VertexBufferId = other.m_VertexBufferId;
	other.m_VertexBufferId = 0;

	m_IndexBufferId = other.m_IndexBufferId;
	other.m_IndexBufferId = 0;

	m_IndexCount = other.m_IndexCount;
	other.m_IndexCount = 0;
}

Mesh::~Mesh()
{
	ClearMesh();
}

void Mesh::CreateMesh(float* vertices, uint32_t* indices, uint32_t numberOfVertices, uint32_t numberOfIndices)
{
	m_IndexCount = (int32_t)numberOfIndices;

	glGenVertexArrays(1, &m_VertexArrayId);
	glBindVertexArray(m_VertexArrayId);

	glGenBuffers(1, &m_VertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, (int64_t)sizeof vertices[0] * numberOfVertices, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &m_IndexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (int64_t)sizeof indices[0] * numberOfIndices, indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof vertices[0]*5, nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof vertices[0]*5, (const void*)(sizeof vertices[0] * 3));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::RenderMesh() const
{
	glBindVertexArray(m_VertexArrayId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId);
	glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, nullptr);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::ClearMesh()
{
	if (m_VertexArrayId != 0)
	{
		glDeleteVertexArrays(1, &m_VertexArrayId);
		m_VertexArrayId = 0;
	}

	if (m_VertexBufferId != 0)
	{
		glDeleteBuffers(1, &m_VertexBufferId);
		m_VertexBufferId = 0;
	}

	if (m_IndexBufferId != 0)
	{
		glDeleteBuffers(1, &m_IndexBufferId);
		m_IndexBufferId = 0;
	}

	m_IndexCount = 0;
}
