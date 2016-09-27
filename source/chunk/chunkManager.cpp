#include <vector>
#include <string>
#include <math.h>
#include <memory>
#include <random>

#include "glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "noise.h"
#include "noiseutils.h"

#include "../block/block.hpp"
#include "chunk.hpp"
#include "chunkManager.hpp"

//Perlin coherent noise generator
module::Perlin perlin;
//Heightmap for generating terrain
utils::NoiseMap heightMap;
//Assembler for heightmap
utils::NoiseMapBuilderPlane heightMapBuilder;

ChunkManager* instance;
std::random_device rd;
std::mt19937 gen(rd());

ChunkManager::ChunkManager()
{
	ChunkManager::Setup(glm::vec3(8, 8, 8));	//Starting position
	instance = this;							//Store reference
}

void ChunkManager::Setup(glm::vec3 position)
{
	//Define vertical scaling
	terrainVerticalScale = 0.1f;

	//Generate all chunks and set empty
	for (int i = 0; i < chunksX; i++)
	{
		for (int j = 0; j < chunksY; j++)
		{
			for (int k = 0; k < chunksZ; k++)
			{
				auto chunk = std::make_shared<Chunk>();
				chunks[i][j][k] = chunk;
				chunks[i][j][k]->state = ChunkState_Empty;
				chunks[i][j][k]->chunkX = i - chunksX / 2;
				chunks[i][j][k]->chunkY = j - chunksZ / 2;
				chunks[i][j][k]->chunkZ = k - chunksY / 2;
			}
		}
	}

	//Initialize heightmap
	heightMapBuilder.SetSourceModule(perlin);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	heightMapBuilder.SetDestSize(16, 16);

	glm::ivec3 targetCoord = glm::ivec3(floor(position.x),
		floor(position.y),
		floor(position.z));

	glm::ivec3 targetChunk = glm::ivec3(floor(targetCoord.x / (Chunk::CHUNK_SIZE)),
		floor(targetCoord.y / (Chunk::CHUNK_SIZE)),
		floor(targetCoord.z / (Chunk::CHUNK_SIZE)));

	targetChunk.x -= (position.x < 0 ? 1 : 0);
	targetChunk.y -= (position.y < 0 ? 1 : 0);
	targetChunk.z -= (position.z < 0 ? 1 : 0);

	loadChunks();
	lastChunk = targetChunk;
}

ChunkManager::~ChunkManager()
{

}

ChunkManager* ChunkManager::getInstance()
{
	return instance;
}

void ChunkManager::update(glm::vec3 position)
{
	glm::ivec3 targetCoord = glm::ivec3(floor(position.x),
		floor(position.y),
		floor(position.z));

	glm::ivec3 targetChunk = glm::ivec3(floor(targetCoord.x / (Chunk::CHUNK_SIZE)),
		floor(targetCoord.y / (Chunk::CHUNK_SIZE)),
		floor(targetCoord.z / (Chunk::CHUNK_SIZE)));

	targetChunk.x -= (position.x < 0 ? 1 : 0);
	targetChunk.y -= (position.y < 0 ? 1 : 0);
	targetChunk.z -= (position.z < 0 ? 1 : 0);

	//Do we need to update loaded chunks?
	if (targetChunk != lastChunk)
	{
		unloadChunks(targetChunk - lastChunk);
		loadChunks();
		lastChunk = targetChunk;
	}
}

//Load all chunks set to empty
void ChunkManager::loadChunks()
{
	for (int i = 0; i < chunksX; i++)
	{
		for (int j = 0; j < chunksY; j++)
		{
			for (int k = 0; k < chunksZ; k++)
			{
				if (chunks[i][j][k]->state == ChunkState_Empty)
				{
					chunks[i][j][k] = loadChunk(chunks[i][j][k]->chunkX, chunks[i][j][k]->chunkY, chunks[i][j][k]->chunkZ);
				}
			}
		}
	}
}

std::shared_ptr<Chunk> ChunkManager::loadChunk(int x, int y, int z)
{
	auto chunk = generateChunk(x, y, z);
	return chunk;
}

