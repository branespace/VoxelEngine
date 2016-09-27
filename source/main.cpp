#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <windows.h>

#include <glew.h>

#include <glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
using namespace glm;

//Structs
#include "render/renderBuffer.hpp"
#include "shader/shader.hpp"

//Sources
#include "character/character.hpp"
#include "chunk/chunk.hpp"
#include "chunk/chunkManager.hpp"
#include "controls/controls.hpp"
#include "geometry/cube.hpp"
#include "geometry/quad.hpp"
#include "geometry/skySphere.hpp"
#include "lighting/global.hpp"
#include "physics/physics.hpp"
#include "shader/loadShader.hpp"
#include "render/text2D.hpp"
#include "settings/settings.hpp"
#include "utility/fpsCounter.hpp"

int clamp2(int val)
{
	while (val < 0)
	{
		val += Chunk::CHUNK_SIZE;
	}
	while (val >= Chunk::CHUNK_SIZE)
	{
		val -= Chunk::CHUNK_SIZE;
	}
	return val;
}

/*
Setup functions
*/
bool initializeGLFW();
GLFWwindow * createWindowContext();
bool initializeGLEW();

/*
Render functions
*/
//Loads all shaders, returns shader array, sets shader uniforms
Shader * prepareShaders();
//Prepares quad geometry for framebuffer render
RenderBuffer prepareFBOQuadBuffer();
//Prepares framebuffer to render depthmap
RenderBuffer prepareFrameBuffersShadow();
//Prepares framebuffer to render final scene for post
RenderBuffer prepareFrameBuffersPost();

/*
Utility functions
*/
void logOpenGLError();
void GLFWErrorCallback(int error, const char* description);

//Default manager
ChunkManager chunkManager;

