#include "chunk.h"
#include "glm/glm.hpp"
#include <unordered_map>

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
class ChunkManager
{
public:
	ChunkManager();
	~ChunkManager();

	void loadChunks(glm::vec3 playerPosition);
	void updateChunks(glm::vec3 playerPosition);
	void unloadChunks(glm::vec3 playerPosition);

	Chunk* getChunk(int x, int y, int z);
	std::unordered_map<glm::ivec3, Chunk*>& getChunks() { return m_chunks; }

	static const int LOAD_RADIUS = 2;

private:
	std::unordered_map<glm::ivec3, Chunk*> m_chunks;
};
} // namespace voxl