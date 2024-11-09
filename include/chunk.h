#include "cube.h"
#include "glad/glad.h"
#include "vector"

namespace voxl
{

static const float faceVertices[] = {
	// Positions      // UVs
	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  // Bottom-left
	 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // Bottom-right
	 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,  // Top-right
	-0.5f,  0.5f, 0.0f, 0.0f, 1.0f   // Top-left
};

// Define indices to form two triangles for the quad
static const int faceIndices[] = {
	0, 1, 2,   // First triangle
	0, 2, 3    // Second triangle
};

enum class Face
{
	Front,
	Back,
	Left,
	Right,
	Top,
	Bottom
};

class Chunk {


public:
	Chunk(int x, int y, int z);
	~Chunk();

	void generate();
	void update();

	static const int CHUNK_SIZE = 16;

	std::vector<BlockType> cubes;

private:
	int m_x, m_y, m_z;

	std::vector<float> m_vertices; // Chunk's mesh vertices
	std::vector<unsigned int> m_indices; // Chunk's mesh indices

	unsigned int m_VBO, m_VAO, m_EBO;

	void addCubeToMesh(int x, int y, int z);
	bool isFaceVisible(int x, int y, int z);
	void addFace(int x, int y, int z, Face face);

};

} // namespace voxl