#version 440

uniform sampler2D texture;

in vec2 texCoord;
out vec4 fragColour;

void main() { 
	fragColour = texture2D(texture, texCoord.st);
}