#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "shader.h"
#include "cube.h"
#include "mesh.h"
#include <player.h>
#include <chunk_manager.h>
#include <chunk.h>


#define window_width 1920
#define window_height 1080

namespace voxl {

    static const std::unordered_map<BlockType, glm::vec3> g_cubeColors = {
        { BlockType::Grass, glm::vec3(0.25f, 1.0f, 0.25f)},
        { BlockType::Dirt, glm::vec3(0.5f, 0.35f, 0.05f) },
        { BlockType::Stone, glm::vec3(0.7f, 0.85f, 0.85f) },
        { BlockType::Sand, glm::vec3(1.0f, 1.0f, 0.5f) },
        { BlockType::Wood, glm::vec3(0.83f, 0.69f, 0.415f)},
		{ BlockType::Water, glm::vec3(0.0f, 0.0f, 1.0f) },
		{ BlockType::Leaves, glm::vec3(0.5f, 1.0f, 0.0f) },
        { BlockType::Snow, glm::vec3(2.0f, 2.0f, 2.0f) }
    };

    static const std::vector<glm::vec3> g_cubeVertices = {
        // Front face
        glm::vec3(-0.5f, -0.5f,  0.5f), // Vertex 0
        glm::vec3(0.5f, -0.5f,  0.5f), // Vertex 1
        glm::vec3(0.5f,  0.5f,  0.5f), // Vertex 2
        glm::vec3(-0.5f,  0.5f,  0.5f), // Vertex 3
        // Back face
        glm::vec3(-0.5f, -0.5f, -0.5f), // Vertex 4
        glm::vec3(0.5f, -0.5f, -0.5f), // Vertex 5
        glm::vec3(0.5f,  0.5f, -0.5f), // Vertex 6
        glm::vec3(-0.5f,  0.5f, -0.5f), // Vertex 7
        // Right face
        glm::vec3(0.5f, -0.5f,  0.5f), // Vertex 8
        glm::vec3(0.5f, -0.5f, -0.5f), // Vertex 9
        glm::vec3(0.5f,  0.5f, -0.5f), // Vertex 10
        glm::vec3(0.5f,  0.5f,  0.5f), // Vertex 11
        // Left face
        glm::vec3(-0.5f, -0.5f,  0.5f), // Vertex 12
        glm::vec3(-0.5f,  0.5f,  0.5f), // Vertex 13
        glm::vec3(-0.5f,  0.5f, -0.5f), // Vertex 14
        glm::vec3(-0.5f, -0.5f, -0.5f), // Vertex 15
        // Top face
        glm::vec3(-0.5f,  0.5f,  0.5f), // Vertex 16
        glm::vec3(0.5f,  0.5f,  0.5f), // Vertex 17
        glm::vec3(0.5f,  0.5f, -0.5f), // Vertex 18
        glm::vec3(-0.5f,  0.5f, -0.5f), // Vertex 19
        // Bottom face
        glm::vec3(-0.5f, -0.5f,  0.5f), // Vertex 20
        glm::vec3(-0.5f, -0.5f, -0.5f), // Vertex 21
        glm::vec3(0.5f, -0.5f, -0.5f), // Vertex 22
        glm::vec3(0.5f, -0.5f,  0.5f), // Vertex 23
    };


    // Indices data
    static const std::vector<unsigned int> g_cubeIndices = {
        0, 1, 2, 0, 2, 3,       // Front face
        4, 5, 6, 4, 6, 7,       // Back face
        8, 9, 10, 8, 10, 11,    // Right face
        12, 13, 14, 12, 14, 15, // Left face
        16, 17, 18, 16, 18, 19, // Top face
        20, 21, 22, 20, 22, 23  // Bottom face
    };


    static const std::vector< glm::vec3> g_cubeNormals = {
        // Front face
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        // Back face
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        // Right face
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        // Left face
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        // Top face
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        // Bottom face
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f)
    };


class Renderer {
public:
	Renderer();
	~Renderer();

	void init();
	void generateCubeMesh();
	void initUI() const;

	void setupUI(Player& player, const glm::vec3& blockPos);

    void update(Player& player, const ChunkManager& chunkManager);

	void renderCube(BlockType type, glm::vec3 position, glm::mat4 view, glm::mat4 projection);
	void renderChunk(Chunk& chunk, glm::mat4 view, glm::mat4 projection, bool transparent);
    void renderChunks(const ChunkManager& chunkManager, glm::mat4 view, glm::mat4 projection);
	void renderHighlight(glm::vec3 block, glm::mat4 view, glm::mat4 projection);

    void renderMesh(Mesh& mesh, Shader& shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection);

	void renderUI();

    void clear();

	unsigned int getCrosshairTexture() { return m_crosshairTexture; }


    void updateLighting(const glm::vec3& lightTarget, float deltaTime);

    GLFWwindow* window;
    

private:
    const unsigned int SHADOW_WIDTH = 4096*4, SHADOW_HEIGHT = 4096*4;

	std::unique_ptr<Mesh> m_cubeMesh;
    std::unique_ptr<Shader> m_defaultShader;
    std::unique_ptr<Shader> m_highlightShader;
	std::unique_ptr<Shader> m_shadowShader;

	glm::vec4 m_skyColor;

    bool m_initialized;

	unsigned int m_crosshairTexture;

	unsigned int m_depthMapFBO;
	unsigned int m_depthMap;

	glm::mat4 m_lightProjection;
	glm::mat4 m_lightView;
    glm::mat4 m_lightSpaceMatrix;
	glm::vec3 m_lightPos;
	glm::vec3 m_lightDir;

    float m_lightAzimuth = 0.0f;  
    float m_lightElevation = glm::radians(45.0f);
	float m_lightDistance = 300.0f;

	void initDepthMap();
	void initLighting();

	bool isDay();

	unsigned int loadTexture(const char* path);

	void renderShadowMap(const ChunkManager& chunkManager);

};

}; // namespace voxl