#define _USE_MATH_DEFINES
#include <cmath>

#include <glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "../settings/settings.hpp"
#include "global.hpp"

glm::vec3 Lighting::sunVector;
float Lighting::lastTime;
bool Lighting::sunPhase;
glm::vec3 Lighting::sunColor;
glm::vec3 Lighting::moonColor;
float Lighting::sunPower;
float Lighting::moonPower;
float Lighting::dayLength;
glm::vec3 Lighting::Light_InvWavelength;
float Lighting::outerRadius;
float Lighting::innerRadius;
float Lighting::sunBrightness;
float Lighting::rayleighConstant;
float Lighting::rayleigh4PI;
float Lighting::mieConstant;
float Lighting::mie4Pi;
float Lighting::scale;
float Lighting::scaleDepth;
float Lighting::mieAsymmetry;

void Lighting::setupLighting()
{
	Lighting::sunVector = glm::vec3(-1, 0, -3);
	Lighting::lastTime = glfwGetTime();
	Lighting::sunPhase = true;
	Lighting::sunColor = glm::vec3(Settings::get().getFloat("Lighting.SunColorRed"),
		Settings::get().getFloat("Lighting.SunColorGreen"),
		Settings::get().getFloat("Lighting.SunColorBlue"));
	Lighting::moonColor = glm::vec3(Settings::get().getFloat("Lighting.MoonColorRed"),
		Settings::get().getFloat("Lighting.MoonColorGreen"),
		Settings::get().getFloat("Lighting.MoonColorBlue"));
	Lighting::sunPower = Settings::get().getFloat("Lighting.SunPower");
	Lighting::moonPower = Settings::get().getFloat("Lighting.MoonPower");
	Lighting::dayLength = 3.14f / (Settings::get().getFloat("Lighting.DayLengthMinutes") * 60.0f);
	Lighting::Light_InvWavelength = glm::vec3(1.0f / pow(0.650f, 4.0f), 1.0f / pow(0.570f, 4.0f), 1.0f / pow(0.475f, 4.0f));
	Lighting::outerRadius = 1.0f;
	Lighting::innerRadius = 0.95f;;
	Lighting::sunBrightness = 20.0f;
	Lighting::rayleighConstant = 0.0025f;
	Lighting::rayleigh4PI = rayleighConstant * 4 * M_PI;
	Lighting::mieConstant = 0.0010f;
	Lighting::mie4Pi = mieConstant * 4 * M_PI;
	Lighting::scale = 1.0f / (1.0f - 0.95f);
	Lighting::scaleDepth = 0.01f;
	Lighting::mieAsymmetry = 0.0f;
}

glm::vec3 Lighting::getGlobalVector()
{
	float currentTime = glfwGetTime();
	float deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	glm::mat4 rotationMat(1);
	rotationMat = glm::rotate(rotationMat, dayLength * deltaTime, glm::vec3(1, 0, 0));
	Lighting::sunVector = glm::vec3(rotationMat * glm::vec4(sunVector, 0.0));

	if (sunVector.y >= 0)
	{
		sunPhase = !sunPhase;
	}

	return sunVector;
}

glm::vec3 Lighting::getGlobalColor()
{
	return sunColor;
}

float Lighting::getGlobalPower()
{
	return sunPower;
}