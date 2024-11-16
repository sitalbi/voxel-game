#include "shader.h"
#include "cube.h"
#include "chunk_manager.h"
#include "chunk.h"
#include "mesh.h"
#include "map"

namespace voxl {

class Renderer {
public:
	Renderer();
	~Renderer();

	void init();
	void renderChunk(Chunk& chunk, glm::mat4 view, glm::mat4 projection);
    void renderChunks(const ChunkManager& chunkManager, glm::mat4 view, glm::mat4 projection);

    void renderMesh(Mesh* mesh, glm::mat4 model, glm::mat4 view, glm::mat4 projection);

    void clear();

private:
	Shader* m_shader;
    bool m_initialized;

	unsigned int loadTexture(const char* path);
};
}; // namespace voxl