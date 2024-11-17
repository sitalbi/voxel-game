#include "mesh.h"
#include "glad/glad.h"
#include <iostream>

namespace voxl {

Mesh::Mesh()
{
    vertices = std::vector<glm::vec3>();
    normals = std::vector<glm::vec3>();
    indices = std::vector<unsigned int>();
}

Mesh::Mesh(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<unsigned int> indices)
{
    this->vertices = vertices;
    this->normals = normals;
    this->indices = indices;

    generateBuffers();
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &NBO);
    glDeleteVertexArrays(1, &VAO);
}

void Mesh::generateBuffers()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate and bind vertex buffer
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    // Set vertex attribute for position (location 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    // Generate and bind normal buffer
    glGenBuffers(1, &NBO);
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);

    // Set vertex attribute for normals (location 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    // Generate and bind index buffer
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Unbind VAO to prevent accidental modification
    glBindVertexArray(0);
}

} // namespace voxl