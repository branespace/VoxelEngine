#pragma once

namespace Keyboard
{
	glm::vec3 keyboardMovementInput(GLFWwindow*, Character&, float);
	void getFlagKeyPresses(GLFWwindow* window);
}