int main() 
{

	//Load Settings
	std::ifstream ifs;
	char buffer[MAX_PATH];
	//Get exe name
	GetModuleFileName(NULL, buffer, MAX_PATH);
	//Strip to just directory
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	std::string programDirectory = std::string(buffer).substr(0, pos);
	//Open config.ini
	ifs.open(programDirectory + "\\config.ini");
	//Parse to settings
	Settings::get().parseFile(ifs);

	//Initialize GLFW instance and catch failure
	if (!initializeGLFW())
	{
		return -1;
	}

	//Get window pointer and bail if we don't have one
	GLFWwindow* window = createWindowContext();
	if (window == nullptr)
	{
		return -1;
	}

	//Initialize GLEW instance and bail if it does
	if (!initializeGLEW())
	{
		return -1;
	}

	//Set flags
	//Enable face culling
	glEnable(GL_CULL_FACE);
	//Enable z-index rendering
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//Intialize chunks
	//chunkManager = ChunkManager();

	//Prepare framebuffers for depth and final render
	struct RenderBuffer frameBuffers = prepareFrameBuffersPost();
	struct RenderBuffer depthBuffer = prepareFrameBuffersShadow();

	//Prepare geometry buffers
	struct RenderBuffer cubeBuffers = prepareCubeBuffers();
	struct RenderBuffer FBOQuadBuffer = prepareFBOQuadBuffer();
	struct RenderBuffer skySphereBuffer = prepareSkySphereBuffers();

	//Load and prepare shaders
	struct Shader* shaders = prepareShaders();

	//Create buffer for instanced cubes
	GLuint instanceVBO;
	glGenBuffers(1, &instanceVBO);

	//Create buffer for instanced cube types
	GLuint instanceTypeVBO;
	glGenBuffers(1, &instanceTypeVBO);

	//Create character
	Character player;

	initText2D("textures/courier.dds");

	Lighting::setupLighting();

	//Render loop
	do {
		//Compute MVP matricies
		Controls::computeMatricesFromInputs(window, player);

		//Update controls
		Controls::processInputs(window, player);

		//Process positions
		Physics::processPhysics(player);

		//Update all chunks
		chunkManager.update(player.position);

		//Set default background color
		glClearColor(0.53f, 0.81f, 0.93f, 0.0f);

		/*
			RENDER TO SHADOWMAP
		*/
		glBindFramebuffer(GL_FRAMEBUFFER, depthBuffer.frameBuffer);
		glViewport(0, 0, 8096, 8096);
		
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaders[2].shaderProgram);

		vec3 lightInvDir = Lighting::getGlobalVector() * -1.0f;
		if (lightInvDir.y < 0)
		{
			lightInvDir *= -1.0f;
		}

		mat4 depthProjectionMatrix = ortho<float>(-100, 100, -45, 45, -100, 100);
		mat4 depthViewMatrix = lookAt(player.position, player.position - lightInvDir, vec3(0, 1, 0));
		mat4 depthModelMatrix = mat4(1.0);
		mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

		glUniformMatrix4fv(shaders[2].uniform0, 1, GL_FALSE, &depthMVP[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, cubeBuffers.vertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized
			0,                  // stride
			nullptr            // array buffer offset
			);
		for (int i = 0; i < ChunkManager::chunksX; i++)
		{
			for (int j = 0; j < ChunkManager::chunksY; j++)
			{
				for (int k = 0; k < ChunkManager::chunksZ; k++)
				{
					auto chunk = chunkManager.getChunk(i, j, k);
					if (chunk->renderList.size() != 0)
					{
						//Setup vertex buffers for instanced cubes
						glEnableVertexAttribArray(1);
						glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
						glBufferData(GL_ARRAY_BUFFER, chunk->renderList.size() * 3 * sizeof(float), chunk->renderList.data(), GL_STATIC_DRAW);
						glVertexAttribPointer(
							1,                 // attribute
							3,                 // size
							GL_FLOAT,          // type
							GL_FALSE,          // normalized
							0,                 // stride
							(void*)0           // array buffer offset
							);
						glBindBuffer(GL_ARRAY_BUFFER, 0);
						glVertexAttribDivisor(1, 1);

						//Draw Cubes
						glDrawArraysInstanced(GL_TRIANGLES, 0, 36, chunk->renderList.size());

						glVertexAttribDivisor(1, 0);
						glDisableVertexAttribArray(1);
					}
				}
			}
		}
		
		//Reset everything for next draw pass
		glDisableVertexAttribArray(0);
		
		/*
			RENDER TO POST PROCESS BUFFER
		*/

		//Set to render to framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers.frameBuffer);
		glViewport(0, 0, 1024, 768);

		//glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		//Clear Color and Z Buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Calculate and retrieve MVP matrices
		mat4 ProjectionMatrix = Controls::getProjectionMatrix();
		mat4 ViewMatrix = Controls::getViewMatrix();
		mat4 ViewRotationMatrix = lookAt(vec3(0, 0, 0), player.direction, player.up);
		mat4 ModelMatrix = mat4(1.0);
		mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		
		//Get shadow bias matrix
		mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
		);
		mat4 depthBiasMVP = biasMatrix * depthMVP;

		/*
		RENDER SKY DOME
		*/

		//Set sky shader
		glUseProgram(shaders[3].shaderProgram);

		//
		glUniformMatrix4fv(shaders[3].uniform0, 1, GL_FALSE, &inverse(ProjectionMatrix)[0][0]);
		glUniformMatrix4fv(shaders[3].uniform1, 1, GL_FALSE, &inverse(ViewRotationMatrix)[0][0]);
		glUniform3fv(shaders[3].uniform2, 1, &(1.4f * normalize(Lighting::sunVector * -1.0f))[0]);

		//FBO Quad vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, FBOQuadBuffer.vertexBuffer);
		glVertexAttribPointer(
			0,                  //attribute 
			3,                  //size
			GL_FLOAT,           //type
			GL_FALSE,           //normalized
			0,                  //stride
			(void*)0            //array buffer offset
			);

		//Draw the quad
		glDrawArrays(GL_TRIANGLES, 0, 6); //Two tris, 6 vertices total (2 * 3)

		/*
		RENDER CUBES
		*/

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);

		//Use standard shader
		glUseProgram(shaders[0].shaderProgram);
		
		//Set MVP transformation uniform
		glUniformMatrix4fv(shaders[0].uniform0, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(shaders[0].uniform2, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(shaders[0].uniform1, 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(shaders[0].uniform3, 1, GL_FALSE, &depthBiasMVP[0][0]);
		glUniform3fv(shaders[0].uniform5, 1, &(Lighting::sunVector * -1.0f)[0]);
		glUniform3f(shaders[0].uniform8, Lighting::sunColor.x, Lighting::sunColor.y, Lighting::sunColor.z);
		glUniform1f(shaders[0].uniform7, Lighting::getGlobalPower());
		glUniform1i(shaders[0].uniform9, BlockType_LAST);
		glUniform3f(shaders[0].uniform10, player.position.x, player.position.y + 1, player.position.z);
		glUniform1f(shaders[0].uniform11, Lighting::moonPower);
		glUniform3fv(shaders[0].uniform12, 1, &Lighting::moonColor[0]);
	
		//Assign shadowmap texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthBuffer.depthTexture);
		glUniform1i(shaders[0].uniform4, 0);

		//Assign block texture
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, cubeBuffers.texture);
		glUniform1i(shaders[0].uniform6, 2);

		//First attrib for shaders: vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, cubeBuffers.vertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized
			0,                  // stride
			(void*)0            // array buffer offset
			);
		//Second attrib for shaders: colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, cubeBuffers.colorBuffer);
		glVertexAttribPointer(
			1,                 // attribute 
			3,                 // size
			GL_FLOAT,          // type
			GL_FALSE,          // normalized
			0,                 // stride
			(void*)0           // array buffer offset
			);
		//Fourth attribute: offsets array for cube instances
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, cubeBuffers.normalBuffer);
		glVertexAttribPointer(
			3,                 // attribute
			3,                 // size
			GL_FLOAT,          // type
			GL_FALSE,          // normalized
			0,                 // stride
			(void*)0           // array buffer offset
			);
		//Fifth attribute: UV coordinates
		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, cubeBuffers.uvBuffer);
		glVertexAttribPointer(
			4,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
			);
		for (int i = 0; i < ChunkManager::chunksX; i++)
		{
			for (int j = 0; j < ChunkManager::chunksY; j++)
			{
				for (int k = 0; k < ChunkManager::chunksZ; k++)
				{
					auto chunk = chunkManager.getChunk(i, j, k);
					if (chunk->renderList.size() != 0)
					{
						glEnableVertexAttribArray(2);
						glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
						glBufferData(GL_ARRAY_BUFFER, chunk->renderList.size() * sizeof(vec3), chunk->renderList.data(), GL_STATIC_DRAW);
						glVertexAttribPointer(
							2,                 // attribute
							3,                 // size
							GL_FLOAT,          // type
							GL_FALSE,          // normalized
							0,                 // stride
							(void*)0           // array buffer offset
							);
						glBindBuffer(GL_ARRAY_BUFFER, 0);
						glVertexAttribDivisor(2, 1);

						glEnableVertexAttribArray(5);
						glBindBuffer(GL_ARRAY_BUFFER, instanceTypeVBO);
						glBufferData(GL_ARRAY_BUFFER, chunk->renderBlockList.size() * sizeof(float), chunk->renderBlockList.data(), GL_STATIC_DRAW);
						glVertexAttribPointer(
							5,                 // attribute
							1,                 // size
							GL_FLOAT,		   // type
							GL_FALSE,		   // normalizeed
							0,                 // stride
							(void*)0           // array buffer offset
							);
						glBindBuffer(GL_ARRAY_BUFFER, 0);
						glVertexAttribDivisor(5, 1);

						//Draw Cubes
						glDrawArraysInstanced(GL_TRIANGLES, 0, 36, chunk->renderList.size());
						glDisableVertexAttribArray(2);
						glVertexAttribDivisor(2, 0);
						glDisableVertexAttribArray(5);
						glVertexAttribDivisor(5, 0);
					}
				}
			}
		}
		//Reset everything for next draw pass
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);
		glDisable(GL_BLEND);

		/*
			RENDER TO SCREEN
		*/

		//Render to screen instead of framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//Render on the entire buffer
		glViewport(0, 0, 1024, 768);

		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use framebuffer shader
		glUseProgram(shaders[1].shaderProgram);

		//Set texture to Texture Unit 2
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, frameBuffers.frameBuffer);
		//Set texture to link to texture unit 2
		glUniform1i(shaders[1].uniform0, 0);

		//FBO Quad vertices
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, FBOQuadBuffer.vertexBuffer);
		glVertexAttribPointer(
			3,                  //attribute 
			3,                  //size
			GL_FLOAT,           //type
			GL_FALSE,           //normalized
			0,                  //stride
			(void*)0            //array buffer offset
			);

		//Disable z-indexing to allow overlay
		glDisable(GL_DEPTH_TEST);

		//Draw the quad
		glDrawArrays(GL_TRIANGLES, 0, 6); //Two tris, 6 vertices total (2 * 3)

		//Clean up for next pass
		glDisableVertexAttribArray(3);

		/*
		RENDER TEXT
		*/
		char text[256];
		sprintf(text, "pos:(%.2f, %.2f, %.2f)", player.position.x, player.position.y, player.position.z);
		printText2D(text, 10, 768 - (10 + 30), 30);

		glm::ivec3 targetCoord = glm::ivec3(floor(player.position.x),
			floor(player.position.y),
			floor(player.position.z));

		glm::ivec3 targetChunk = glm::ivec3(floor(targetCoord.x / (Chunk::CHUNK_SIZE)),
			floor(targetCoord.y / (Chunk::CHUNK_SIZE)),
			floor(targetCoord.z / (Chunk::CHUNK_SIZE)));

		targetChunk.x -= (player.position.x < 0 ? 1 : 0);
		targetChunk.y -= (player.position.y < 0 ? 1 : 0);
		targetChunk.z -= (player.position.z < 0 ? 1 : 0);

		//targetCoord /= 2;

		targetCoord.x = clamp2(targetCoord.x);
		targetCoord.y = clamp2(targetCoord.y);
		targetCoord.z = clamp2(targetCoord.z);

		sprintf(text, "blp:(%d, %d, %d)", targetCoord.x, targetCoord.y, targetCoord.z);
		printText2D(text, 10, 768 - (10 + 30) * 2, 30);

		sprintf(text, "tch:(%d, %d, %d)", targetChunk.x, targetChunk.y, targetChunk.z);
		printText2D(text, 10, 768 - (10 + 30) * 3, 30);

		sprintf(text, "vel:(%.2f, %.2f, %.2f)", player.velocity.x, player.velocity.y, player.velocity.z);
		printText2D(text, 10, 768 - (10 + 30) * 4, 30);

		logOpenGLError();

		//Display buffer
		glfwSwapBuffers(window);

		//Re-enable z-indexing after displaying overlay
		glEnable(GL_DEPTH_TEST);

		//Accept control input
		glfwWaitEvents();

		//FPS Counter
		logFPS();
	} 
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	cleanupText2D();

	glfwTerminate();

	return 0;
}

