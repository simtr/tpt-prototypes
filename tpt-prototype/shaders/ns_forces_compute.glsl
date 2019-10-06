#version 440

layout(local_size_x = 1, local_size_y = 1) in;

uniform int fieldwidth;
uniform float halfrdx;

layout (std430, binding = 0) buffer divergence_buffer { vec2[] divergence; };
layout (std430, binding = 1) buffer velocity_buffer { vec2[] velocity; };

vec2 buf2d_velocity(uvec2 coord) {
	return velocity[coord.x + (coord.y * fieldwidth)];
}

void main()
{
	uvec2 coords = gl_GlobalInvocationID.xy;
	vec2 wL = buf2d_velocity(coords - uvec2(1, 0));
	vec2 wR = buf2d_velocity(coords + uvec2(1, 0));
	vec2 wB = buf2d_velocity(coords - uvec2(0, 1));
	vec2 wT = buf2d_velocity(coords + uvec2(0, 1));

	divergence[coords.x + (coords.y * fieldwidth)] = vec2(halfrdx * ((wR.x - wL.x) + (wT.y - wB.y)));
}