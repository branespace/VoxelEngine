#pragma once

namespace Physics
{
	void processPhysics(Character& character);

	glm::vec3 getAccelerationFromForce(glm::vec3 force, float mass);
	glm::vec3 getVelocityFromAcceleration(glm::vec3 acceleration, float deltaTime);
	glm::vec3 getDisplacementFromVelocity(glm::vec3 velocity, float deltaTime);
	glm::vec3 addFrictionForce(glm::vec3& velocity, float deltaTime);
	void addGravityForce(glm::vec3& accel);
}