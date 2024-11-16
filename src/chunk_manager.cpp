#include "chunk_manager.h"
#include <iostream>

voxl::ChunkManager::ChunkManager()
{
}

voxl::ChunkManager::~ChunkManager()
{
	for (auto& chunk : m_chunks)
	{
		delete chunk.second;
	}
	m_chunks.clear();
}

void voxl::ChunkManager::loadChunks(glm::vec3 playerPosition)
{
	// Load chunks around the player
	int playerChunkX = static_cast<int>(playerPosition.x) / Chunk::CHUNK_SIZE;
	int playerChunkZ = static_cast<int>(playerPosition.z) / Chunk::CHUNK_SIZE;

	for (int x = playerChunkX - ChunkManager::LOAD_RADIUS; x < playerChunkX + ChunkManager::LOAD_RADIUS; x++)
	{
		for (int z = playerChunkZ - ChunkManager::LOAD_RADIUS; z < playerChunkZ + ChunkManager::LOAD_RADIUS; z++)
		{
			// Check if the chunk is already loaded
			if (m_chunks.find(glm::ivec3(x, 0, z)) == m_chunks.end())
			{
				Chunk* chunk = new Chunk(x * Chunk::CHUNK_SIZE, 0, z * Chunk::CHUNK_SIZE);
				chunk->generate();
				m_chunks[glm::ivec3(x, 0, z)] = chunk;
			}
		}
	}
}

void voxl::ChunkManager::updateChunks(glm::vec3 playerPosition)
{
	loadChunks(playerPosition);
	unloadChunks(playerPosition);
}

void voxl::ChunkManager::unloadChunks(glm::vec3 playerPosition)
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
		delete m_chunks[chunk];
		m_chunks.erase(chunk);
	}
}

voxl::Chunk* voxl::ChunkManager::getChunk(int x, int y, int z)
{
	return nullptr;
}
