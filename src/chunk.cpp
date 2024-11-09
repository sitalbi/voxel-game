#include "chunk.h"

voxl::Chunk::Chunk(int x, int y, int z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}

voxl::Chunk::~Chunk()
{
}

void voxl::Chunk::generate()
{
	cubes.resize(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			cubes[x + z * CHUNK_SIZE * CHUNK_SIZE] = BlockType::Dirt;
		}
		
	}
}

void voxl::Chunk::update()
{
	// Clear mesh data
	m_vertices.clear();
	m_indices.clear();

	// Generate mesh data
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				// if block visible add to mesh data
				if (cubes[x + y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE] != BlockType::None)
				{
					addCubeToMesh(x, y, z);
				}
			}
		}
	}

	// Generate buffers if not already generated
	if (m_VAO == 0)
	{
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);
	}

	// Bind VAO
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

	// Vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // Texture coordinates
	glEnableVertexAttribArray(1);

	// Unbind VAO
	glBindVertexArray(0);

}

void voxl::Chunk::addCubeToMesh(int x, int y, int z)
{
	if (isFaceVisible(x, y, z + 1))
	{
		addFace(x, y, z, Face::Front);
	}
	if (isFaceVisible(x, y, z - 1))
	{
		addFace(x, y, z, Face::Back);
	}
	if (isFaceVisible(x - 1, y, z))
	{
		addFace(x, y, z, Face::Left);
	}
	if (isFaceVisible(x + 1, y, z))
	{
		addFace(x, y, z, Face::Right);
	}
	if (isFaceVisible(x, y + 1, z))
	{
		addFace(x, y, z, Face::Top);
	}
	if (isFaceVisible(x, y - 1, z))
	{
		addFace(x, y, z, Face::Bottom);
	}

}

bool voxl::Chunk::isFaceVisible(int x, int y, int z)
{
	if (x < 0 || y < 0 || z < 0 || x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE)
	{
		return true;
	}

	int index = x + y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE;
	return cubes[index] == BlockType::None;
}

void voxl::Chunk::addFace(int x, int y, int z, Face face)
{
	for (int i = 0; i < 20; i+=5)
	{
		m_vertices.push_back(faceVertices[i] + x);
		m_vertices.push_back(faceVertices[i + 1] + y);
		m_vertices.push_back(faceVertices[i + 2] + z);
		m_vertices.push_back(faceVertices[i + 3]);
		m_vertices.push_back(faceVertices[i + 4]);
	}

	unsigned int offset = m_vertices.size() / 5 - 4;
	m_indices.insert(m_indices.end(), {offset, offset + 1, offset + 2, offset, offset + 1, offset + 2});
}
