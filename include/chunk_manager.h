#include "chunk.h"
#include "glm/glm.hpp"
#include <unordered_map>

namespace voxl
{
class ChunkManager
{
public:
	ChunkManager();
	~ChunkManager();

	void loadChunks(glm::vec3 playerPosition);
	void updateChunks(glm::vec3 playerPosition);

	Chunk* getChunk(int x, int y, int z);

private:
	std::unordered_map<glm::ivec3, Chunk*> m_chunks;

}
} // namespace voxl