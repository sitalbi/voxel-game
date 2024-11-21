#include "chunk.h"
#include "chunk_manager.h"
#include "renderer.h"  
#include "glm/glm.hpp"
#include <array>
#include <iostream>

#define FNL_IMPL
#include "FastNoiseLite.h"

namespace voxl {

Chunk::Chunk(const Chunk* chunk)
{
	m_x = chunk->m_x;
	m_y = chunk->m_y;
	m_z = chunk->m_z;
	m_chunkManager = chunk->m_chunkManager;
	if (chunk->m_mesh)
	{
		m_mesh = std::make_unique<Mesh>(*chunk->m_mesh);
	}
}

Chunk::Chunk(int x, int y, int z, ChunkManager* chunkManager)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_chunkManager = chunkManager;
    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < CHUNK_SIZE; y++)
        {
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                cubes[x][y][z] = BlockType::None;
            }
        }
    }
}

Chunk::~Chunk()
{
}

void Chunk::setBlockType(int x, int y, int z, BlockType type)
{
	cubes[x][y][z] = type;
}

void Chunk::generate()
{
    fnl_state heightNoise = fnlCreateState();
    heightNoise.noise_type = FNL_NOISE_PERLIN;
    heightNoise.frequency = 0.017f;

    fnl_state biomeNoise = fnlCreateState();
	biomeNoise.noise_type = FNL_NOISE_OPENSIMPLEX2;
    biomeNoise.frequency = 0.001f;

    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int z = 0; z < CHUNK_SIZE; z++)
        {
			BiomeType biome = getBiomeType(biomeNoise, m_x + x, m_z + z);

            // Calculate height using height noise
            float noiseValue = fnlGetNoise2D(&heightNoise, m_x + x, m_z + z);
            int maxHeight = static_cast<int>((noiseValue + 1.0f) * (CHUNK_SIZE / 2));

            for (int y = 0; y < CHUNK_SIZE; y++)
            {
                BlockType type = BlockType::None;

                if (y < maxHeight)
                {
                    // Assign block types based on biome and height
                    switch (biome)
                    {
                    case BiomeType::Desert:
                        if (y < maxHeight - 3) {
                            type = BlockType::Stone;
                        }
                        else {
                            type = BlockType::Sand;
                        }
                        break;

                    case BiomeType::Plains:
                        if (y < maxHeight - 5) {
                            type = BlockType::Stone;
                        }
                        else if (y < maxHeight - 1) {
                            type = BlockType::Dirt;
                        }
                        else if (y >= 20) {
							type = BlockType::Stone;
						}
                        else {
                            type = BlockType::Grass;
                        }
                        break;

                    case BiomeType::Forest:
                        if (y < maxHeight - 5) {
                            type = BlockType::Stone;
                        }
                        else if (y < maxHeight - 1) {
                            type = BlockType::Dirt;
                        }
                        else {
                            type = BlockType::Grass;
                        }
                        break;
                    }
                    if (cubes[x][y][z] == BlockType::None)
                    {
                        cubes[x][y][z] = type;
                    }
                }

                
            }

            // Place trees on top of grass blocks for suitable biomes 
            if (biome == BiomeType::Forest || biome == BiomeType::Plains)
            {
                float treeProbability = (biome == BiomeType::Forest) ? 0.0035f : 0.001f;

				if (cubes[x][maxHeight-1][z] == BlockType::Grass)
				{
					if (static_cast<float>(rand()) / RAND_MAX < treeProbability)
					{
						placeTree(x, maxHeight, z);
					}
				}
            }
        }
    }
}



void Chunk::generateMesh() {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<uint32_t> indices;
    std::vector<glm::vec3> colors;

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (cubes[x][y][z] != BlockType::None) {
                    for (int direction = 0; direction < 6; direction++) {
                        if (isFaceVisible(x, y, z, direction)) {
							colors.push_back(g_cubeColors.at(cubes[x][y][z]));
                            colors.push_back(g_cubeColors.at(cubes[x][y][z]));
                            colors.push_back(g_cubeColors.at(cubes[x][y][z]));
                            colors.push_back(g_cubeColors.at(cubes[x][y][z]));
                            addFace(vertices, normals, indices, x, y, z, direction);
                        }
                    }
                }
            }
        }
    }

    // Create the actual mesh
	m_mesh = std::make_unique<Mesh>(vertices, normals, indices, colors);
}

