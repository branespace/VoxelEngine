#pragma once

enum BlockType
{
	BlockType_Default = 0,
	BlockType_Grass,
	BlockType_OakTree,
	BlockType_OakLeaves,
	BlockType_LAST
};

class Block
{
public:
	Block();
	Block(BlockType);
	Block(BlockType type, bool act);

	bool IsActive() const;
	void SetActive(bool active);
	BlockType getBlockType() const;

private:
	bool active;
	BlockType blockType;
};