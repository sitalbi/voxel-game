#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "renderer.h"
#include "shader.h"
#include "cube.h"
#include "camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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

	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();



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
	

	voxl::Camera camera(glm::vec3(1.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
	glfwSetWindowUserPointer(window, &camera); 
	
	// ChunkManager
	voxl::ChunkManager chunkManager;
	chunkManager.loadChunks(camera.getPosition());

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

		// UI ==========================================
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Set window position and size
		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always); // Top left with a 10-pixel offset
		ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_Always); // Set size to 300x100 pixels

		// Create an ImGui window with no title bar, resize, or move
		ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
		ImGui::Text("App average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		// ==============================================

		// Rendering
		renderer.renderChunks(chunkManager, camera.getViewMatrix(), camera.getProjectionMatrix());

		// UI Rendering =================================
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		// ==============================================

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
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
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
