#pragma once

#include <memory>
#include <vector>
#include "chunk.hpp"

class ChunkManager
{
public:
	ChunkManager::ChunkManager();
	void ChunkManager::Setup(glm::vec3 position);
	ChunkManager::~ChunkManager();

	void update(glm::vec3 position);
	std::shared_ptr<Chunk> ChunkManager::getChunkFromWorldCoords(int x, int y, int z);
	std::shared_ptr<Chunk> getChunk(int x, int y, int z);
	Block ChunkManager::getBlockFromPosition(glm::ivec3 direction, glm::vec3 position);

	static const int chunksX = 9;	//Must be odd
	static const int chunksY = chunksX;
	static const int chunksZ = chunksX;
	static const int OFFSET = 0;
	
	int ChunkManager::clampToChunk(int, int&);
	int roundTowardsZero(int val);

	static ChunkManager* ChunkManager::getInstance();

private:
	std::shared_ptr<Chunk> chunks[chunksX][chunksY][chunksZ];
	std::vector<std::shared_ptr<Chunk> > renderList;
	std::vector<glm::vec3> cubeCoords;
	float terrainVerticalScale;
	glm::ivec3 lastChunk = glm::vec3(999, 999, 999);

	void ChunkManager::loadChunks();
	void ChunkManager::unloadChunks(glm::ivec3 chunkShift);
	std::shared_ptr<Chunk> ChunkManager::loadChunk(int x, int y, int z);
	std::shared_ptr<Chunk> ChunkManager::generateChunk(int x, int y, int z);
	bool ChunkManager::generateTree(glm::ivec3 location, std::shared_ptr<Chunk> chunk);
};