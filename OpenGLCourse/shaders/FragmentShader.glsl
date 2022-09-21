#version 450 core

out vec4 o_Color;

layout (location = 0) in vec4 v_Color;
layout (location = 1) in vec2 v_TexCoords;
layout (location = 2) in vec3 v_Normal;
layout (location = 3) in vec3 v_FragPos;

const int MAX_POINT_LIGHTS = 3;

struct LightBase
{
	vec3 Color;
	float AmbientIntensity;
	float DiffuseIntensity;
};

struct DirectionalLight
{
	LightBase Base;
	vec3 Direction;
};

struct PointLight
{
	LightBase Base;
	vec3 Position;
	float Constant;
	float Linear;
	float Exponent;
};

layout (std140, binding = 0) uniform DirectionalLightData
{
	DirectionalLight u_DirectionalLight;
};

layout (std140, binding = 1) uniform PointLightData
{
	PointLight u_PointLights[MAX_POINT_LIGHTS];
};

layout (std140, binding = 2) uniform Material
{
	float SpecularIntensity;
	float Shininess;
} u_Material;

uniform sampler2D u_Textures[32];
uniform vec3 u_EyePosition;
uniform int u_PointLightCount;

vec4 CalculateLightByDirection(LightBase light, vec3 direction)
{
	vec4 ambientColor = vec4(light.Color, 1.0f) * light.AmbientIntensity;

	float diffuseFactor = max(dot(normalize(v_Normal), normalize(direction)), 0.0f);
	vec4 diffuseColor = vec4(light.Color * light.DiffuseIntensity * diffuseFactor, 1.0f);

	vec4 specularColor = vec4(0.0f);

	if (diffuseFactor > 0.0f)
	{
		vec3 fragToEye = normalize(u_EyePosition - v_FragPos);
		vec3 reflectedVertex = normalize(reflect(direction, normalize(v_Normal)));
		float specularFactor = dot(fragToEye, reflectedVertex);

		if (specularFactor > 0.0f)
		{
			specularFactor = pow(specularFactor, u_Material.Shininess);
			specularColor = vec4(light.Color * u_Material.SpecularIntensity * specularFactor, 1.0f);
		}
	}

	return ambientColor + diffuseColor + specularColor;
}

vec4 CalculateDirectionalLight()
{
	return CalculateLightByDirection(u_DirectionalLight.Base, u_DirectionalLight.Direction);
}

vec4 CalculatePointLight()
{
	vec4 totalColor = vec4(0);

	for (int i = 0; i < u_PointLightCount; i++)
	{
		vec3 direction = v_FragPos - u_PointLights[i].Position;
		float distance = length(direction);
		direction = normalize(direction);

		vec4 color = CalculateLightByDirection(u_PointLights[i].Base, direction);

		// Ax^2 + Bx + C
		float attenuation = u_PointLights[i].Exponent * distance * distance +
							u_PointLights[i].Linear * distance +
							u_PointLights[i].Constant;

		totalColor += (color / attenuation);
	}

	return totalColor;
}

void main()
{
	vec4 finalColor = CalculateDirectionalLight();
	finalColor += CalculatePointLight();
	o_Color = texture(u_Textures[0], v_TexCoords) * finalColor;
}