#version 440

layout(local_size_x = 1, local_size_y = 1) in;

uniform int fieldwidth;
uniform float halfrdx;
              
layout(std430, binding = 0) buffer pressure_buffer { vec2 pressure[]; };
layout(std430, binding = 1) buffer old_velocity_buffer { vec2 old_velocity[]; };
layout(std430, binding = 2) buffer velocity_buffer { vec2 velocity[]; };

vec2 buf2d_pressure(uvec2 coord) {
	return pressure[coord.x + (coord.y * fieldwidth)];
}

vec2 buf2d_old_velocity(uvec2 coord) {
	return old_velocity[coord.x + (coord.y * fieldwidth)];
}

void main()
{
	uvec2 coords = gl_GlobalInvocationID.xy;

	float pL = buf2d_pressure(coords - uvec2(1, 0)).x;
	float pR = buf2d_pressure(coords + uvec2(1, 0)).x;
	float pB = buf2d_pressure(coords - uvec2(0, 1)).x;
	float pT = buf2d_pressure(coords + uvec2(0, 1)).x;

	vec2 new = buf2d_old_velocity(coords);
	new.xy -= halfrdx * vec2(pR - pL, pT - pB);

	velocity[coords.x + (coords.y * fieldwidth)] = new;
}
