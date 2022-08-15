#version 450 core

out vec4 o_Color;

layout (location = 0) in vec4 col;
layout (location = 1) in vec2 texCoords;

layout (binding = 0) uniform sampler2D u_Textures[32];

void main()
{
	o_Color = texture(u_Textures[0], texCoords);
}