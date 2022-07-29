#pragma once

#include <cstdint>

class Mesh
{
public:
	Mesh() = default;
	Mesh(const Mesh& other) = default;
	Mesh(Mesh&& other) noexcept;
	~Mesh();

	void CreateMesh(float* vertices, uint32_t* indices, uint32_t numberOfVertices, uint32_t numberOfIndices);
	void RenderMesh() const;
	void ClearMesh();

private:
	uint32_t m_VertexArrayId = 0;
	uint32_t m_VertexBufferId = 0;
	uint32_t m_IndexBufferId = 0;
	int32_t m_IndexCount = 0;
};
