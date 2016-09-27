#pragma once

namespace Controls
{
	void processInputs(GLFWwindow* window, Character& character);
	void computeMatricesFromInputs(GLFWwindow*, Character&);
	glm::mat4 getProjectionMatrix();
	glm::mat4 getViewMatrix();
}