void GLFWErrorCallback(int error, const char* description)
{
	fputs(description, stderr);
}

bool initializeGLFW()
{
	//Try to initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return false;
	}

	fprintf(stderr, "GLFW loaded\n");

	//Register callback for GLFW errors only
	glfwSetErrorCallback(GLFWErrorCallback);

	//All good!
	return true;
}

GLFWwindow* createWindowContext()
{
	//Set window states
	//Enable multisampling, 4 samples
	glfwWindowHint(GLFW_SAMPLES, 4);
	//Set OpenGL Version to 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Enable forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//Set OpenGL profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Window pointer and creation
	GLFWwindow* window;
	window = glfwCreateWindow(1024, 768, "VoxelEngine", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window\n");
		glfwTerminate();
		return NULL;
	}

	//Bind OpenGL Context to window
	glfwMakeContextCurrent(window);

	//Set key mode on window
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	//Hide mouse pointer
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	//Good to go!
	return window;
}

bool initializeGLEW()
{
	//Required for compatibility functions
	glewExperimental = true;
	//Initialize GLEW
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return false;
	}
	return true;
}

Shader* prepareShaders()
{
	//Shader array
	struct Shader* shaders = new Shader[4];

	//Load vertex and fragment shaders for main render
	shaders[0].shaderProgram = LoadShaders("shaders/vertexshader.glsl", "shaders/fragmentshader.glsl");
	shaders[0].uniform0 = glGetUniformLocation(shaders[0].shaderProgram, "MVP");	//matrixid
	shaders[0].uniform1 = glGetUniformLocation(shaders[0].shaderProgram, "V");		//viewmatrixid
	shaders[0].uniform2 = glGetUniformLocation(shaders[0].shaderProgram, "M");		//modelmatrixid
	shaders[0].uniform3 = glGetUniformLocation(shaders[0].shaderProgram, "DepthBiasMVP");	//depthbiasmvpid
	shaders[0].uniform4 = glGetUniformLocation(shaders[0].shaderProgram, "shadowMap");		//shadowmapid
	shaders[0].uniform5 = glGetUniformLocation(shaders[0].shaderProgram, "LightInvDirection_worldspace"); //Lightinvdirection
	shaders[0].uniform6 = glGetUniformLocation(shaders[0].shaderProgram, "textureSampler");	//Cube Textures
	shaders[0].uniform7 = glGetUniformLocation(shaders[0].shaderProgram, "SunPower");
	shaders[0].uniform8 = glGetUniformLocation(shaders[0].shaderProgram, "SunColor");
	shaders[0].uniform9 = glGetUniformLocation(shaders[0].shaderProgram, "TypeCount");
	shaders[0].uniform10 = glGetUniformLocation(shaders[0].shaderProgram, "Camera_worldspace");
	shaders[0].uniform11 = glGetUniformLocation(shaders[0].shaderProgram, "MoonPower");
	shaders[0].uniform12 = glGetUniformLocation(shaders[0].shaderProgram, "MoonColor");
	//Shaders for framebuffer
	shaders[1].shaderProgram = LoadShaders("shaders/Passthrough.vertexshader.glsl", "shaders/SimpleTexture.fragmentshader.glsl");
	shaders[1].uniform0 = glGetUniformLocation(shaders[1].shaderProgram, "renderedTexture");
	//Depth shaders
	shaders[2].shaderProgram = LoadShaders("shaders/DepthRTT.vertexshader.glsl", "shaders/DepthRTT.fragmentshader.glsl");
	shaders[2].uniform0 = glGetUniformLocation(shaders[2].shaderProgram, "depthMVP");
	//Sky Shader
	shaders[3].shaderProgram = LoadShaders("shaders/SkyVertexShader.glsl", "shaders/SkyFragmentShader.glsl");
	shaders[3].uniform0 = glGetUniformLocation(shaders[3].shaderProgram, "inv_proj");
	shaders[3].uniform1 = glGetUniformLocation(shaders[3].shaderProgram, "inv_view_rot");
	shaders[3].uniform2 = glGetUniformLocation(shaders[3].shaderProgram, "lightdir");

	return shaders;
}