void ChunkManager::unloadChunks(glm::ivec3 chunkShift)
{
	if (chunkShift.x == 1)
	{
		//Swap down chunks by X
		for (int i = 0; i <= chunksX - 2; i++)
		{
			for (int j = 0; j < chunksY; j++)
			{
				for (int k = 0; k < chunksZ; k++)
				{
					//delete chunks[i][j][k];
					chunks[i][j][k] = chunks[i + 1][j][k];
				}
			}
		}
		//Replace distant chunk
		for (int j = 0; j < chunksY; j++)
		{
			for (int k = 0; k < chunksZ; k++)
			{
				int x = chunks[chunksX - 1][j][k]->chunkX;
				int y = chunks[chunksX - 1][j][k]->chunkY;
				int z = chunks[chunksX - 1][j][k]->chunkZ;
				auto chunk = std::make_shared<Chunk>();
				chunks[chunksX - 1][j][k] = chunk;
				chunk->chunkX = x + 1;
				chunk->chunkY = y;
				chunk->chunkZ = z;
				chunk->state = ChunkState_Empty;
			}
		}
	}
	if (chunkShift.x == -1)
	{
		//Swap chunks up by X
		for (int i = chunksX - 2; i >= 0; i--)
		{
			for (int j = 0; j < chunksY; j++)
			{
				for (int k = 0; k < chunksZ; k++)
				{
					//delete chunks[i + 1][j][k];
					chunks[i + 1][j][k] = chunks[i][j][k];
				}
			}
		}
		//Replace distant chunk
		for (int j = 0; j < chunksY; j++)
		{
			for (int k = 0; k < chunksZ; k++)
			{
				int x = chunks[0][j][k]->chunkX;
				int y = chunks[0][j][k]->chunkY;
				int z = chunks[0][j][k]->chunkZ;
				auto chunk = std::make_shared<Chunk>();
				chunks[0][j][k] = chunk;
				chunk->chunkX = x - 1;
				chunk->chunkY = y;
				chunk->chunkZ = z;
				chunk->state = ChunkState_Empty;
			}
		}
	}
	if (chunkShift.z == 1)
	{
		//Swap chunks up by z
		for (int i = 0; i < chunksX; i++)
		{
			for (int j = 0; j < chunksY; j++)
			{
				for (int k = 0; k <= chunksZ - 2; k++)
				{
					//delete chunks[i][j][k];
					chunks[i][j][k] = chunks[i][j][k + 1];
				}
			}
		}
		//Replace distant chunk
		for (int i = 0; i < chunksX; i++)
		{
			for (int j = 0; j < chunksY; j++)
			{
				int x = chunks[i][j][chunksZ - 1]->chunkX;
				int y = chunks[i][j][chunksZ - 1]->chunkY;
				int z = chunks[i][j][chunksZ - 1]->chunkZ;
				auto chunk = std::make_shared<Chunk>();
				chunks[i][j][chunksZ - 1] = chunk;
				chunk->chunkX = x;
				chunk->chunkY = y;
				chunk->chunkZ = z + 1;
				chunk->state = ChunkState_Empty;
				;
			}
		}
	}
	if (chunkShift.z == -1)
	{
		//Swap chunks down by z
		for (int i = 0; i < chunksX; i++)
		{
			for (int j = 0; j < chunksY; j++)
			{
				for (int k = chunksZ - 2; k >= 0; k--)
				{
					//delete chunks[i][j][k + 1];
					chunks[i][j][k + 1] = chunks[i][j][k];
				}
			}
		}
		//Remove far chunk
		for (int i = 0; i < chunksX; i++)
		{
			for (int j = 0; j < chunksY; j++)
			{
				int x = chunks[i][j][0]->chunkX;
				int y = chunks[i][j][0]->chunkY;
				int z = chunks[i][j][0]->chunkZ;
				auto chunk = std::make_shared<Chunk>();
				chunks[i][j][0] = chunk;
				chunk->chunkX = x;
				chunk->chunkY = y;
				chunk->chunkZ = z - 1;
				chunk->state = ChunkState_Empty;
			}
		}
	}
	if (chunkShift.y == 1)
	{
		//Swap chunks up by z
		for (int i = 0; i < chunksX; i++)
		{
			for (int k = 0; k < chunksZ; k++)
			{
				for (int j = 0; j <= chunksY - 2; j++)
				{
					chunks[i][j][k] = chunks[i][j + 1][k];
				}
			}
		}
		//Replace distant chunk
		for (int i = 0; i < chunksX; i++)
		{
			for (int k = 0; k < chunksZ; k++)
			{
				int x = chunks[i][chunksY - 1][k]->chunkX;
				int y = chunks[i][chunksY - 1][k]->chunkY;
				int z = chunks[i][chunksY - 1][k]->chunkZ;
				auto chunk = std::make_shared<Chunk>();
				chunks[i][chunksY - 1][k] = chunk;
				chunk->chunkX = x;
				chunk->chunkY = y + 1;
				chunk->chunkZ = z;
				chunk->state = ChunkState_Empty;
			}
		}
	}
	if (chunkShift.y == -1)
	{
		//Swap chunks down by z
		for (int i = 0; i < chunksX; i++)
		{
			for (int k = 0; k < chunksZ; k++)
			{
				for (int j = chunksY - 2; j >= 0; j--)
				{
					chunks[i][j + 1][k] = chunks[i][j][k];
				}
			}
		}
		//Remove far chunk
		for (int i = 0; i < chunksX; i++)
		{
			for (int k = 0; k < chunksZ; k++)
			{
				int x = chunks[i][0][k]->chunkX;
				int y = chunks[i][0][k]->chunkY;
				int z = chunks[i][0][k]->chunkZ;
				auto chunk = std::make_shared<Chunk>();
				chunks[i][0][k] = chunk;
				chunk->chunkX = x;
				chunk->chunkY = y - 1;
				chunk->chunkZ = z;
				chunk->state = ChunkState_Empty;
			}
		}
	}
}

