#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) out vec4 o_Color;
layout (location = 1) out vec2 o_TexCoords;

void main()
{
	gl_Position = projection * view * model * vec4(pos, 1.0);
	o_Color = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
	o_TexCoords = texCoords;
}