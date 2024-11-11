#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "renderer.h"
#include "shader.h"
#include "cube.h"
#include "camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

void mouseCallback(GLFWwindow* window, double xpos, double ypos);

int main() {
	
	GLFWwindow* window;

	if (!glfwInit()) {
		return -1;
	}

	window = glfwCreateWindow(voxl::Camera::width, voxl::Camera::height, "Voxel Game", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);


	// Load OpenGL functions
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	voxl::Renderer renderer;

	// Initialize matrices
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	voxl::Chunk chunk(0, 0, 0);
	chunk.generate();

	voxl::Chunk chunk2(0, 0, voxl::Chunk::CHUNK_SIZE);
	chunk2.generate();

	voxl::Chunk chunk3(0, 0, voxl::Chunk::CHUNK_SIZE * 2);
	chunk3.generate();

	voxl::Chunk chunk4(voxl::Chunk::CHUNK_SIZE, 0, 0);
	chunk4.generate();

	voxl::Chunk chunk5(voxl::Chunk::CHUNK_SIZE, 0, voxl::Chunk::CHUNK_SIZE);
	chunk5.generate();

	voxl::Chunk chunk6(voxl::Chunk::CHUNK_SIZE, 0, voxl::Chunk::CHUNK_SIZE * 2);
	chunk6.generate();

	voxl::Camera camera(glm::vec3(1.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
	glfwSetWindowUserPointer(window, &camera);

	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	float currentFrame = 0.0f;

	while (!glfwWindowShouldClose(window)) {
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderer.renderChunks(chunk, camera.getViewMatrix(), camera.getProjectionMatrix());
		renderer.renderChunks(chunk2, camera.getViewMatrix(), camera.getProjectionMatrix());
		renderer.renderChunks(chunk3, camera.getViewMatrix(), camera.getProjectionMatrix());
		renderer.renderChunks(chunk4, camera.getViewMatrix(), camera.getProjectionMatrix());
		renderer.renderChunks(chunk5, camera.getViewMatrix(), camera.getProjectionMatrix());
		renderer.renderChunks(chunk6, camera.getViewMatrix(), camera.getProjectionMatrix());

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera.moveForward(deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.moveBack(deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera.moveLeft(deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.moveRight(deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			camera.moveUp(deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
			camera.moveDown(deltaTime);
		}

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}

	}
	renderer.clear();
	glfwTerminate();
	return 0;
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	voxl::Camera* camera = static_cast<voxl::Camera*>(glfwGetWindowUserPointer(window));

	static double lastX = voxl::Camera::width / 2.0;
	static double lastY = voxl::Camera::height / 2.0;
	static bool firstMouse = true;

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; 
	lastX = xpos;
	lastY = ypos;

	camera->processMouseMovement(static_cast<float>(xoffset), static_cast<float>(yoffset));
}
