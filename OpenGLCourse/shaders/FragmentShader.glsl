#version 450 core

out vec4 o_Color;

layout (location = 0) in vec4 v_Color;
layout (location = 1) in vec2 v_TexCoords;
layout (location = 2) in vec3 v_Normal;
layout (location = 3) in vec3 v_FragPos;

struct DirectionalLight
{
	vec3 Color;
	float AmbientIntensity;
	vec3 Direction;
	float DiffuseIntensity;
};

struct Material
{
	float SpecularIntensity;
	float Shininess;
};

uniform sampler2D u_Textures[32];
uniform DirectionalLight u_DirectionalLight;
uniform Material u_Material;
uniform vec3 u_EyePosition;

void main()
{
	vec4 ambientColor = vec4(u_DirectionalLight.Color, 1.0f) * u_DirectionalLight.AmbientIntensity;

	float diffuseFactor = max(dot(normalize(v_Normal), normalize(u_DirectionalLight.Direction)), 0.0f);
	vec4 diffuseColor = vec4(u_DirectionalLight.Color, 1.0f) * u_DirectionalLight.DiffuseIntensity * diffuseFactor;

	vec4 specularColor = vec4(0.0f);

	if (diffuseFactor > 0.0f)
	{
		vec3 fragToEye = normalize(u_EyePosition - v_FragPos);
		vec3 reflectedVertex = normalize(reflect(u_DirectionalLight.Direction, normalize(v_Normal)));
		float specularFactor = dot(fragToEye, reflectedVertex);

		if (specularFactor > 0.0f)
		{
			specularFactor = pow(specularFactor, u_Material.Shininess);
			specularColor = vec4(u_DirectionalLight.Color * u_Material.SpecularIntensity * specularFactor, 1.0f);
		}
	}

	o_Color = texture(u_Textures[0], v_TexCoords) * (ambientColor + diffuseColor + specularColor);
}