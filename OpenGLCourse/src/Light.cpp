#include "Light.h"

Light::Light(const LightSpecification& spec)
	: m_Color(spec.Color), m_AmbientIntensity(spec.AmbientIntensity), m_Direction(spec.Direction), m_DiffuseIntensity(spec.DiffuseIntensity)
{

}

void Light::UploadLight(const Shader& shader)
{
	shader.UploadUniformFloat3("u_DirectionalLight.Color", m_Color);
	shader.UploadUniformFloat("u_DirectionalLight.AmbientIntensity", m_AmbientIntensity);
	shader.UploadUniformFloat3("u_DirectionalLight.Direction", m_Direction);
	shader.UploadUniformFloat("u_DirectionalLight.DiffuseIntensity", m_DiffuseIntensity);
}
