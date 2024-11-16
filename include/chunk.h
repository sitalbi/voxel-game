#include "cube.h"
#include "glad/glad.h"
#include "mesh.h"
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

struct Quad {
	int x;        
	int y;       
	int z;       
	int width;    
	int height;   
	int axis;     
};


class Chunk {


public:
	static const int CHUNK_SIZE = 32;

	Chunk(int x, int y, int z);
	~Chunk();

	void generate();
	void generateMesh();

	bool isFaceVisible(int x, int y, int z, int direction);
	BlockType getBlock(glm::vec3 pos);

	Mesh* getMesh() { return m_mesh; }

	glm::vec3 getPosition() { return glm::vec3(m_x, m_y, m_z); }


	BlockType cubes[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

	int getIndexCount() { return m_indexCount; }

private:
	int m_x, m_y, m_z;
	int m_indexCount;

	Mesh* m_mesh;

	void addFace(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, std::vector<unsigned int>& indices,
				int x, int y, int z, int faceIndex);

};

} // namespace voxl