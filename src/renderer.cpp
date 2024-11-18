#include "renderer.h"

#include <glm/ext/matrix_transform.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace voxl {

Renderer::Renderer() : m_initialized(false)
{
	init();
}

Renderer::~Renderer()
{
	delete m_defaultShader;
	delete m_highlightShader;
	delete m_cubeMesh;
	glfwDestroyWindow(window);
}


void Renderer::init()
{
	if (m_initialized)
		return;

	if (!glfwInit()) {
		return;
	}

	window = glfwCreateWindow(window_width, window_height, "Voxel Game", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);

	// Load OpenGL functions
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return;
	}

	initUI();

	m_defaultShader = new Shader(RES_DIR "/shaders/default_vert.glsl", RES_DIR "/shaders/default_frag.glsl");
	m_highlightShader = new Shader(RES_DIR "/shaders/highlight_vert.glsl", RES_DIR "/shaders/highlight_frag.glsl");
	generateCubeMesh();

	// OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_STENCIL_TEST);

	// Load crosshair texture
	m_crosshairTexture = loadTexture(RES_DIR "/textures/gui/crosshair.png");

	m_highlightedBlock = glm::vec3(0.0f, 0.0f, 0.0f);

	m_initialized = true;
}

void Renderer::generateCubeMesh()
{
	m_cubeMesh = new Mesh(g_cubeVertices, g_cubeNormals, g_cubeIndices);
}

void Renderer::initUI() const
{
	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
}

void Renderer::setupUI(const glm::vec3& playerPos, const glm::vec3& blockPos)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_Always);

	ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	ImGui::Text("App average %.3f ms/frame (%.1f FPS)\n", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("");
	ImGui::Text("Player position: (%.2f, %.2f, %.2f)", playerPos.x, playerPos.y, playerPos.z);
	ImGui::Text("Block position: (%.2f, %.2f, %.2f)", blockPos.x, blockPos.y, blockPos.z);
	ImGui::End();

	// Crosshair
	ImGui::SetNextWindowPos(ImVec2(window_width / 2.0f, window_height / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::Begin("Crosshair", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs);
	ImGui::Image((void*)(intptr_t)m_crosshairTexture, ImVec2(20, 20));
	ImGui::End();
}

void Renderer::update(const Player& player, const ChunkManager& chunkManager)
{
	bool blockFound = player.blockFound();
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	setupUI(player.getPosition(), player.getBlockPosition());

	glEnable(GL_DEPTH_TEST);
	glStencilMask(0x00);
	renderChunks(chunkManager, player.getCamera().getViewMatrix(), player.getCamera().getProjectionMatrix());


	if (blockFound) {
		glm::vec3 blockPosition = player.getBlockPosition();
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		// Enable writing to the stencil buffer
		glStencilMask(0xFF); 

		renderCube(voxl::BlockType::Grass, blockPosition, player.getCamera().getViewMatrix(), player.getCamera().getProjectionMatrix());

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF); 
		glStencilMask(0x00); 
		glDepthMask(GL_FALSE); 
		glLineWidth(3);

		renderHighlight(blockPosition, player.getCamera().getViewMatrix(), player.getCamera().getProjectionMatrix());

		glDisable(GL_POLYGON_OFFSET_FILL);

		// Re-enable depth writing and stencil writing
		glDepthMask(GL_TRUE);
		glStencilMask(0xFF); 
		glStencilFunc(GL_ALWAYS, 1, 0xFF);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	renderUI();

	glfwSwapBuffers(window);
	glfwPollEvents();

}

void Renderer::renderCube(BlockType type, glm::vec3 position, glm::mat4 view, glm::mat4 projection)
{
	glm::mat4 scaledModel = glm::scale(glm::translate(glm::mat4(1.0f), position), glm::vec3(1.01f, 1.01f, 1.01f));
	renderMesh(m_cubeMesh, m_defaultShader, scaledModel, view, projection);
}


void Renderer::renderChunk(Chunk& chunk, glm::mat4 view, glm::mat4 projection)
{	
	renderMesh(chunk.getMesh(), m_defaultShader, glm::translate(glm::mat4(1.0),chunk.getPosition()), view, projection);
}

void Renderer::renderChunks(const ChunkManager& chunkManager, glm::mat4 view, glm::mat4 projection)
{
	for (auto& chunk : chunkManager.getChunks())
	{
		renderChunk(*chunk.second, view, projection);
	}
}

void Renderer::renderHighlight(glm::vec3 block, glm::mat4 view, glm::mat4 projection)
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), block);

	glm::mat4 scaleModel = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));

	renderMesh(m_cubeMesh, m_highlightShader, scaleModel, view, projection);
}


void Renderer::renderMesh(Mesh* mesh, Shader* shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
	glBindVertexArray(mesh->VAO);

	shader->Bind();

	// Set uniforms
	shader->SetUniformMat4f("model", model);
	shader->SetUniformMat4f("view", view);
	shader->SetUniformMat4f("projection", projection);
	
	glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, nullptr); 

	glBindVertexArray(0);
}

void Renderer::renderUI()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::clear()
{
	glDeleteProgram(m_defaultShader->GetID());
	glDeleteProgram(m_highlightShader->GetID());

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

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