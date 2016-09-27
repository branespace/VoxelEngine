#pragma once

#include <vector>

struct RenderBuffer
{
	GLuint colorBuffer;
	GLuint vertexBuffer;
	GLuint depthBuffer;
	GLuint texture;
	GLuint frameBuffer;
	GLuint renderedTexture;
	GLuint depthTexture;
	GLuint normalBuffer;
	GLuint uvBuffer;
	std::vector<GLuint> indices;
	std::vector<GLfloat> vertices;
};