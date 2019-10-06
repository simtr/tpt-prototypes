#version 440
            
layout(local_size_x = 1, local_size_y = 1) in;

uniform int fieldwidth;
uniform float alpha;
uniform float rBeta;

layout (std430, binding = 0) buffer b_buffer { vec2[] b; };
layout (std430, binding = 1) buffer x_buffer { vec2[] x; };
layout (std430, binding = 2) buffer output_buffer { vec2[] outp; };

vec2 buf2d_x(uvec2 coord) {
	return x[coord.x + (coord.y * fieldwidth)];
}

vec2 buf2d_b(uvec2 coord) {
	return b[coord.x + (coord.y * fieldwidth)];
}

void main()
{
	uvec2 coords = gl_GlobalInvocationID.xy;

	vec2 xL = buf2d_x(coords - uvec2(1, 0));
	vec2 xR = buf2d_x(coords + uvec2(1, 0));
	vec2 xB = buf2d_x(coords - uvec2(0, 1));
	vec2 xT = buf2d_x(coords + uvec2(0, 1));
  
	vec2 bC = buf2d_b(coords);

	outp[coords.x + (coords.y * fieldwidth)] = (xL + xR + xB + xT + alpha * bC) * rBeta;
}