#include "Material.h"

Material::Material(float specularIntensity, float shininess)
	: m_SpecularIntensity(specularIntensity), m_Shininess(shininess)
{
}

void Material::UploadMaterial(const Shader& shader) const
{
	shader.UploadUniformFloat("u_Material.SpecularIntensity", m_SpecularIntensity);
	shader.UploadUniformFloat("u_Material.Shininess", m_Shininess);
}
