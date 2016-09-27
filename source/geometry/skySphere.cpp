#define _USE_MATH_DEFINES 
#include <vector>
#include <cmath>

#include "glew.h"
#include "glfw3.h"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "../render/renderBuffer.hpp"

#include "skySphere.hpp"

RenderBuffer prepareSkySphereBuffers()
{
	struct RenderBuffer render;

	int Band_Power = 4;
	int Band_Points = 16;
	int Band_Mask = Band_Points - 2;
	int Sections_In_Band = (Band_Points / 2) - 1;
	int Total_Points = Sections_In_Band * Band_Points;
	float Section_Arc = M_PI * 2 / Sections_In_Band;
	float Radius = 1.0f;

	float x_angle;
	float y_angle;

	std::vector<GLfloat> vertices;

	for (int i = 0; i < Total_Points; i++)
	{
		// using last bit to alternate,+band number (which band)
		x_angle = (float)(i & 1) + (i >> Band_Power);

		// (i&Band_Mask)>>1 == Local Y value in the band
		// (i>>Band_Power)*((Band_Points/2)-1) == how many bands
		//  have we processed?
		// Remember - we go "right" one value for every 2 points.
		//  i>>bandpower - tells us our band number
		y_angle = (float)((i&Band_Mask) >> 1) + ((i >> Band_Power)*(Sections_In_Band));

		x_angle *= (float)Section_Arc / 2.0f; // remember - 180° x rot not 360
		y_angle *= (float)Section_Arc*-1;

		vertices.push_back(Radius*sin(x_angle)*sin(y_angle));
		vertices.push_back(Radius*cos(x_angle));
		vertices.push_back(Radius*sin(x_angle)*cos(y_angle));
	}

	render.vertices = vertices;

	GLuint VertexVBOID;
	glGenBuffers(1, &VertexVBOID);
	glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GL_FLOAT), &vertices[0], GL_STATIC_DRAW);
	render.vertexBuffer = VertexVBOID;

	return render;
}