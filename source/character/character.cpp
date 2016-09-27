#include "glm/glm.hpp"

#include "../settings/settings.hpp"

#include "character.hpp"

Character::Character()
{
	position = glm::vec3(8, 8, 8);
	force = glm::vec3(0, 0, 0);
	velocity = glm::vec3(0, 0, 0);
	verticalAngle = 0.0f;
	horizontalAngle = 3.14f;
	speed = 3.0f;
	FoV = 45.0f;
	mass = Settings::get().getFloat("Player.CharacterMass");

	jump = false;
	canJump = false;
}