#version 330 core

//spherical harmonics shader for order = 4

uniform mat4 model_view_proj;

layout (location = 0) in vec3 position;
layout (location = 3) in vec4 render_coeffs[6];
layout (location = 9) in float render_coeffs_25;

out VS_OUT{
	float render_coeffs[25];
}vs;

void main(void) 
{
	gl_Position = model_view_proj * vec4(position,1);

	int i = 0;
	int index = 0;
	while (index < 6) {
		index = i / 4;
		vs.render_coeffs[i++] = render_coeffs[index].x;
		vs.render_coeffs[i++] = render_coeffs[index].y;
		vs.render_coeffs[i++] = render_coeffs[index].z;
		vs.render_coeffs[i++] = render_coeffs[index].w;
	}
	vs.render_coeffs[24] = render_coeffs_25;
}