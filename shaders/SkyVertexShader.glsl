#version 330 core

layout(location = 0) in vec3 vertex_coordinates;

void main(void) {
	gl_Position = vec4(vertex_coordinates, 1.0);
}