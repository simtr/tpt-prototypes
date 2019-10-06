#version 440

in vec2 vertexPos2D;
out vec2 texCoord;

void main() {
	gl_Position = vec4(vertexPos2D.x, vertexPos2D.y, 0, 1);
	texCoord = (vertexPos2D * vec2(0.5, -0.5)) + vec2(0.5, 0.5);
}