#include "renderer.h"
#include "cube.h"
#include "glad/glad.h"
#include <glm/ext/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace voxl {

Renderer::Renderer() : m_initialized(false)
{
	init();
}

Renderer::~Renderer()
{
	clear();
	delete m_shader;
}


void Renderer::init()
{
	if (m_initialized)
		return;
	m_shader = new Shader(RES_DIR "/shaders/default_vert.glsl", RES_DIR "/shaders/default_frag.glsl");
	m_initialized = true;
}


void Renderer::renderChunk(Chunk& chunk, glm::mat4 view, glm::mat4 projection)
{	
	renderMesh(chunk.getMesh(), glm::translate(glm::mat4(1.0),chunk.getPosition()), view, projection);
}

void Renderer::renderChunks(const ChunkManager& chunkManager, glm::mat4 view, glm::mat4 projection)
{
	for (auto& chunk : chunkManager.getChunks())
	{
		renderChunk(*chunk.second, view, projection);
	}
}

void Renderer::renderMesh(Mesh* mesh, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
	glBindVertexArray(mesh->VAO);

	m_shader->Bind();


	// Set uniforms
	m_shader->SetUniformMat4f("model", model);
	m_shader->SetUniformMat4f("view", view);
	m_shader->SetUniformMat4f("projection", projection);

	glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
}

void Renderer::clear()
{
	glDeleteProgram(m_shader->GetID());
}

unsigned int Renderer::loadTexture(const char* path)
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

}; // namespace voxl