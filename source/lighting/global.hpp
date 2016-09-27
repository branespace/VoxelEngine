#pragma once

namespace Lighting
{
	glm::vec3 getGlobalVector();
	glm::vec3 getGlobalColor();
	float getGlobalPower();
	void setupLighting();

	extern float lastTime;
	extern glm::vec3 sunVector;
	extern bool sunPhase;
	extern glm::vec3 sunColor;
	extern float sunPower;
	extern glm::vec3 moonColor;
	extern float moonPower;
	extern float dayLength;
	extern glm::vec3 Light_InvWavelength;
	extern float outerRadius;
	extern float innerRadius;
	extern float sunBrightness;
	extern float rayleighConstant;
	extern float rayleigh4PI;
	extern float mieConstant;
	extern float mie4Pi;
	extern float scale;
	extern float scaleDepth;
	extern float mieAsymmetry;
}