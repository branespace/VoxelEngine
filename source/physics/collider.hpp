#pragma once

#include "glm/glm.hpp"
#include "../block/block.hpp"

namespace Physics
{
	bool sphereAABBCollision(glm::vec3 position, Block block, glm::vec3 direction);
	float sphereAABBMinDistance(glm::vec3 position, glm::vec3 boxMin, glm::vec3 boxMax);

	glm::vec3 collideCharacterWithBlocks(Character &character, glm::vec3 preliminaryVector);
}
int getSign(float val);