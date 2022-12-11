#version 450 core

out vec4 o_Color;

layout (location = 0) in vec4 v_Color;
layout (location = 1) in vec2 v_TexCoords;
layout (location = 2) in vec3 v_Normal;
layout (location = 3) in vec3 v_FragPos;
layout (location = 4) in vec4 v_DirectionalLightSpacePos;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

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

struct SpotLight
{
	PointLight Base;
	vec3 Direction;
	float Edge;
};

struct OmniShadowMap
{
	samplerCube ShadowMap;
	float FarPlane;
};

layout (std140, binding = 0) uniform DirectionalLightData
{
	DirectionalLight u_DirectionalLight;
};

layout (std140, binding = 1) uniform PointLightData
{
	PointLight u_PointLights[MAX_POINT_LIGHTS];
};

layout (std140, binding = 2) uniform SpotLightData
{
	SpotLight u_SpotLights[MAX_SPOT_LIGHTS];
};

layout (std140, binding = 3) uniform Material
{
	float SpecularIntensity;
	float Shininess;
} u_Material;

uniform sampler2D u_Texture;
uniform sampler2D u_DirectionalShadowMap;
uniform OmniShadowMap u_OmniShadowMaps[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS];

uniform vec3 u_EyePosition;
uniform int u_PointLightCount;
uniform int u_SpotLightCount;

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float CalculateDirectionalShadowFactor(DirectionalLight light)
{
	vec3 projCoords = v_DirectionalLightSpacePos.xyz / v_DirectionalLightSpacePos.w;
	projCoords = (projCoords * 0.5f) + 0.5f;

	if (projCoords.z > 1.0f)
		return 0.0f;

	float currentDepth = projCoords.z;

	vec3 normal = normalize(v_Normal);
	vec3 lightDir = normalize(light.Direction);
	float bias = max(0.05f * (1 - dot(normal, lightDir)), 0.0005f);

	float shadow = 0.0f;
	vec2 texelSize = 1.0f / textureSize(u_DirectionalShadowMap, 0);
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			float pcfDepth = texture(u_DirectionalShadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;
		}
	}

	shadow /= 9.0f;
	return shadow;
}

float CaculateOmniShadowFactor(PointLight light, int shadowIndex)
{
	vec3 fragToLight = v_FragPos - light.Position;
	float currentDepth = length(fragToLight);

	float shadow = 0.0f;
	float bias = 0.05f;
	int samples = 20;

	float viewDistance = length(u_EyePosition - v_FragPos);
	float diskRadius = (1.0f + (viewDistance / u_OmniShadowMaps[shadowIndex].FarPlane)) / 25.0f;

	for (int i = 0; i < samples; i++)
	{
		float closestDepth = texture(u_OmniShadowMaps[shadowIndex].ShadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
		closestDepth *= u_OmniShadowMaps[shadowIndex].FarPlane;
		if (currentDepth - bias > closestDepth)
			shadow += 1.0f;
	}

	shadow /= float(samples);
	return shadow;
}

vec4 CalculateLightByDirection(LightBase light, vec3 direction, float shadowFactor)
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

	return (ambientColor + (1.0f - shadowFactor) * (diffuseColor + specularColor));
}

vec4 CalculateDirectionalLight()
{
	float shadowFactor = CalculateDirectionalShadowFactor(u_DirectionalLight);
	return CalculateLightByDirection(u_DirectionalLight.Base, u_DirectionalLight.Direction, shadowFactor);
}

vec4 CalculatePointLight(PointLight light, int shadowIndex)
{
	vec3 direction = v_FragPos - light.Position;
	float distance = length(direction);
	direction = normalize(direction);

	float shadowFactor = CaculateOmniShadowFactor(light, shadowIndex);
	vec4 color = CalculateLightByDirection(light.Base, direction, shadowFactor);

	// Ax^2 + Bx + C
	float attenuation = light.Exponent * distance * distance +
						light.Linear * distance +
						light.Constant;

	return color / attenuation;
}

vec4 CalculateSpotLight(SpotLight light, int shadowIndex)
{
	vec3 rayDirection = normalize(v_FragPos - light.Base.Position);
	float slFactor = dot(rayDirection, light.Direction);

	if (slFactor > light.Edge)
	{
		vec4 color = CalculatePointLight(light.Base, shadowIndex);
		return color * (1.0f - (1.0f - slFactor) * (1.0f / (1.0f - light.Edge)));
	}

	return vec4(0);
}

vec4 CalculatePointLights()
{
	vec4 totalColor = vec4(0);
	for (int i = 0; i < u_PointLightCount; i++)
		totalColor += CalculatePointLight(u_PointLights[i], i);

	return totalColor;
}

vec4 CalculateSpotLights()
{
	vec4 totalColor = vec4(0);
	for (int i = 0; i < u_SpotLightCount; i++)
		totalColor += CalculateSpotLight(u_SpotLights[i], i + u_PointLightCount);

	return totalColor;
}

void main()
{
	vec4 finalColor = CalculateDirectionalLight();
	finalColor += CalculatePointLights();
	finalColor += CalculateSpotLights();
	o_Color = texture(u_Texture, v_TexCoords) * finalColor;
}