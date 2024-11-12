#include "shader.h"
#include "cube.h"
#include "chunk_manager.h"
#include "map"

namespace voxl {

    static const std::vector<float> g_VertexBufferData = {
        // Front face (correct orientation)
        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, 0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, 0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, 0.5f,  0.0f, 1.0f,

        // Back face (flipped to match front)
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,

        // Right face (adjusted to be consistent)
         0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f,

         // Left face (adjusted to be consistent)
         -0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
         -0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
         -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

         // Top face (consistent orientation)
         -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,
          0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
          0.5f,  0.5f, -0.5f, 1.0f, 0.0f,
         -0.5f,  0.5f, -0.5f, 0.0f, 0.0f,

         // Bottom face (consistent orientation)
         -0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 0.0f, 0.0f
    };


// Indices data
static const std::vector<unsigned int> g_IndexBufferData = {
    0, 1, 2, 0, 2, 3,       // Front face
    4, 6, 5, 4, 7, 6,       // Back face (reversed order)
    8, 9, 10, 8, 10, 11,    // Right face
    12, 13, 14, 12, 14, 15, // Left face
    16, 17, 18, 16, 18, 19, // Top face
    20, 21, 22, 20, 22, 23  // Bottom face
};

static const std::vector<float> g_NormalBufferData = {
    // Front face
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,

    // Back face
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,

    // Right face
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,

    // Left face
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,

    // Top face
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,

    // Bottom face
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f
};

class Renderer {
public:
	Renderer();
	~Renderer();

	void init();
	void renderCube(Cube& cube, glm::mat4 model, glm::mat4 view, glm::mat4 projection);
    void renderCubeWithCulling(Cube& cube, glm::mat4 model, glm::mat4 view, glm::mat4 projection,
        bool front, bool back, bool right, bool left, bool top, bool bottom);
	void renderChunk(Chunk& chunk, glm::mat4 view, glm::mat4 projection);
    void renderChunks(ChunkManager& chunkManager, glm::mat4 view, glm::mat4 projection);
    void clear();

private:
	unsigned int m_VAO, m_VBO, m_IBO, m_NBO;
	Shader* m_shader;
    bool m_initialized;

	unsigned int m_vertexDataSize;
	unsigned int m_indexDataCount;
	const float* m_vertexData;
	const unsigned int* m_indexData;
    unsigned int m_normalDataSize;
	const float* m_normalData;

	std::map<BlockType, unsigned int> m_textureIDs;

	void setupCube();
	unsigned int loadTexture(const char* path);
};
}; // namespace voxl