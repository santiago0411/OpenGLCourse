#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoords;
layout (location = 2) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_LightSpaceTransform;

layout (location = 0) out vec4 o_Color;
layout (location = 1) out vec2 o_TexCoords;
layout (location = 2) out vec3 o_Normal;
layout (location = 3) out vec3 o_FragPos;
layout (location = 4) out vec4 o_DirectionalLightSpacePos;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0f);
	o_DirectionalLightSpacePos = u_LightSpaceTransform * u_Model * vec4(a_Position, 1.0f);
	o_Color = vec4(clamp(a_Position, 0.0f, 1.0f), 1.0f);
	o_TexCoords = a_TexCoords;
	o_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
	o_FragPos = (u_Model * vec4(a_Position, 1.0f)).xyz;
}