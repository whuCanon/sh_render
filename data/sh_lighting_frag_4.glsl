#version 330

const int SH_NUM = 25;  // = 1, 4, 9, 16, 25

uniform float light_coeffs[SH_NUM];

in VS_OUT{
	float render_coeffs[SH_NUM];
}vs;

out vec4 color;

void main(void) 
{
	float c = 0;
	for (int i = 0; i < SH_NUM; i++){
		c = c + vs.render_coeffs[i] * light_coeffs[i];
	}
	c = c / 255;
	if (c < 0)	c = 0.3;
	color = vec4(c, c, c, 1);
}