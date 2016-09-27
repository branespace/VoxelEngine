#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D textureSampler;

void main() {

	// Output color = color of the texture at the specified UV
	//color = vec4(texture(textureSampler, UV).xyz, 1.0);
	color = vec4(1, 1, 1, 1);
}