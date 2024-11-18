#pragma once

#include "cube.h"
#include "mesh.h"
#include "vector"

#include "glad/glad.h"

namespace voxl
{

class ChunkManager;

class Chunk {


public:
	static const int CHUNK_SIZE = 32;

	Chunk(const Chunk* chunk);
	Chunk(int x, int y, int z, ChunkManager* chunkManager);
	~Chunk();

	BlockType cubes[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

	Mesh* getMesh() { return m_mesh; }
	glm::vec3 getPosition() const { return glm::vec3(m_x, m_y, m_z); }
	int getIndexCount() { return m_indexCount; }

	void generate();
	void generateMesh();

	bool isFaceVisible(int x, int y, int z, int direction);

private:
	int m_x, m_y, m_z;
	int m_indexCount;

	ChunkManager* m_chunkManager;

	Mesh* m_mesh;

	void addFace(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, std::vector<unsigned int>& indices,
				int x, int y, int z, int faceIndex);
};

} // namespace voxl