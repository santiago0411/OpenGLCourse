#version 450 core

layout (location = 0) in vec3 a_Position;

layout (location = 0) out vec3 o_TexCoords;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
	o_TexCoords = a_Position;
	gl_Position = u_Projection * u_View * vec4(a_Position, 1.0f);
}