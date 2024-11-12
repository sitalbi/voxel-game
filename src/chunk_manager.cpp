#include "chunk_manager.h"

voxl::ChunkManager::ChunkManager()
{
}

voxl::ChunkManager::~ChunkManager()
{
}

void voxl::ChunkManager::loadChunks(glm::vec3 playerPosition)
{
	int chunkX = static_cast<int>(playerPosition.x) / Chunk::CHUNK_SIZE;
	int chunkZ = static_cast<int>(playerPosition.z) / Chunk::CHUNK_SIZE;

	for (int x = -LOAD_RADIUS; x < LOAD_RADIUS; ++x) {
		for (int z = -LOAD_RADIUS; z < LOAD_RADIUS; ++z) {
			glm::ivec3 chunkCoords(chunkX + (x * Chunk::CHUNK_SIZE), 0, chunkZ + (z * Chunk::CHUNK_SIZE));

			// Check if this chunk is already loaded
			if (m_chunks.find(chunkCoords) == m_chunks.end()) {
				Chunk* newChunk = new Chunk(chunkCoords.x, chunkCoords.y, chunkCoords.z);
				newChunk->generate();    // Generate block data
				m_chunks[chunkCoords] = newChunk;
			}
		}
	}
}

void voxl::ChunkManager::updateChunks(glm::vec3 playerPosition)
{
}

void voxl::ChunkManager::unloadChunks(glm::vec3 playerPosition)
{
	int chunkX = static_cast<int>(playerPosition.x) / Chunk::CHUNK_SIZE;
	int chunkZ = static_cast<int>(playerPosition.z) / Chunk::CHUNK_SIZE;

	for (auto it = m_chunks.begin(); it != m_chunks.end(); ) {
		glm::ivec3 chunkCoords = it->first;
		int distanceX = abs(chunkCoords.x - chunkX);
		int distanceZ = abs(chunkCoords.z - chunkZ);

		if (distanceX > LOAD_RADIUS || distanceZ > LOAD_RADIUS) {
			delete it->second;
			it = m_chunks.erase(it); // Erase and advance iterator
		}
		else {
			++it;
		}
	}
}

voxl::Chunk* voxl::ChunkManager::getChunk(int x, int y, int z)
{
	return nullptr;
}
