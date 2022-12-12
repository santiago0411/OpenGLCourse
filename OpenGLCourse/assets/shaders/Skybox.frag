#version 450 core

out vec4 o_Color;

layout (location = 0) in vec3 v_TexCoords;

uniform samplerCube u_Skybox;

void main()
{
	o_Color = texture(u_Skybox, v_TexCoords);
}