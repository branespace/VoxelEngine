#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 offset;

// Values that stay constant for the whole mesh.
uniform mat4 depthMVP;

void main() {
	gl_Position = depthMVP * vec4(vertexPosition_modelspace + offset, 1);
}