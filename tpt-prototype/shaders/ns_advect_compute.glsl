#version 440

layout(local_size_x = 1, local_size_y = 1) in;

uniform int fieldwidth;
uniform float dt;
uniform float rdx;

layout (std430, binding = 0) buffer old_velocity_buffer { vec2 old_velocity[]; };
layout (std430, binding = 1) buffer advection_buffer { vec2 advection[]; };
layout (std430, binding = 2) buffer velocity_buffer { vec2 velocity[]; };

vec2 buf2d_advection(uvec2 coord) {
	return advection[coord.x + (coord.y * fieldwidth)];
}

vec2 buf2d_old_velocity(uvec2 coord) {
	return old_velocity[coord.x + (coord.y * fieldwidth)];
}

vec2 buf2d_bilerp_advection(vec2 coord) {
	vec4 st = vec4(floor(coord), 0, 0);
    st.zw = st.xy + 1;
 
	vec2 t = coord - st.xy;
   
	vec2 tex11 = buf2d_advection(uvec2(st.xy));
	vec2 tex21 = buf2d_advection(uvec2(st.zy));
	vec2 tex12 = buf2d_advection(uvec2(st.xw));
	vec2 tex22 = buf2d_advection(uvec2(st.zw));

	return mix(mix(tex11, tex21, t.x), mix(tex12, tex22, t.x), t.y);
}

void main() {

	uvec2 coords = gl_GlobalInvocationID.xy;
	vec2 pos = coords - (dt * rdx * buf2d_old_velocity(coords));
	velocity[coords.x + (coords.y * fieldwidth)] = buf2d_bilerp_advection(pos);
}