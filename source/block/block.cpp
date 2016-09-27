#include "block.hpp"

Block::Block()
{
	blockType = BlockType_Default;
	active = false;
}

Block::Block(BlockType type)
{
	blockType = type;
	active = false;
}

Block::Block(BlockType type, bool act)
{
	blockType = type;
	active = act;
}

bool Block::IsActive() const
{
	return active;
}

void Block::SetActive(bool act)
{
	active = act;
}

BlockType Block::getBlockType() const
{
	return blockType;
}
