#version 440

#define rsqrt inversesqrt

layout(local_size_x = 1, local_size_y = 1) in;

uniform int fieldwidth;
uniform float epsilon;
uniform float dx;
uniform float dt;

layout (std430, binding = 0) buffer old_velocity_buffer { vec2[] old_velocity; }; // v
layout (std430, binding = 1) buffer voricity_buffer { vec2[] voricity; }; //w
layout (std430, binding = 2) buffer velocity_buffer { vec2[] velocity; };

vec2 buf2d_old_velocity(uvec2 coord) {
	return old_velocity[coord.x + (coord.y * fieldwidth)];
}

vec2 buf2d_vorticity(uvec2 coord) {
	return voricity[coord.x + (coord.y * fieldwidth)];
}

void main()
{
	uvec2 coords = gl_GlobalInvocationID.xy;

	float wL = buf2d_vorticity(coords - uvec2(1, 0)).x;
  	float wR = buf2d_vorticity(coords + uvec2(1, 0)).x;
  	float wB = buf2d_vorticity(coords - uvec2(0, 1)).x;
  	float wT = buf2d_vorticity(coords + uvec2(0, 1)).x;

	float wC = buf2d_vorticity(coords).x;

	vec2 curl = vec2(0.3);

    vec2 force = dx * vec2(abs(wT) - abs(wB), abs(wR) - abs(wL));
    force *= inversesqrt(max(epsilon, dot(force, force))) * curl * wC;
    force.y *= -1.0;

    vec2 velc = buf2d_old_velocity(coords);
    velocity[coords.x + (coords.y * fieldwidth)] = velc + (dt * force);
}