void Chunk::addFace(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, std::vector<uint32_t>& indices,
    int x, int y, int z, int faceIndex) {
    glm::vec3 v1, v2, v3, v4;
    glm::vec3 normal;

    switch (faceIndex) {
    case 0: // Left face
        v1 = glm::vec3(x, y, z + 1);
        v2 = glm::vec3(x, y + 1, z + 1);
        v3 = glm::vec3(x, y + 1,z);
        v4 = glm::vec3(x, y, z);
        normal = glm::vec3(-1.0f, 0.0f, 0.0f);
        break;
    case 1: // Right face
        v1 = glm::vec3(x + 1, y, z);
        v2 = glm::vec3(x + 1, y + 1, z);
        v3 = glm::vec3(x + 1, y + 1, z + 1);
        v4 = glm::vec3(x + 1, y, z + 1);
        normal = glm::vec3(1.0f, 0.0f, 0.0f);
        break;
    case 2: // Bottom face
        v1 = glm::vec3(x+1, y, z);
        v2 = glm::vec3(x + 1, y, z+1);
        v3 = glm::vec3(x, y, z + 1);
        v4 = glm::vec3(x, y, z);
        normal = glm::vec3(0.0f, -1.0f, 0.0f);
        break;
    case 3: // Top face
        v1 = glm::vec3(x, y + 1, z+1);
        v2 = glm::vec3(x + 1, y + 1, z+1);
        v3 = glm::vec3(x + 1, y + 1, z);
        v4 = glm::vec3(x, y + 1, z);
        normal = glm::vec3(0.0f, 1.0f, 0.0f);
        break;
    case 4: // Back face
        v1 = glm::vec3(x+1, y, z);
        v2 = glm::vec3(x, y, z);
        v3 = glm::vec3(x, y + 1, z);
        v4 = glm::vec3(x+1, y + 1, z);
        normal = glm::vec3(0.0f, 0.0f, -1.0f);
        break;
    case 5: // Front face
        v1 = glm::vec3(x, y, z + 1);
        v2 = glm::vec3(x + 1, y, z + 1);
        v3 = glm::vec3(x + 1, y + 1, z + 1);
        v4 = glm::vec3(x, y + 1, z + 1);
        normal = glm::vec3(0.0f, 0.0f, 1.0f);
        break;
    default:
        return;
    }

    uint32_t baseIndex = static_cast<uint32_t>(vertices.size());
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    vertices.push_back(v4);

    normals.push_back(normal);
    normals.push_back(normal);
    normals.push_back(normal);
    normals.push_back(normal);

    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 3);
    indices.push_back(baseIndex);
}

BiomeType Chunk::getBiomeType(fnl_state& noise, int x, int z) const
{
    // Simple example of determining biome based on noise value.
    float biomeNoise = fnlGetNoise2D(&noise, m_x + x, m_z + z);

    if (biomeNoise < -0.3f) {
        return BiomeType::Desert;
    }
    else if (biomeNoise < 0.3f) {
        return BiomeType::Plains;
    }
    else {
        return BiomeType::Forest;
    }
}

void Chunk::placeTree(int x, int y, int z)
{
    int trunkHeight = rand() % 4 + 3; ; 
    int treeTopHeight = y + trunkHeight;

    // Place trunk blocks
    for (int h = y; h < treeTopHeight && h < CHUNK_SIZE; h++)
    {
        cubes[x][h][z] = BlockType::Wood;
    }

    // Place leaves
    int leafStart = treeTopHeight - 2; 
    for (int lx = -2; lx <= 2; lx++)
    {
        for (int lz = -2; lz <= 2; lz++)
        {
            for (int ly = 0; ly <= 2; ly++)
            {
                int leafX = x + lx;
                int leafY = leafStart + ly;
                int leafZ = z + lz;

                // Make sure leaves are within bounds of the chunk
                if (leafX >= 0 && leafX < CHUNK_SIZE &&
                    leafY >= 0 && leafY < CHUNK_SIZE &&
                    leafZ >= 0 && leafZ < CHUNK_SIZE)
                {
                    cubes[leafX][leafY][leafZ] = BlockType::Leaves;
                }
            }
        }
    }
}



bool Chunk::isFaceVisible(int x, int y, int z, int direction)
{
    if (direction == 0) { // Left face
        if (x == 0) {
            const Chunk* neighbor = m_chunkManager->getChunk(m_x - CHUNK_SIZE, m_y, m_z);
            return !neighbor || neighbor->cubes[CHUNK_SIZE - 1][y][z] == BlockType::None;
        }
        return cubes[x - 1][y][z] == BlockType::None;
    }
    else if (direction == 1) { // Right face
        if (x == CHUNK_SIZE - 1) {
            const Chunk* neighbor = m_chunkManager->getChunk(m_x + CHUNK_SIZE, m_y , m_z );
            return !neighbor || neighbor->cubes[0][y][z] == BlockType::None;
        }
        return cubes[x + 1][y][z] == BlockType::None;
    }
    else if (direction == 2) { // Bottom face
        if (y == 0) {
            const Chunk* neighbor = m_chunkManager->getChunk(m_x, m_y - CHUNK_SIZE, m_z);
            return !neighbor || neighbor->cubes[x][CHUNK_SIZE - 1][z] == BlockType::None;
        }
        return cubes[x][y - 1][z] == BlockType::None;
    }
    else if (direction == 3) { // Top face
        if (y == CHUNK_SIZE - 1) {
            const Chunk* neighbor = m_chunkManager->getChunk(m_x, m_y + CHUNK_SIZE, m_z);
            return !neighbor || neighbor->cubes[x][0][z] == BlockType::None;
        }
        return cubes[x][y + 1][z] == BlockType::None;
    }
    else if (direction == 4) { // Back face
        if (z == 0) {
            const Chunk* neighbor = m_chunkManager->getChunk(m_x, m_y, m_z - CHUNK_SIZE);
            return !neighbor || neighbor->cubes[x][y][CHUNK_SIZE - 1] == BlockType::None;
        }
        return cubes[x][y][z - 1] == BlockType::None;
    }
    else if (direction == 5) { // Front face
        if (z == CHUNK_SIZE - 1) {
            const Chunk* neighbor = m_chunkManager->getChunk(m_x, m_y, m_z + CHUNK_SIZE);
            return !neighbor || neighbor->cubes[x][y][0] == BlockType::None;
        }
        return cubes[x][y][z + 1] == BlockType::None;
    }
    return false;
}

} // namespace voxl