#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace voxl {

class Mesh {

public:
	Mesh();
	Mesh(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<unsigned int> indices, std::vector<glm::vec4> colors);
	~Mesh();

	unsigned int VAO, VBO, EBO, NBO, CBO;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec4> colors;
	std::vector<unsigned int> indices;

	void generateBuffers();

	void setColors(std::vector<glm::vec4> colors);

};

} // namespace voxl
