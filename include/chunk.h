#pragma once

#include "cube.h"
#include "mesh.h"
#include "vector"

#include <memory>

#include "glad/glad.h"

struct fnl_state;

namespace voxl
{
class ChunkManager;

enum class BiomeType {
	Forest = 0, 
	Plains, 
	Desert,
	Mountains
};

class Chunk {


public:
	static const int CHUNK_SIZE = 32;
	static const int CHUNK_HEIGHT = 128;

	Chunk(const Chunk* chunk);
	Chunk(int x, int y, int z, ChunkManager* chunkManager);
	~Chunk();

	BlockType cubes[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];

	Mesh* getMesh() { return m_mesh.get(); }
	Mesh* getWaterMesh() { return m_waterMesh.get(); }

	glm::vec3 getPosition() const { return glm::vec3(m_x, m_y, m_z); }
	int getIndexCount() { return m_indexCount; }

	void setBlockType(int x, int y, int z, BlockType type);

	void generate();
	void generateMesh();

	bool isFaceVisible(int x, int y, int z, int direction, BlockType faceType);

private:
	int m_x, m_y, m_z;
	int m_indexCount;

	ChunkManager* m_chunkManager;

	std::unique_ptr<Mesh> m_mesh;
	std::unique_ptr<Mesh> m_waterMesh;

	void addFace(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, std::vector<unsigned int>& indices,
				int x, int y, int z, int faceIndex);

	BiomeType getBiomeType(fnl_state& noise, int x, int z) const;

	void placeTree(int x, int y, int z);
};

} // namespace voxl