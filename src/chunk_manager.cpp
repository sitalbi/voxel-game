#include "chunk_manager.h"
#include "chunk.h"
#include <iostream>

namespace voxl {

ChunkManager::ChunkManager()
{
}

ChunkManager::~ChunkManager()
{
	for (auto& chunk : m_chunks)
	{
		delete chunk.second;
	}
	m_chunks.clear();
}

void ChunkManager::loadChunks(glm::vec3 playerPosition)
{
	// Load chunks around the player
	int playerChunkX = static_cast<int>(playerPosition.x) / Chunk::CHUNK_SIZE;
	int playerChunkZ = static_cast<int>(playerPosition.z) / Chunk::CHUNK_SIZE;


	for (int x = playerChunkX - ChunkManager::LOAD_RADIUS; x < playerChunkX + ChunkManager::LOAD_RADIUS; x++)
	{
		for (int z = playerChunkZ - ChunkManager::LOAD_RADIUS; z < playerChunkZ + ChunkManager::LOAD_RADIUS; z++)
		{
			glm::ivec3 chunkPos(x, 0, z);
			if (m_chunksCache.find(chunkPos) == m_chunksCache.end()) {
				if (m_chunks.find(chunkPos) == m_chunks.end()) {
					Chunk* chunk = new Chunk(x * Chunk::CHUNK_SIZE, 0, z * Chunk::CHUNK_SIZE, this);
					chunk->generate();
					m_chunks[chunkPos] = chunk;
					//m_chunksCache[chunkPos] = chunk;
					m_updateList.insert(chunkPos);

					// Update neighboring chunks
					std::vector<glm::ivec3> neighbors = {
						glm::ivec3(x - 1, 0, z),
						glm::ivec3(x + 1, 0, z),
						glm::ivec3(x, 0, z - 1),
						glm::ivec3(x, 0, z + 1)
					};

					for (const auto& neighborPos : neighbors) {
						if (m_chunks.find(neighborPos) != m_chunks.end()) {
							m_updateList.insert(neighborPos);
						}
					}
				}
			}
			else {
				m_chunks[chunkPos] = m_chunksCache[chunkPos];
			}
		}
	}
}

void ChunkManager::updateChunks(glm::vec3 playerPosition)
{
	loadChunks(playerPosition);
	unloadChunks(playerPosition);

	for (const glm::ivec3& chunkPos : m_updateList)
	{
		auto it = m_chunks.find(chunkPos);
		if (it != m_chunks.end())
		{
			it->second->generateMesh();
		}
	}

	m_updateList.clear();
}

void ChunkManager::unloadChunks(glm::vec3 playerPosition)
{
	// Unload chunks that are too far away from the player
	int playerChunkX = static_cast<int>(playerPosition.x) / Chunk::CHUNK_SIZE;
	int playerChunkZ = static_cast<int>(playerPosition.z) / Chunk::CHUNK_SIZE;
	std::vector<glm::ivec3> chunksToRemove;
	for (auto& chunk : m_chunks)
	{
		int chunkX = chunk.second->getPosition().x / Chunk::CHUNK_SIZE;
		int chunkZ = chunk.second->getPosition().z / Chunk::CHUNK_SIZE;
		if (abs(chunkX - playerChunkX) > ChunkManager::LOAD_RADIUS || abs(chunkZ - playerChunkZ) > ChunkManager::LOAD_RADIUS)
		{
			chunksToRemove.push_back(glm::ivec3(chunkX, 0, chunkZ));
		}
	}
	for (auto& chunk : chunksToRemove)
	{
		m_chunks.erase(chunk);
	}
}

Chunk* ChunkManager::getChunk(int x, int y, int z) const
{
	int chunkX = x / Chunk::CHUNK_SIZE;
	int chunkY = y / Chunk::CHUNK_HEIGHT;
	int chunkZ = z / Chunk::CHUNK_SIZE;

	if (x < 0 && x % Chunk::CHUNK_SIZE != 0) chunkX -= 1;
	if (y < 0 && y % Chunk::CHUNK_HEIGHT != 0) chunkY -= 1;
	if (z < 0 && z % Chunk::CHUNK_SIZE != 0) chunkZ -= 1;

	glm::ivec3 chunkPos(chunkX, chunkY, chunkZ);
	auto it = m_chunks.find(chunkPos);
	if (it != m_chunks.end()) {
		return it->second;
	}
	return nullptr;
}

void ChunkManager::updateChunk(Chunk* chunk)
{
	glm::vec3 chunkPos = chunk->getPosition();
	chunkPos.x = static_cast<int>(chunkPos.x) / Chunk::CHUNK_SIZE;
	chunkPos.z = static_cast<int>(chunkPos.z) / Chunk::CHUNK_SIZE;
	m_updateList.insert(chunkPos);

	// Update neighboring chunks
	std::vector<glm::ivec3> neighbors = {
		glm::ivec3(chunkPos.x - 1, 0, chunkPos.z),
		glm::ivec3(chunkPos.x + 1, 0, chunkPos.z),
		glm::ivec3(chunkPos.x, 0, chunkPos.z - 1),
		glm::ivec3(chunkPos.x, 0, chunkPos.z + 1)
	};

	for (const auto& neighborPos : neighbors) {
		if (m_chunks.find(neighborPos) != m_chunks.end()) {
			m_updateList.insert(neighborPos);
		}
	}
}
} // namespace voxl