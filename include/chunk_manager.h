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
			using std::size_t;
			using std::hash;

			return ((hash<int>()(k.x) ^ (hash<int>()(k.y) << 1)) >> 1) ^ (hash<int>()(k.z) << 1);
		}
	};
} // namespace std

namespace voxl
{

class Chunk;

class ChunkManager
{
public:
	static const int LOAD_RADIUS = 12;

	ChunkManager();
	~ChunkManager();

	void loadChunks(glm::vec3 playerPosition);
	void updateChunks(glm::vec3 playerPosition);
	void unloadChunks(glm::vec3 playerPosition);

	const Chunk* getChunk(int x, int y, int z) const;
	std::unordered_map<glm::ivec3, Chunk*> getChunks() const { return m_chunks; }


private:
	std::unordered_map<glm::ivec3, Chunk*> m_chunks;

	std::unordered_set<glm::ivec3> m_updateList;
};
} // namespace voxl