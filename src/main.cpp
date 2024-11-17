#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "renderer.h"
#include "shader.h"
#include "cube.h"
#include "camera.h"
#include "mesh.h"
#include "chunk_manager.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <player.h>

#define window_width 1920
#define window_height 1080

void mouseCallback(GLFWwindow* window, double xpos, double ypos);

int main() {
	
	GLFWwindow* window;

	if (!glfwInit()) {
		return -1;
	}

	window = glfwCreateWindow(window_width, window_height, "Voxel Game", NULL, NULL);
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
	glCullFace(GL_BACK);

	// Initialization
	voxl::Renderer renderer;

	voxl::Camera camera(window_width, window_height, glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

	voxl::Player player(glm::vec3(0.0f, 30.0f, 0.0f), camera);
	
	voxl::ChunkManager chunkManager;

	// Window settings
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowUserPointer(window, &camera);

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

		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_Always); 

		ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
		ImGui::Text("App average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		// ==============================================

		// Player update
		player.processInput(window, deltaTime);
		player.update(deltaTime);

		// ChunkManager update
		chunkManager.updateChunks(player.getPosition());

		// Rendering
		renderer.renderChunks(chunkManager, player.getCamera().getViewMatrix(), player.getCamera().getProjectionMatrix());

		// UI Rendering =================================
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		// ==============================================

		glfwSwapBuffers(window);
		glfwPollEvents();
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
