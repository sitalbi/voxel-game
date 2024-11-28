#include <iostream>

#include "renderer.h"
#include "shader.h"
#include "cube.h"
#include "camera.h"
#include "mesh.h"
#include "chunk_manager.h"
#include "player.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

void mouseCallback(GLFWwindow* window, double xpos, double ypos);

int main() {
	// Initialization
	voxl::Renderer renderer;
	voxl::ChunkManager chunkManager;

	voxl::Camera camera(window_width, window_height, glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

	voxl::Player player(glm::vec3(0.0f, 20.0f, 2.0f), camera, chunkManager);
	

	// Window settings
	glfwSetCursorPosCallback(renderer.window, mouseCallback);
	glfwSetInputMode(renderer.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	voxl::WindowContext context = { &camera, &player };
	glfwSetWindowUserPointer(renderer.window, &context);

	glfwSetScrollCallback(renderer.window, player.scroll_callback);

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	float currentFrame = 0.0f;

	while (!glfwWindowShouldClose(renderer.window)) {
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Player update
		player.update(deltaTime);

		// ChunkManager update
		chunkManager.updateChunks(player.getPosition());

		// Renderer update
		renderer.update(player, chunkManager);

		
	}
	renderer.clear();
	return 0;
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	voxl::WindowContext* context = static_cast<voxl::WindowContext*>(glfwGetWindowUserPointer(window));
	voxl::Camera* camera = context->camera;

	static double lastX = window_width / 2.0;
	static double lastY = window_height / 2.0;
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
