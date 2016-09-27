#include "glew.h"
#include "glfw3.h"

#include "../render/renderBuffer.hpp"

#include "quad.hpp"
#include "../render/texture.hpp"

RenderBuffer prepareFBOQuadBuffer()
{
	struct RenderBuffer FBOQuadBuffer;

	//FBO Quad vector array
	GLuint quad_VertexArrayID;
	glGenVertexArrays(1, &quad_VertexArrayID);
	glBindVertexArray(quad_VertexArrayID);

	//FBO Quad geometry
	static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};

	//FBO Quad binding
	GLuint quad_vertexbuffer;
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
	FBOQuadBuffer.vertexBuffer = quad_vertexbuffer;

	return FBOQuadBuffer;
}
