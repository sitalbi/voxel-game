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
        for (int y = 0; y < CHUNK_HEIGHT; y++)
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
    const int WATER_HEIGHT = 10; 

    // Create and configure biome noise
    fnl_state biomeNoise = fnlCreateState();
    biomeNoise.noise_type = FNL_NOISE_OPENSIMPLEX2;
    biomeNoise.frequency = 0.0005f;

    // Precompute biomes for the chunk
    BiomeType biomeMap[CHUNK_SIZE][CHUNK_SIZE];
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            biomeMap[x][z] = getBiomeType(biomeNoise, m_x + x, m_z + z);
        }
    }

    // Create noise configurations for each biome
    std::unordered_map<BiomeType, fnl_state> biomeNoiseConfigs;

    fnl_state desertNoise = fnlCreateState();
    desertNoise.noise_type = FNL_NOISE_PERLIN;
    desertNoise.frequency = 0.02f; 
    biomeNoiseConfigs[BiomeType::Desert] = desertNoise;

    fnl_state forestNoise = fnlCreateState();
    forestNoise.noise_type = FNL_NOISE_PERLIN;
    forestNoise.frequency = 0.01f; 
    biomeNoiseConfigs[BiomeType::Forest] = forestNoise;

    fnl_state plainsNoise = fnlCreateState();
    plainsNoise.noise_type = FNL_NOISE_PERLIN;
    plainsNoise.frequency = 0.03f; 
    biomeNoiseConfigs[BiomeType::Plains] = plainsNoise;

    fnl_state mountainsNoise = fnlCreateState();
    mountainsNoise.noise_type = FNL_NOISE_PERLIN;
    mountainsNoise.frequency = 0.05f; 
    biomeNoiseConfigs[BiomeType::Mountains] = mountainsNoise;

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            BiomeType biome = biomeMap[x][z];

            // Use biome-specific noise to calculate height
            fnl_state& heightNoise = biomeNoiseConfigs[biome];
            float noiseValue = fnlGetNoise2D(&heightNoise, m_x + x, m_z + z);
            int maxHeight = static_cast<int>((noiseValue + 1.0f) * (CHUNK_SIZE / 2));

            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                BlockType type = BlockType::None;

                if (y < maxHeight) {
                    switch (biome) {
                    case BiomeType::Desert:
                        type = (y < maxHeight - 3) ? BlockType::Stone : BlockType::Sand;
                        break;

                    case BiomeType::Plains:
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

                    case BiomeType::Mountains:
                        if (y >= 20) {
                            type = BlockType::Stone;
                        }
                        else if (y < maxHeight - 3) {
                            type = BlockType::Dirt;
                        }
                        else {
                            type = BlockType::Grass;
                            break;
                        }
                    }
                    cubes[x][y][z] = type;
                }
            }

            // Add water blocks at a minimum height
			if ((biome == BiomeType::Forest || biome == BiomeType::Plains) && maxHeight < WATER_HEIGHT)
			{
				for (int y = maxHeight; y < WATER_HEIGHT; y++) {
					if (cubes[x][y][z] == BlockType::None) {
						cubes[x][y][z] = BlockType::Water;
					}
				}
			}

            // Place trees for suitable biomes
            if ((biome == BiomeType::Forest || biome == BiomeType::Plains) &&
                cubes[x][maxHeight - 1][z] == BlockType::Grass && cubes[x][maxHeight][z] == BlockType::None) {
                float treeProbability = (biome == BiomeType::Forest) ? 0.0035f : 0.001f;
                if (static_cast<float>(rand()) / RAND_MAX < treeProbability) {
                    placeTree(x, maxHeight, z);
                }
            }
        }
    }
}





void Chunk::generateMesh() {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<uint32_t> indices;
    std::vector<glm::vec4> colors;

	std::vector<glm::vec3> waterVertices;
	std::vector<glm::vec3> waterNormals;
	std::vector<uint32_t> waterIndices;
	std::vector<glm::vec4> waterColors;

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (cubes[x][y][z] != BlockType::None) {
                    for (int direction = 0; direction < 6; direction++) {
                        if (isFaceVisible(x, y, z, direction, cubes[x][y][z])) {
							if (cubes[x][y][z] == BlockType::Water)
							{
								waterColors.push_back(glm::vec4(g_cubeColors.at(cubes[x][y][z]), 0.5f));
								waterColors.push_back(glm::vec4(g_cubeColors.at(cubes[x][y][z]), 0.5f));
								waterColors.push_back(glm::vec4(g_cubeColors.at(cubes[x][y][z]), 0.5f));
								waterColors.push_back(glm::vec4(g_cubeColors.at(cubes[x][y][z]), 0.5f));
								addFace(waterVertices, waterNormals, waterIndices, x, y, z, direction);
							}
							else
							{
								colors.push_back(glm::vec4(g_cubeColors.at(cubes[x][y][z]), 1.0f));
								colors.push_back(glm::vec4(g_cubeColors.at(cubes[x][y][z]), 1.0f));
								colors.push_back(glm::vec4(g_cubeColors.at(cubes[x][y][z]), 1.0f));
								colors.push_back(glm::vec4(g_cubeColors.at(cubes[x][y][z]), 1.0f));
								addFace(vertices, normals, indices, x, y, z, direction);
							}
                        }
                    }
                }
            }
        }
    }

    // Create the actual mesh
	m_mesh = std::make_unique<Mesh>(vertices, normals, indices, colors);
	m_waterMesh = std::make_unique<Mesh>(waterVertices, waterNormals, waterIndices, waterColors);
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
	else if (biomeNoise < 0.6f) {
		return BiomeType::Forest;
	}
	else {
		return BiomeType::Mountains;
	}
}

