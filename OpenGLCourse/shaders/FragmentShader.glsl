#version 450 core

out vec4 o_Color;

layout (location = 0) in vec4 col;

void main()
{
	o_Color = col;
}