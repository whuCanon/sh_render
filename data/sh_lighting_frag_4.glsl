#version 330

const int SH_NUM = 25;  // = 1, 4, 9, 16, 25
const int SCALE = 2550000;

uniform float basic_coeff;
uniform float light_coeffs[SH_NUM];

in VS_OUT{
	float render_coeffs[SH_NUM];
    float isPlane;
}vs;

out vec4 color;

void main(void) 
{
	float b = 0;
	float c = 0;
	for (int i = 0; i < SH_NUM; i++){
		c = c + vs.render_coeffs[i] * light_coeffs[i];
	}

	b = basic_coeff / SCALE / 1.1;
	c = c / SCALE / 1.1;

	if (c < 0)	c = 0.3;

	if (vs.isPlane-1 > -1e-5 && vs.isPlane-1 < 1e-5)
		color = vec4(c, c, c, b-c);
	else
		color = vec4(c, c, c, 1);
}