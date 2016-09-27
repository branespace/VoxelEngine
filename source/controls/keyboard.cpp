#include "glfw3.h"
#include "glm\glm.hpp"

using namespace glm;

#include "../settings/settings.hpp"
#include "../character/character.hpp"
#include "../physics/collider.hpp"

#include "keyboard.hpp"

int keyDelay = 0;
int initialKeyDelay = 60;

vec3 Keyboard::keyboardMovementInput(GLFWwindow* window, Character &character, float deltaTime)
{
	vec3 movementVector = vec3(0, 0, 0);

	if (Settings::get().getBool("Player.noclip"))
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			movementVector += character.direction * deltaTime * character.speed;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			movementVector -= character.direction * deltaTime * character.speed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			movementVector += character.right * deltaTime * character.speed;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			movementVector -= character.right * deltaTime * character.speed;
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			movementVector += character.up * deltaTime * character.speed;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			movementVector -= character.up * deltaTime * character.speed;
		}
	}
	else
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			movementVector += character.direction * Settings::get().getFloat("Player.CharacterBaseAccel") * character.mass;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			movementVector -= character.direction * Settings::get().getFloat("Player.CharacterBaseAccel") * character.mass;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			movementVector += character.right * Settings::get().getFloat("Player.CharacterBaseAccel") * character.mass;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			movementVector -= character.right * Settings::get().getFloat("Player.CharacterBaseAccel") * character.mass;
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			if (character.canJump)
			{
				character.jump = true;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			//Crouch control
		}
		movementVector.y = 0;
	}
	return movementVector;
}

void Keyboard::getFlagKeyPresses(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		if (keyDelay == 0)
		{
			Settings::get().setBool("Player.noclip", !Settings::get().getBool("Player.noclip"));
			keyDelay = initialKeyDelay;
		}
	}

	if (keyDelay > 0)
	{
		keyDelay--;
	}
}