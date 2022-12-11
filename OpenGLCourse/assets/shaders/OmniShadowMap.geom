#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 u_LightMatrices[6];

layout (location = 0) out vec4 o_FragPos;

void main()
{
	for (int face = 0; face < 6; face++)
	{
		gl_Layer = face;
		for (int i = 0; i < 3; i++)
		{
			o_FragPos = gl_in[i].gl_Position;
			gl_Position = u_LightMatrices[face] * o_FragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}