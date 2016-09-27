#pragma once

class Character
{
public:
	Character::Character();

	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 force;
	glm::vec3 velocity;

	float verticalAngle;
	float horizontalAngle;
	float speed;
	float FoV;
	float mass;

	bool jump;
	bool canJump;
};