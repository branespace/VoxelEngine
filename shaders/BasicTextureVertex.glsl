#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

out vec2 UV;

uniform mat4 MVP;
uniform vec3 position;

void main() {
	gl_Position = MVP * vec4(vertexPosition_modelspace + position, 1.0);
	//gl_Position = vec4(0, 0, 0, 1);
	UV = vertexUV;
}