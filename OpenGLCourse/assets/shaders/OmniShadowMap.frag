#version 450 core

layout (location = 0) in vec4 v_FragPos;

uniform vec3 u_LightPos;
uniform float u_FarPlane;

void main()
{
	float distance = length(v_FragPos.xyz - u_LightPos) / u_FarPlane;
	gl_FragDepth = distance;
}