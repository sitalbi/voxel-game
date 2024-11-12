#include "renderer.h"
#include "cube.h"
#include "glad/glad.h"
#include <glm/ext/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

voxl::Renderer::Renderer() : m_initialized(false), m_vertexDataSize(g_VertexBufferData.size() * sizeof(float)), m_indexDataCount(g_IndexBufferData.size()), m_normalDataSize(g_NormalBufferData.size() * sizeof(float))
{
	m_vertexData = g_VertexBufferData.data();
	m_indexData = g_IndexBufferData.data();
	m_normalData = g_NormalBufferData.data();
	init();
}

voxl::Renderer::~Renderer()
{
	clear();
	delete m_shader;
}


void voxl::Renderer::init()
{
	if (m_initialized)
		return;
	m_shader = new Shader(RES_DIR "/shaders/defaultCubeVertex.glsl", RES_DIR "/shaders/defaultCubeFragment.glsl");

	m_textureIDs[BlockType::Grass] = loadTexture(RES_DIR"/textures/grass_top.png");
	m_textureIDs[BlockType::Dirt] = loadTexture(RES_DIR"/textures/dirt.png");
	m_textureIDs[BlockType::Sand] = loadTexture(RES_DIR"/textures/sand.png");
	m_textureIDs[BlockType::Stone] = loadTexture(RES_DIR"/textures/stone.png");
	m_textureIDs[BlockType::Wood] = loadTexture(RES_DIR"/textures/wood.png");

	setupCube();
	m_initialized = true;
}

void voxl::Renderer::renderCube(Cube& cube, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
	glBindVertexArray(m_VAO);
	m_shader->Bind();

	BlockType type = cube.getType();
	if (m_textureIDs.count(type) > 0) {
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[type]);
	}

	// Set uniforms
	m_shader->SetUniformMat4f("model", model);
	m_shader->SetUniformMat4f("view", view);
	m_shader->SetUniformMat4f("projection", projection);
	m_shader->SetUniform1i("u_Texture", 0);


	glDrawElements(GL_TRIANGLES, m_indexDataCount, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void voxl::Renderer::renderCubeWithCulling(Cube& cube, glm::mat4 model, glm::mat4 view, glm::mat4 projection,
	bool front, bool back, bool right, bool left, bool top, bool bottom)
{
	glBindVertexArray(m_VAO);
	m_shader->Bind();

	BlockType type = cube.getType();
	if (m_textureIDs.count(type) > 0) {
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[type]);
		m_shader->SetUniform1i("u_Texture", 0);

		// Set uniforms
		m_shader->SetUniformMat4f("model", model);
		m_shader->SetUniformMat4f("view", view);
		m_shader->SetUniformMat4f("projection", projection);

		// Render only the selected faces
		if (front) glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
		if (back) glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));
		if (right) glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(unsigned int)));
		if (left) glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(unsigned int)));
		if (top) glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(24 * sizeof(unsigned int)));
		if (bottom) glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30 * sizeof(unsigned int)));
	}
	glBindVertexArray(0);
}

void voxl::Renderer::renderChunk(Chunk& chunk, glm::mat4 view, glm::mat4 projection)
{
	for(int x = 0; x<Chunk::CHUNK_SIZE; x++)
	{
		for (int z = 0; z < Chunk::CHUNK_SIZE; z++)
		{
			for (int y = 0; y < Chunk::CHUNK_SIZE; y++)
			{
				Cube* cube = chunk.cubes[x][y][z];
				glm::vec3 pos = cube->getPosition();

				bool renderFront = chunk.isFaceVisible(pos + glm::vec3(0, 0, 1));
				bool renderBack = chunk.isFaceVisible(pos + glm::vec3(0, 0, -1));
				bool renderLeft = chunk.isFaceVisible(pos + glm::vec3(-1, 0, 0));
				bool renderRight = chunk.isFaceVisible(pos + glm::vec3(1, 0, 0));
				bool renderTop = chunk.isFaceVisible(pos + glm::vec3(0, 1, 0));
				bool renderBottom = chunk.isFaceVisible(pos + glm::vec3(0, -1, 0));

				glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
				//renderCubeWithCulling(*cube, model, view, projection, true, true, true, true, true, true);
				if (renderFront || renderBack || renderLeft || renderRight || renderTop || renderBottom)
				{
					renderCubeWithCulling(*cube, model, view, projection, renderFront, renderBack, renderRight, renderLeft, renderTop, renderBottom);
				}
			}
		}
	}
}

void voxl::Renderer::renderChunks(ChunkManager& chunkManager, glm::mat4 view, glm::mat4 projection)
{
	for (auto& chunk : chunkManager.getChunks())
	{
		renderChunk(*chunk.second, view, projection);
	}
}

void voxl::Renderer::clear()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_IBO);
	glDeleteProgram(m_shader->GetID());
}

void voxl::Renderer::setupCube()
{// Create vertex array
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	// Create and bind vertex buffer
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertexDataSize, m_vertexData, GL_STATIC_DRAW);

	// Vertex buffer layout
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	// Texture buffer layout
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));

	// Create and bind index buffer
	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexDataCount * sizeof(unsigned int), m_indexData, GL_STATIC_DRAW);

	// Create and bind normal buffer
	glGenBuffers(1, &m_NBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_NBO);
	glBufferData(GL_ARRAY_BUFFER, m_normalDataSize, m_normalData, GL_STATIC_DRAW);

	// Normal buffer layout
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);
}

unsigned int voxl::Renderer::loadTexture(const char* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data) {
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		stbi_image_free(data);
	}
	else {
		std::cout << "Failed to load texture !" << std::endl;
	}
	return textureID;
}
