#include "glew.h"
#include "glfw3.h"

#include "../render/renderBuffer.hpp"
#include "../render/texture.hpp"

#include "cube.hpp"

RenderBuffer prepareCubeBuffers()
{
	struct RenderBuffer render;

	static const GLfloat g_vertex_buffer_data[] = {
		0.0f,0.0f,0.0f, //Left Side, Lower Right
		0.0f,0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 1.0f,0.0f, //Back Side, Upper Right
		0.0f,0.0f,0.0f,
		0.0f, 1.0f,0.0f,
		1.0f,0.0f, 1.0f, //Bottom, Rear Right
		0.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		1.0f, 1.0f,0.0f, //Back Side, Lower Left 
		1.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f, //Left Side, Upper Left
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f,0.0f,
		1.0f,0.0f, 1.0f, //Bottom, Near Left
		0.0f,0.0f, 1.0f,
		0.0f,0.0f,0.0f,
		0.0f, 1.0f, 1.0f, //Front, Lower Left
		0.0f,0.0f, 1.0f,
		1.0f,0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, //Right, Upper Right
		1.0f,0.0f,0.0f,
		1.0f, 1.0f,0.0f,
		1.0f,0.0f,0.0f, //Right, Lower Left
		1.0f, 1.0f, 1.0f,
		1.0f,0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, //Top, Far Right
		1.0f, 1.0f,0.0f,
		0.0f, 1.0f,0.0f,
		1.0f, 1.0f, 1.0f, //Top, Near Left
		0.0f, 1.0f,0.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, //Front, Upper Right
		0.0f, 1.0f, 1.0f,
		1.0f,0.0f, 1.0f
	};

	//Cube color definition
	static const GLfloat g_color_buffer_data[] = {
		0.5f, 0.25f, 0.1f,
		0.5f, 0.25f, 0.1f,
		0.5f, 0.25f, 0.1f,
		0.5f, 0.25f, 0.1f,
		0.5f, 0.25f, 0.1f,
		0.5f, 0.25f, 0.1f,
		0.75f, 0.5f, 0.25f,
		0.75f, 0.5f, 0.25f,
		0.75f, 0.5f, 0.25f,
		0.5f, 0.25f, 0.1f,
		0.5f, 0.25f, 0.1f,
		0.5f, 0.25f, 0.1f,
		0.5f, 0.25f, 0.1f,
		0.5f, 0.25f, 0.1f,
		0.5f, 0.25f, 0.1f,
		0.75f, 0.5f, 0.25f,
		0.75f, 0.5f, 0.25f,
		0.75f, 0.5f, 0.25f,
		0.5f, 0.25f, 0.1f,
		0.5f, 0.25f, 0.1f,
		0.5f, 0.25f, 0.1f,
		0.5f, 0.25f, 0.1f,
		0.5f, 0.25f, 0.1f,
		0.5f, 0.25f, 0.1f,
		0.5f, 0.25f, 0.1f,
		0.5f, 0.25f, 0.1f,
		0.5f, 0.25f, 0.1f,
		0.0f, 0.6f, 0.0f,
		0.0f, 0.6f, 0.0f,
		0.0f, 0.6f, 0.0f,
		0.0f, 0.6f, 0.0f,
		0.0f, 0.6f, 0.0f,
		0.0f, 0.6f, 0.0f,
		0.5f, 0.25f, 0.1f,
		0.5f, 0.25f, 0.1f,
		0.5f, 0.25f, 0.1f
	};

	static const GLfloat g_normal_buffer_data[] = {
		-1.0f, 0.0f, 0.0f, //Left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, //Back
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, -1.0f, 0.0f, //Bottom
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, -1.0f, //Back
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		-1.0f, 0.0f, 0.0f, //Left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, //Bottom
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,  //Front
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,  //Right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,  //Right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,  //Top
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,  //Top
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,  //Front
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f
	};

	//UV Buffer
	//Texture UVs go T F L
	//				 BaR Bo
	GLfloat g_uv_buffer_data[] = {
		0.5f, 0.5f,	//Left Lower Right
		0.75f, 0.5f,
		0.75f, 1.0f,
		0.25f, 0.5f, //Back Upper Right
		0.0f, 0.0f,
		0.0f, 0.5f,
		0.75f, 0.0f,	//Bottom Rear Right
		0.5f, 0.0f,
		0.75f, 0.5f,
		0.25f, 0.5f,	//Back Lower Left
		0.25f, 0.0f,
		0.0f, 0.0f,
		0.5f, 0.5f,	//Left Upper Left
		0.75f, 1.0f,
		0.5f, 1.0f,
		0.75f, 0.0f,	//Bottom Near Left
		0.5f, 0.0f,
		0.5f, 0.5f,
		0.25f, 1.0f,	//Front Lower Left
		0.25, 0.5f,
		0.5f, 0.5f,
		0.25f, 0.5f,	//Right Upper Right
		0.5f, 0.0f,
		0.5f, 0.5f,
		0.5f, 0.0f,		//Right Lower Left
		0.25f, 0.5f,
		0.25f, 0.0f,
		0.25f, 0.5f,	//Top Far Right
		0.25f, 1.0f,
		0.0f, 1.0f,
		0.25f, 0.5f,	//Top Near Left
		0.0f, 1.0f,
		0.0f, 0.5f,
		0.5f, 1.0f,		//Front Upper Right
		0.25f, 1.0f,
		0.5f, 0.5f
	};

	for (int i = 1; i < 72; i += 2)
	{
		g_uv_buffer_data[i] = 1.0f - g_uv_buffer_data[i];
	}

	//Define and bind color array
	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
	render.colorBuffer = colorbuffer;

	//Define and bind vertex buffer
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	render.vertexBuffer = vertexbuffer;

	//Define and bind normal array
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_normal_buffer_data), g_normal_buffer_data, GL_STATIC_DRAW);
	render.normalBuffer = normalbuffer;

	//Define and bind UV array
	GLuint UVBuffer;
	glGenBuffers(1, &UVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, UVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);
	render.uvBuffer = UVBuffer;

	//Load basic texture
	render.texture = loadDDS("textures/atlas.DDS");

	return render;
}