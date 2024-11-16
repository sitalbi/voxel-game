#include "chunk.h"
#include "glm/glm.hpp"
#include <array>
#include <iostream>
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
    delete m_mesh;
}

void voxl::Chunk::generate()
{
    // Initialize FastNoiseLite for Perlin noise
    fnl_state noise = fnlCreateState();
    noise.noise_type = FNL_NOISE_PERLIN;
    noise.frequency = 0.015f;

    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int z = 0; z < CHUNK_SIZE; z++)
        {
            // Generate a height for the current (x, z) position based on noise
            float noiseValue = fnlGetNoise2D(&noise, m_x + x, m_z + z);
            int maxHeight = static_cast<int>((noiseValue + 1.0f) * (CHUNK_SIZE / 2)); 

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
                cubes[x][y][z] = type;
            }
        }
    }

    generateMesh();
}


void voxl::Chunk::generateMesh() {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<uint32_t> indices;

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (cubes[x][y][z] != BlockType::None) {
                    for (int direction = 0; direction < 6; direction++) {
                        if (isFaceVisible(x, y, z, direction)) {
                            addFace(vertices, normals, indices, x, y, z, direction);
                        }
                    }
                }
            }
        }
    }

    // Create the actual mesh
    m_mesh = new Mesh(vertices, normals, indices);
}

void voxl::Chunk::addFace(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, std::vector<uint32_t>& indices,
    int x, int y, int z, int faceIndex) {
    glm::vec3 v1, v2, v3, v4;
    glm::vec3 normal;

    switch (faceIndex) {
    case 0: // Left face
        v1 = glm::vec3(x, y, z+1);
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


bool voxl::Chunk::isFaceVisible(int x, int y, int z, int direction)
{
    if (direction == 0) {
        return x == 0 || cubes[x - 1][y][z] == BlockType::None;
    }
    else if (direction == 1) {
        return x == CHUNK_SIZE - 1 || cubes[x + 1][y][z] == BlockType::None;
    }
    else if (direction == 2) {
        return y == 0 || cubes[x][y - 1][z] == BlockType::None;
    }
    else if (direction == 3) {
        return y == CHUNK_SIZE - 1 || cubes[x][y + 1][z] == BlockType::None;
    }
    else if (direction == 4) {
        return z == 0 || cubes[x][y][z - 1] == BlockType::None;
    }
    else if (direction == 5) {
        return z == CHUNK_SIZE - 1 || cubes[x][y][z + 1] == BlockType::None;
    }
    return false;
}

voxl::BlockType voxl::Chunk::getBlock(glm::vec3 pos)
{
    int x = static_cast<int>(pos.x) - m_x;
    int y = static_cast<int>(pos.y) - m_y;
    int z = static_cast<int>(pos.z) - m_z;

    // Check if the position is within the chunk bounds
    if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE) {
        return cubes[x][y][z];
    }

    return BlockType::None;
}