#pragma once

#include "glad/glad.h"
#include "shader.h"
#include <vector>
#include <string>



namespace voxl {
class Skybox
{
public:
	Skybox();
	~Skybox();

	unsigned int getTextureID() const { return m_textureID; }
	unsigned int getVAO() const { return m_VAO; }

private:
	unsigned int m_VAO, m_VBO;
	unsigned int m_textureID;

	void init();

	unsigned int loadCubemap();


};

}; // namespace voxl