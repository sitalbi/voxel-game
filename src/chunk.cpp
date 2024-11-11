#include "chunk.h"
#define FNL_IMPL
#include "FastNoiseLite.h"

voxl::Chunk::Chunk(int x, int y, int z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}

voxl::Chunk::~Chunk()
{
    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < CHUNK_SIZE; y++)
        {
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
				delete cubes[x][y][z];
			}
		}
	}

}

void voxl::Chunk::generate()
{
    // Initialize FastNoiseLite for Perlin noise
    fnl_state noise = fnlCreateState();
    noise.noise_type = FNL_NOISE_PERLIN;
    noise.frequency = 0.05f;

    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int z = 0; z < CHUNK_SIZE; z++)
        {
            // Generate a height for the current (x, z) position based on noise
            float noiseValue = fnlGetNoise2D(&noise, m_x + x, m_z + z);
            int maxHeight = static_cast<int>((noiseValue + 1.0f) * (CHUNK_SIZE / 2));  // Map noise to chunk height

            for (int y = 0; y < CHUNK_SIZE; y++)
            {
                BlockType type = BlockType::None;

                if (y < maxHeight)  // Only set blocks up to maxHeight
                {
                    if (y < maxHeight - 2) {
                        type = BlockType::Stone;
                    }
                    else if (y == maxHeight - 1) {
                        type = BlockType::Grass;
                    }
                    else {
                        type = BlockType::Dirt;
                    }
                }

                // Create the cube with the assigned type at (x, y, z)
                cubes[x][y][z] = new Cube(type, glm::vec3(m_x + x, m_y + y, m_z + z));
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
        return cubes[x][y][z]->getType();
	}

	return BlockType::None;
}