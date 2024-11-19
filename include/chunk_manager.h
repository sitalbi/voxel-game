#pragma once

#include "glm/glm.hpp"
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

	Chunk* getChunk(int x, int y, int z) const;
	std::unordered_map<glm::ivec3, Chunk*> getChunks() const { return m_chunks; }

	void addUpdate(glm::ivec3 chunkPos) { m_updateList.insert(chunkPos); }


private:
	std::unordered_map<glm::ivec3, Chunk*> m_chunks;

	std::unordered_set<glm::ivec3> m_updateList;
};
} // namespace voxl