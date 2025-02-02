#pragma once

#include "glm/glm.hpp"
#include "cube.h"
#include <unordered_map>
#include <unordered_set>

namespace std {
	template <>
	struct hash<glm::ivec3>
	{
		std::size_t operator()(const glm::ivec3& k) const
		{
			return std::hash<int>()(k.x) ^ std::hash<int>()(k.y) ^ std::hash<int>()(k.z);
		}
	};
} // namespace std

namespace voxl
{

class Chunk;

class ChunkManager
{
public:
	static const int LOAD_RADIUS = 8;

	ChunkManager();
	~ChunkManager();

	void loadChunks(glm::vec3 playerPosition);
	void updateChunks(glm::vec3 playerPosition);
	void unloadChunks(glm::vec3 playerPosition);

	Chunk* getChunk(float x, float y, float z) const;
	std::unordered_map<glm::ivec3, Chunk*> getChunks() const { return m_chunks; }

	void updateChunk(Chunk* chunk);

	BlockType getBlockType(float x, float y, float z) const;
	bool isSolidBlock(float x, float y, float z) const;

private:
	std::unordered_map<glm::ivec3, Chunk*> m_chunks;

	std::unordered_set<glm::ivec3> m_updateList;

	std::unordered_map<glm::ivec3, Chunk*> m_chunksCache;
};
} // namespace voxl