#version 450 core

layout (location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 projection;

layout (location = 0) out vec4 o_Color;

void main()
{
	gl_Position = projection * model * vec4(pos, 1.0);
	o_Color = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
}