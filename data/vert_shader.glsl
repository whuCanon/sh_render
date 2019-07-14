#version 330 core

uniform mat4 model_view_proj;
uniform float ymin;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out VS_OUT{
	vec3 normal;
	float isPlane;
}vs;

void main(void) 
{
	gl_Position = model_view_proj * vec4(position,1);

	vs.normal = normal;
	
	if (ymin-position.y < 1e-5 && ymin-position.y > -1e-5)
		vs.isPlane = 1;
	else
		vs.isPlane = 0;
}