std::shared_ptr<Chunk> ChunkManager::generateChunk(int x, int y, int z)
{
	auto chunk = std::make_shared<Chunk>();
	heightMapBuilder.SetBounds(x, x + 1, z, z + 1);
	heightMapBuilder.Build();

	chunk->chunkX = x;
	chunk->chunkY = y;
	chunk->chunkZ = z;

	for (int i = 0; i < Chunk::CHUNK_SIZE; i++)
	{
		for (int k = 0; k < Chunk::CHUNK_SIZE; k++)
		{
			float height = heightMap.GetValue(i, k);
			height *= terrainVerticalScale * Chunk::CHUNK_SIZE;
			height += Chunk::CHUNK_SIZE / 4;
			for (int j = 0; j < Chunk::CHUNK_SIZE; j++)
			{
				if ((j + y * Chunk::CHUNK_SIZE) <= (int)height)
				{
					Block block = Block();
					block.SetActive(true);
					chunk->setBlock(i, j, k, block);
					if ((j + y * Chunk::CHUNK_SIZE) == (int)height)
					{
						if (!generateTree(glm::ivec3(i, j, k), chunk))
						{
							chunk->renderList.push_back(glm::vec3((i)+(x)* (Chunk::CHUNK_SIZE),
								(j)+(y)* (Chunk::CHUNK_SIZE),
								(k)+(z)* (Chunk::CHUNK_SIZE)));
							chunk->renderBlockList.push_back(BlockType_Default);
						}
					}
				}
			}
		}
	}
	chunk->state = ChunkState_Loaded;
	return chunk;
}

