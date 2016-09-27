#pragma once

namespace Mouse
{
	void mouseInput(GLFWwindow* window, Character& character, float deltaTime);
	void clampAngles(Character& character);
}