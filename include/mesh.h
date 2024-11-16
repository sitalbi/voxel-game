#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace voxl {

class Mesh {

public:
	Mesh();
	Mesh(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<unsigned int> indices);
	~Mesh();

	unsigned int VAO, VBO, EBO, NBO;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;



	void generateBuffers();

};

} // namespace voxl