bool ChunkManager::generateTree(glm::ivec3 location, std::shared_ptr<Chunk> chunk)
{
	if (location.x >= 3 && location.x <= Chunk::CHUNK_SIZE - 4 &&
		location.z >= 3 && location.z <= Chunk::CHUNK_SIZE - 4)
	{
		if (gen() % (Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE / 2) == 0)
		{
			int height = gen() % 4 + 4;
			for (int j = 0; j <= height; j++)
			{
				Block block = Block();
				block.SetActive(true);
				chunk->setBlock(location.x, location.y + j, location.z, block);
				chunk->renderList.push_back(glm::vec3((location.x)+(chunk->chunkX)* (Chunk::CHUNK_SIZE),
					(location.y + j)+(chunk->chunkY)* (Chunk::CHUNK_SIZE),
					(location.z)+(chunk->chunkZ)* (Chunk::CHUNK_SIZE)));
				chunk->renderBlockList.push_back(BlockType_OakTree);
			}
			for (int j = -1; j <= 1; j += 2)
			{
				for (int i = -2; i <= 2; i++)
				{
					for (int k = -2; k <= 2; k++)
					{
						Block block = Block();
						block.SetActive(true);
						chunk->setBlock(location.x + i, location.y + height + j, location.z+ k, block);
						chunk->renderList.push_back(glm::vec3((location.x + i) + (chunk->chunkX)* (Chunk::CHUNK_SIZE),
							(location.y + height + j) + (chunk->chunkY)* (Chunk::CHUNK_SIZE),
							(location.z + k) + (chunk->chunkZ)* (Chunk::CHUNK_SIZE)));
						chunk->renderBlockList.push_back(BlockType_OakLeaves);
					}
				}
			}
			for (int j = -2; j <= 2; j += 4)
			{
				for (int i = -1; i <= 1; i++)
				{
					for (int k = -1; k <= 1; k++)
					{
						Block block = Block();
						block.SetActive(true);
						chunk->setBlock(location.x + i, location.y + height + j, location.z + k, block);
						chunk->renderList.push_back(glm::vec3((location.x + i) + (chunk->chunkX)* (Chunk::CHUNK_SIZE),
							(location.y + height + j) + (chunk->chunkY)* (Chunk::CHUNK_SIZE),
							(location.z + k) + (chunk->chunkZ)* (Chunk::CHUNK_SIZE)));
						chunk->renderBlockList.push_back(BlockType_OakLeaves);
					}
				}
			}
			for (int j = 0; j < 1; j ++)
			{
				for (int i = -3; i <= 3; i++)
				{
					for (int k = -3; k <= 3; k++)
					{
						Block block = Block();
						block.SetActive(true);
						chunk->setBlock(location.x + i, location.y + height + j, location.z + k, block);
						chunk->renderList.push_back(glm::vec3((location.x + i) + (chunk->chunkX)* (Chunk::CHUNK_SIZE),
							(location.y + height + j) + (chunk->chunkY)* (Chunk::CHUNK_SIZE),
							(location.z + k) + (chunk->chunkZ)* (Chunk::CHUNK_SIZE)));
						chunk->renderBlockList.push_back(BlockType_OakLeaves);
					}
				}
			}
			return true;
		}
	}
	return false;
}

Block ChunkManager::getBlockFromPosition(glm::ivec3 direction, glm::vec3 position)
{
	//Get blockspace position for target position
	glm::ivec3 targetCoord = glm::ivec3(floor(position.x) + direction.x,
		floor(position.y) + direction.y,
		floor(position.z) + direction.z);

	//Get target chunk from blockspace position
	glm::ivec3 targetChunk = glm::ivec3(floor(targetCoord.x / (Chunk::CHUNK_SIZE)),
		floor(targetCoord.y / (Chunk::CHUNK_SIZE)),
		floor(targetCoord.z / (Chunk::CHUNK_SIZE)));

	//Correct for negative values of chunk coordinates
	targetChunk.x -= (targetCoord.x < 0 ? 1 : 0);
	targetChunk.y -= (targetCoord.y < 0 ? 1 : 0);
	targetChunk.z -= (targetCoord.z < 0 ? 1 : 0);

	//Reduce blockspace coordinates to location inside chunk
	targetCoord.x = clampToChunk(targetCoord.x, targetChunk.x);
	targetCoord.y = clampToChunk(targetCoord.y, targetChunk.y);
	targetCoord.z = clampToChunk(targetCoord.z, targetChunk.z);

	//Get target chunk and blockspace block and return
	return getChunkFromWorldCoords(targetChunk.x, targetChunk.y, targetChunk.z)->getBlock(targetCoord.x, targetCoord.y, targetCoord.z);
}

int ChunkManager::clampToChunk(int val, int& chunk)
{
	while (val < 0)
	{
		val += Chunk::CHUNK_SIZE;
	}
	while (val >= Chunk::CHUNK_SIZE)
	{
		val -= Chunk::CHUNK_SIZE;
	}
	return val;
}

int ChunkManager::roundTowardsZero(int val)
{
	return val < 0 ? ceil(val) : floor(val);
}

std::shared_ptr<Chunk> ChunkManager::getChunkFromWorldCoords(int x, int y, int z)
{
	for (int i = 0; i < chunksX; i++)
	{
		for (int j = 0; j < chunksY; j++)
		{
			for (int k = 0; k < chunksZ; k++)
			{
				if (chunks[i][j][k]->chunkX == x &&
					chunks[i][j][k]->chunkY == y &&
					chunks[i][j][k]->chunkZ == z)
				{
					return chunks[i][j][k];
				}
			}
		}
	}
	return NULL;
}

std::shared_ptr<Chunk> ChunkManager::getChunk(int x, int y, int z)
{
	return chunks[x][y][z];
}