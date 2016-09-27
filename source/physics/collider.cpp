#include "glm/glm.hpp"

using namespace glm;

#include "../block/block.hpp"
#include "../character/character.hpp"
#include "../chunk/chunkManager.hpp"

#include "collider.hpp"

vec3 Physics::collideCharacterWithBlocks(Character &character, vec3 preliminaryVector)
{
	auto chunkManager = ChunkManager::getInstance();
	vec3 newVector = preliminaryVector;

	//Get cubes surrounding character object
	Block testBlock;
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			for (int k = -1; k < 2; k++)
			{
				testBlock = chunkManager->getBlockFromPosition(vec3(i, j, k), character.position);
				//If the cube is active and it's not the ground cube or a diagonal cube
				if (testBlock.IsActive() && (vec3(i, j, k) != vec3(0, -1, 0))) {
					//Collide the player sphere with an axis-aligned bounding-box
					if (sphereAABBCollision(character.position, testBlock, vec3(i, j, k)))
					{
						//If it's a cardinal direction cube in the x axis
						if ((i == 1 || i == -1) && k == 0 && j == 0) {
							//And the vector is AWAY from the cube face
							if (getSign(preliminaryVector.x) != i)
							{
								newVector.x = preliminaryVector.x;
							}
							else //Or the vector is TOWARDS the cube face
							{
								newVector.x = 0;
								character.velocity.x = 0;
							}
						}
						else if ((k == 1 || k == -1) && i == 0 && j == 0) //Or in the z axis
						{
							if (getSign(preliminaryVector.z) != k)
							{
								newVector.z = preliminaryVector.z;
							}
							else
							{
								newVector.z = 0;
								character.velocity.z = 0;
							}
						}
						else if (vec3(i, j, k) == vec3(0, 1, 0)) //Or the block directly above
						{
							if (getSign(preliminaryVector.y) == j)
							{
								newVector.y = 0;
								character.velocity.y = 0;
							}
						}
					}
				}
				if (vec3(i, j, k) == vec3(0, -1, 0) && testBlock.IsActive()) //Or down
				{
					if (getSign(preliminaryVector.y) == j)
					{
						newVector.y = 0;
						character.velocity.y = 0;
						character.canJump = true;
					}
				}
			}
		}
	}

	return newVector;
}

int getSign(float val)
{
	return (val > 0) ? 1 : ((val < 0) ? -1 : 0);
}

bool Physics::sphereAABBCollision(vec3 position, Block block, vec3 direction)
{
	glm::ivec3 targetCoord = glm::ivec3(floor(position.x) + direction.x,
		floor(position.y) + direction.y,
		floor(position.z) + direction.z);

	vec3 boxMin = targetCoord;
	vec3 boxMax = targetCoord + 1;

	float minDist = sphereAABBMinDistance(position, boxMin, boxMax);

	return minDist < std::pow(0.25f, 2);
}

float Physics::sphereAABBMinDistance(vec3 position, vec3 boxMin, vec3 boxMax)
{
	float minDist = 0;
	if (position.x < boxMin.x)
	{
		minDist += std::pow(position.x - boxMin.x, 2);
	}
	else if (position.x > boxMax.x)
	{
		minDist += std::pow(position.x - boxMax.x, 2);
	}

	if (position.y < boxMin.y)
	{
		minDist += std::pow(position.y - boxMin.y, 2);
	}
	else if (position.y > boxMax.y)
	{
		minDist += std::pow(position.y - boxMax.y, 2);
	}

	if (position.z < boxMin.z)
	{
		minDist += std::pow(position.z - boxMin.z, 2);
	}
	else if (position.z > boxMax.z)
	{
		minDist += std::pow(position.z - boxMax.z, 2);
	}
	return minDist;
}