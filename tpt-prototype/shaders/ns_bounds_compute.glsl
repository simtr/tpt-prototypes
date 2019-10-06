#version 440

layout(local_size_x = 1, local_size_y = 1) in;

uniform int fieldwidth;
uniform int fieldheight;

layout (std430, binding = 0) buffer a_buffer { vec2[] a; };
layout (std430, binding = 1) buffer b_buffer { vec2[] b; };

void main()
{
	uvec2 coords = gl_GlobalInvocationID.xy;

	int border = 5;

	if(coords.x >= border && coords.x < fieldwidth - border && coords.y >= border && coords.y < fieldheight - border)
		return;

	a[coords.x + (coords.y * fieldwidth)] = vec2(0,0);
	b[coords.x + (coords.y * fieldwidth)] = vec2(0,0);
} 