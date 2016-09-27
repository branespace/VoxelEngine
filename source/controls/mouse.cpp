#include "glfw3.h"
#include "glm\glm.hpp"

#include "../settings/settings.hpp"
#include "../character/character.hpp"

#include "mouse.hpp"

void Mouse::mouseInput(GLFWwindow* window, Character& character, float deltaTime)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	character.horizontalAngle += Settings::get().getFloat("Controls.MouseSpeed") * deltaTime * float(1024 / 2 - xpos);
	character.verticalAngle += Settings::get().getFloat("Controls.MouseSpeed") * deltaTime * float(768 / 2 - ypos);

	Mouse::clampAngles(character);
};

void Mouse::clampAngles(Character& character)
{
	if (character.verticalAngle >= Settings::get().getFloat("Controls.MaximumVerticalAngle"))
	{
		character.verticalAngle = Settings::get().getFloat("Controls.MaximumVerticalAngle");
	}
	if (character.verticalAngle <= Settings::get().getFloat("Controls.MinimumVerticalAngle"))
	{
		character.verticalAngle = Settings::get().getFloat("Controls.MinimumVerticalAngle");
	}
}