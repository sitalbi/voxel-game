#include "chunk.h"

voxl::Chunk::Chunk(int x, int y, int z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}

voxl::Chunk::~Chunk()
{
}

void voxl::Chunk::generate()
{
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++) 
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				BlockType type = BlockType::None;



				if (y <= x && y <= z) {
					if (y < 2) {
						type = BlockType::Stone;
					}
					else if (y == CHUNK_SIZE - 1) {
						type = BlockType::Grass;
					}
					else {
						type = BlockType::Dirt;
					}
				}

				cubes.push_back(new Cube(type, glm::vec3(m_x + x, m_y + y, m_z + z)));
			}
		}
		
	}
}

bool voxl::Chunk::isFaceVisible(glm::vec3 pos)
{
	BlockType neighborType = getBlock(pos);

	return (neighborType == BlockType::None);
}
voxl::BlockType voxl::Chunk::getBlock(glm::vec3 pos)
{
	int x = static_cast<int>(pos.x) - m_x;
	int y = static_cast<int>(pos.y) - m_y;
	int z = static_cast<int>(pos.z) - m_z;

	// Check if the position is within the chunk bounds
	if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE) {
		int index = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
		return cubes[index]->getType();
	}

	return BlockType::None;
}