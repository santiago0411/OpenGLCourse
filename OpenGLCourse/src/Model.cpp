#include "Model.h"

#include <iostream>

Model::Model(const std::string& filepath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);
	if (!scene)
	{
		std::cerr << "Assimp failed to import model '" << filepath << "'. " << importer.GetErrorString() << '\n';
		return;
	}

	LoadNode(scene->mRootNode, scene);
	LoadMaterials(scene);
}

Model::~Model()
{
	for (const auto& texture : m_Textures)
		delete texture;
}

void Model::Render() const
{
	for (size_t i = 0; i < m_Meshes.size(); i++)
	{
		uint32_t materialIndex = m_MeshToTex[i];
		if (materialIndex < m_Textures.size() && m_Textures[materialIndex])
			m_Textures[materialIndex]->Bind();

		m_Meshes[i].RenderMesh();
	}
}

void Model::LoadNode(aiNode* node, const aiScene* scene)
{
	for (size_t i = 0; i < node->mNumMeshes; i++)
		LoadMesh(scene->mMeshes[node->mMeshes[i]]);

	for (size_t i = 0; i < node->mNumChildren; i++)
		LoadNode(node->mChildren[i], scene);
}

void Model::LoadMesh(aiMesh* mesh)
{
	std::vector<float> vertices;
	std::vector<uint32_t> indices;

	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		vertices.insert(vertices.end(), { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });

		if (mesh->mTextureCoords[0])
			vertices.insert(vertices.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
		else
			vertices.insert(vertices.end(), { 0.0f, 0.0f });

		vertices.insert(vertices.end(), { -mesh->mNormals[i].x, -mesh->mNormals[i].y, -mesh->mNormals[i].z });
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	m_Meshes.emplace_back(vertices.data(), indices.data(), vertices.size(), indices.size());
	m_MeshToTex.push_back(mesh->mMaterialIndex);
}

void Model::LoadMaterials(const aiScene* scene)
{
	m_Textures.resize(scene->mNumMaterials);

	for (size_t i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* material = scene->mMaterials[i];
		m_Textures[i] = nullptr;

		if (material->GetTextureCount(aiTextureType_DIFFUSE))
		{
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				std::string pathStr(path.data);
				size_t idx = pathStr.rfind('\\');
				std::string filename = pathStr.substr(idx + 1);

				std::string texPath = std::string("./assets/textures/") + filename;
				m_Textures[i] = new Texture2D(texPath);

				if (!m_Textures[i]->IsLoaded())
				{
					printf("Texture '%s' for model was not found.", texPath.c_str());
					delete m_Textures[i];
					m_Textures[i] = nullptr;
				}
			}
		}

		if (!m_Textures[i])
			m_Textures[i] = new Texture2D("./assets/textures/plain.png");
	}
}
