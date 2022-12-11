#pragma once

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Texture2D.h"

class Model
{
public:
	Model(const std::string& filepath);
	~Model();

	void Render() const;

private:
	void LoadNode(aiNode* node, const aiScene* scene);
	void LoadMesh(aiMesh* mesh);
	void LoadMaterials(const aiScene* scene);

private:
	std::vector<Mesh> m_Meshes;
	std::vector<Texture2D*> m_Textures;
	std::vector<uint32_t> m_MeshToTex;
};

