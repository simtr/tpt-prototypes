#version 440

uniform int fieldwidth;
uniform int fieldheight;

layout (std430, binding=0)  buffer velocity_buffer { vec2 velocity[]; };
layout (std430, binding=1)  buffer pressure_buffer { vec2 pressure[]; };
in vec2 texCoord;
out vec4 fragColour;

void main() {
	uvec2 coord = uvec2((texCoord.x * fieldwidth), (texCoord.y * fieldheight));
	fragColour = vec4(velocity[(coord.y * fieldwidth) + coord.x] + vec2(0.5f, 0.5f), pressure[(coord.y * fieldwidth) + coord.x].x + 0.5f, 0);
}