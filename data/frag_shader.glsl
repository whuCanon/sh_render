#version 330

in VS_OUT{
	vec3 normal;
	float isPlane;
}vs;

out vec4 color;

void main(void) 
{
	float r = vs.normal.x;
	float g = vs.normal.y;
	float b = vs.normal.z;

	if (vs.isPlane-1 > -1e-5 && vs.isPlane-1 < 1e-5)
		color = vec4(0,0,0,0);
	else
		color = vec4(g, g, g, 1);
}