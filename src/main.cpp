#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "renderer.h"
#include "shader.h"
#include "cube.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define width 1280
#define height 720

int main() {
	
	GLFWwindow* window;

	if (!glfwInit()) {
		return -1;
	}

	window = glfwCreateWindow(width, height, "Voxel Game", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Load OpenGL functions
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return -1;
	}

	// Enable depth 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	voxl::Renderer renderer;
	renderer.init();

	// Initialize matrices
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 view = glm::lookAt(glm::vec3(3.0f, 1.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::perspective(glm::radians(30.0f), (float)width / (float)height, 0.1f, 500.0f);

	voxl::Cube cube(voxl::BlockType::Dirt);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderer.renderCube(cube, model, view, projection);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	renderer.clear();
	glfwTerminate();
	return 0;
}