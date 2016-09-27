#pragma once

#include "../block/block.hpp"

enum ChunkState
{
	ChunkState_Empty = 0,
	ChunkState_Loaded = 1,
	ChunkState_Unload = 2
};

class Chunk
{
public:
	Chunk();
	~Chunk();

	//void Update(float dt);
	//void Render();
	void Chunk::setBlock(int x, int y, int z, Block block);
	Block& Chunk::getBlock(int x, int y, int z);

	std::vector<glm::vec3> renderList;
	std::vector<float> renderBlockList;
	static const int CHUNK_SIZE = 16;
	int chunkX;
	int chunkY;
	int chunkZ;
	ChunkState state;

private:
	Block*** blocks;
};