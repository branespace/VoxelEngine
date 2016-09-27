#include <vector>

#include "glm/glm.hpp"

#include "../block/block.hpp"
#include "chunk.hpp"

Chunk::Chunk()
{
	// Create the blocks
	blocks = new Block**[CHUNK_SIZE];
	for (int i = 0; i < CHUNK_SIZE; i++)
	{
		blocks[i] = new Block*[CHUNK_SIZE];

		for (int j = 0; j < CHUNK_SIZE; j++)
		{
			blocks[i][j] = new Block[CHUNK_SIZE];
		}
	}
}

Chunk::~Chunk()
{
	for (int i = 0; i < CHUNK_SIZE; i++)
	{
		for (int j = 0; j < CHUNK_SIZE; j++)
		{
			delete[] blocks[i][j];
		}
		delete[] blocks[i];
	}
	delete[] blocks;
}

void Chunk::setBlock(int x, int y, int z, Block block)
{
	blocks[x][y][z] = block;
}

Block& Chunk::getBlock(int x, int y, int z)
{
	return blocks[x][y][z];
}