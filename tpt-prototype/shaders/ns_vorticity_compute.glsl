#version 440

layout(local_size_x = 1, local_size_y = 1) in;

uniform int fieldwidth;
uniform float dx;

layout (std430, binding = 0) buffer velocity_buffer { vec2 velocity[]; };
layout (std430, binding = 1) buffer vorticity_buffer { vec2 vorticity[]; };

vec2 buf2d_velocity(uvec2 coord) {
	return velocity[coord.x + (coord.y * fieldwidth)];
}

void main() {
	uvec2 coords = gl_GlobalInvocationID.xy;

	float xL = buf2d_velocity(coords - uvec2(1, 0)).y;
	float xR = buf2d_velocity(coords + uvec2(1, 0)).y;
	float xB = buf2d_velocity(coords - uvec2(0, 1)).x;
	float xT = buf2d_velocity(coords + uvec2(0, 1)).x;

	vorticity[coords.x + (coords.y * fieldwidth)].x = (xR - xL - xT + xB)/(2.0f*dx);
}