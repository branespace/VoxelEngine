#include <algorithm>

#include <glfw3.h>
#include "glm/glm.hpp"

#include "../character/character.hpp"
#include "../chunk/chunkManager.hpp"
#include "../settings/settings.hpp"
#include "collider.hpp"

#include "physics.hpp"

float lastTime = glfwGetTime();
float currentTime;

void Physics::processPhysics(Character& character)
{
	//Get time differential
	float currentTime = glfwGetTime();
	float deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	if(!Settings::get().getBool("Player.noclip"))
	{
		//Check to see if we can jump
		if (character.jump && character.canJump && character.velocity.y <= 0.0f)
		{
			auto chunkManager = ChunkManager::getInstance();
			Block testBlock = chunkManager->getBlockFromPosition(glm::vec3(0, -1, 0), character.position);
			if (testBlock.IsActive())
			{				
				character.force.y += character.mass * Settings::get().getFloat("Player.CharacterJumpAccel") * 10.0f;
			}
			character.canJump = false;
		}
		character.jump = false;

		//Add accelerations
		glm::vec3 accel = getAccelerationFromForce(character.force, character.mass);
		addGravityForce(accel);

		//Calculate velocities
		glm::vec3 velocity = getVelocityFromAcceleration(accel, deltaTime);
		character.velocity += velocity;
		character.velocity += addFrictionForce(character.velocity, deltaTime);

		//Clamp velocities
		float maxHorizontalVelocity = Settings::get().getFloat("Player.CharacterMaxHorizontalVelocity");
		float maxVerticalVelocity = Settings::get().getFloat("Player.CharacterMaxVerticalVelocity");
		character.velocity.x = glm::clamp(character.velocity.x, -1 * maxHorizontalVelocity, maxHorizontalVelocity);
		character.velocity.z = glm::clamp(character.velocity.z, -1 * maxHorizontalVelocity, maxHorizontalVelocity);
		character.velocity.y = glm::clamp(character.velocity.y, -1 * maxVerticalVelocity, maxVerticalVelocity / 10.0f);

		//Calculate displacement
		glm::vec3 displacement = getDisplacementFromVelocity(character.velocity, deltaTime);

		//Collide character
		character.position += collideCharacterWithBlocks(character, displacement);
	} 
	else
	{
		character.position += character.force;
	}

	//Reset forces
	character.force = glm::vec3(0, 0, 0);
}

glm::vec3 Physics::getAccelerationFromForce(glm::vec3 force, float mass)
{
	return force / mass;
}

glm::vec3 Physics::getVelocityFromAcceleration(glm::vec3 acceleration, float deltaTime)
{
	return acceleration * deltaTime;
}

glm::vec3 Physics::getDisplacementFromVelocity(glm::vec3 velocity, float deltaTime)
{
	return velocity * deltaTime;
}

glm::vec3 Physics::addFrictionForce(glm::vec3& velocity, float deltaTime)
{
	glm::vec3 deltaV = glm::vec3(0, 0, 0);
	float yComp = velocity.y;
	velocity.y = 0;
	float friction = Settings::get().getFloat("Player.CharacterDragConstant") * deltaTime;
	float speed = abs(glm::length(velocity));
	if (friction < speed)
	{
		deltaV = friction * -1.0f * glm::normalize(velocity);
	}
	else
	{
		deltaV = -1.0f * velocity;
	}
	deltaV.y = yComp;
	return deltaV;
}

void Physics::addGravityForce(glm::vec3& accel)
{
	accel.y -= Settings::get().getFloat("Physics.GravityAccel");
}