RenderBuffer prepareFrameBuffersShadow()
{
	struct RenderBuffer frameBuffer;
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	GLuint FramebufferName = 0;
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	frameBuffer.frameBuffer = FramebufferName;

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	GLuint depthTexture;
	glGenTextures(1, &depthTexture);
	frameBuffer.depthTexture = depthTexture;

	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 8096, 8096, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

	// No color output in the bound framebuffer, only depth.
	glDrawBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "Framebuffer invalid");
		exit(-1);
	}
	return frameBuffer;
}

RenderBuffer prepareFrameBuffersPost()
{
	struct RenderBuffer frameBuffer;
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	GLuint FramebufferName;
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	frameBuffer.frameBuffer = FramebufferName;

	// The texture we're going to render to
	GLuint renderedTexture;
	glGenTextures(1, &renderedTexture);
	frameBuffer.renderedTexture = renderedTexture;

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	// Give an empty image to OpenGL ( the last "0" means "empty" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 768, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Poor filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// The depth buffer
	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
	frameBuffer.depthBuffer = depthrenderbuffer;

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

								   // Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "Framebuffer invalid");
		exit(-1);
	}

	return frameBuffer;
}

void logOpenGLError() {
	GLenum err(glGetError());

	while (err != GL_NO_ERROR) {
		std::string error;

		switch (err) {
		case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
		case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
		case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
		case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
		}

		std::cerr << "GL_" << error.c_str() << std::endl;
		err = glGetError();
	}
}