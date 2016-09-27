#include <vector>

#include <glew.h>
#include <glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

#include "../block/block.hpp"
#include "../character/character.hpp"
#include "../chunk/chunk.hpp"
#include "../chunk/chunkManager.hpp"
#include "keyboard.hpp"
#include "mouse.hpp"

#include "controls.hpp"

using namespace glm;

double lastTime = glfwGetTime();;
mat4 ProjectionMatrix;
mat4 ViewMatrix;

void Controls::processInputs(GLFWwindow* window, Character& character)
{
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);
	lastTime = currentTime;

	vec3 inputMovement;

	Keyboard::getFlagKeyPresses(window);
	inputMovement = Keyboard::keyboardMovementInput(window, character, deltaTime);
	Mouse::mouseInput(window, character, deltaTime);

	character.force = inputMovement;
}

void Controls::computeMatricesFromInputs(GLFWwindow* window, Character& character)
{
	float initialFoV = 45.0f;

	character.direction = vec3(
		cos(character.verticalAngle) * sin(character.horizontalAngle),
		sin(character.verticalAngle),
		cos(character.verticalAngle) * cos(character.horizontalAngle)
	);

	character.right = vec3(
		sin(character.horizontalAngle - 3.14f/2.0f),
		0,
		cos(character.horizontalAngle - 3.14f/2.0f)
	);

	character.up = cross(character.right, character.direction);

	vec3 cameraPos = vec3(character.position.x, character.position.y + 1, character.position.z);

	//Prep camera angles
	ProjectionMatrix = perspective(character.FoV, 4.0f / 3.0f, 0.1f, 200.0f);
	ViewMatrix = lookAt(cameraPos, cameraPos + character.direction, character.up);
}

//Getters for Projection and View Matrices
mat4 Controls::getProjectionMatrix()
{
	return ProjectionMatrix;
}

mat4 Controls::getViewMatrix()
{
	return ViewMatrix;
}