void Chunk::placeTree(int x, int y, int z)
{
    int trunkHeight = rand() % 4 + 3; ; 
    int treeTopHeight = y + trunkHeight;

    // Place trunk blocks
    for (int h = y; h < treeTopHeight && h < CHUNK_HEIGHT; h++)
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
                    leafY >= 0 && leafY < CHUNK_HEIGHT &&
                    leafZ >= 0 && leafZ < CHUNK_SIZE)
                {
                    cubes[leafX][leafY][leafZ] = BlockType::Leaves;
                }
            }
        }
    }
}



bool Chunk::isFaceVisible(int x, int y, int z, int direction, BlockType faceType)
{
    if (faceType == BlockType::Water) {
        if (direction == 0) { // Left face
            if (x == 0) {
                const Chunk* neighbor = m_chunkManager->getChunk(m_x - CHUNK_SIZE, m_y, m_z);
                return !neighbor || neighbor->cubes[CHUNK_SIZE - 1][y][z] == BlockType::None;
            }
            return cubes[x - 1][y][z] == BlockType::None;
        }
        else if (direction == 1) { // Right face
            if (x == CHUNK_SIZE - 1) {
                const Chunk* neighbor = m_chunkManager->getChunk(m_x + CHUNK_SIZE, m_y, m_z);
                return !neighbor || neighbor->cubes[0][y][z] == BlockType::None;
            }
            return cubes[x + 1][y][z] == BlockType::None;
        }
        else if (direction == 2) { // Bottom face
            if (y == 0) {
                const Chunk* neighbor = m_chunkManager->getChunk(m_x, m_y - CHUNK_HEIGHT, m_z);
                return !neighbor || neighbor->cubes[x][CHUNK_SIZE - 1][z] == BlockType::None;
            }
            return cubes[x][y - 1][z] == BlockType::None;
        }
        else if (direction == 3) { // Top face
            if (y == CHUNK_HEIGHT - 1) {
                const Chunk* neighbor = m_chunkManager->getChunk(m_x, m_y + CHUNK_HEIGHT, m_z);
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
    else {
        if (direction == 0) { // Left face
            if (x == 0) {
                const Chunk* neighbor = m_chunkManager->getChunk(m_x - CHUNK_SIZE, m_y, m_z);
                return !neighbor || neighbor->cubes[CHUNK_SIZE - 1][y][z] == BlockType::None || neighbor->cubes[CHUNK_SIZE - 1][y][z] == BlockType::Water;
            }
            return cubes[x - 1][y][z] == BlockType::None || cubes[x - 1][y][z] == BlockType::Water;
        }
        else if (direction == 1) { // Right face
            if (x == CHUNK_SIZE - 1) {
                const Chunk* neighbor = m_chunkManager->getChunk(m_x + CHUNK_SIZE, m_y, m_z);
                return !neighbor || neighbor->cubes[0][y][z] == BlockType::None || neighbor->cubes[0][y][z] == BlockType::Water;
            }
            return cubes[x + 1][y][z] == BlockType::None || cubes[x + 1][y][z] == BlockType::Water;
        }
        else if (direction == 2) { // Bottom face
            if (y == 0) {
                const Chunk* neighbor = m_chunkManager->getChunk(m_x, m_y - CHUNK_HEIGHT, m_z);
                return !neighbor || neighbor->cubes[x][CHUNK_SIZE - 1][z] == BlockType::None || neighbor->cubes[x][CHUNK_SIZE - 1][z] == BlockType::Water;
            }
            return cubes[x][y - 1][z] == BlockType::None || cubes[x][y - 1][z] == BlockType::Water;
        }
        else if (direction == 3) { // Top face
            if (y == CHUNK_HEIGHT - 1) {
                const Chunk* neighbor = m_chunkManager->getChunk(m_x, m_y + CHUNK_HEIGHT, m_z);
                return !neighbor || neighbor->cubes[x][0][z] == BlockType::None || neighbor->cubes[x][0][z] == BlockType::Water;
            }
            return cubes[x][y + 1][z] == BlockType::None || cubes[x][y + 1][z] == BlockType::Water;
        }
        else if (direction == 4) { // Back face
            if (z == 0) {
                const Chunk* neighbor = m_chunkManager->getChunk(m_x, m_y, m_z - CHUNK_SIZE);
                return !neighbor || neighbor->cubes[x][y][CHUNK_SIZE - 1] == BlockType::None || neighbor->cubes[x][y][CHUNK_SIZE - 1] == BlockType::Water;
            }
            return cubes[x][y][z - 1] == BlockType::None || cubes[x][y][z - 1] == BlockType::Water;
        }
        else if (direction == 5) { // Front face
            if (z == CHUNK_SIZE - 1) {
                const Chunk* neighbor = m_chunkManager->getChunk(m_x, m_y, m_z + CHUNK_SIZE);
                return !neighbor || neighbor->cubes[x][y][0] == BlockType::None || neighbor->cubes[x][y][0] == BlockType::Water;
            }
            return cubes[x][y][z + 1] == BlockType::None || cubes[x][y][z + 1] == BlockType::Water;
        }
        return false;
    }
   
}

} // namespace voxl