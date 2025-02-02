#include "renderer.h"

#include <glm/ext/matrix_transform.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "algorithm"

#define PI 3.14159265359

namespace voxl {

Renderer::Renderer() : m_initialized(false)
{
	init();
}

Renderer::~Renderer()
{
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

	// Load crosshair texture
	m_crosshairTexture = loadTexture(RES_DIR "/textures/gui/crosshair.png");

	m_defaultShader = std::make_unique<Shader>(RES_DIR "/shaders/default_vert.glsl", RES_DIR "/shaders/default_frag.glsl");
	m_highlightShader = std::make_unique<Shader>(RES_DIR "/shaders/highlight_vert.glsl", RES_DIR "/shaders/highlight_frag.glsl");
	m_shadowShader = std::make_unique<Shader>(RES_DIR "/shaders/shadow_vert.glsl", RES_DIR "/shaders/shadow_frag.glsl");
	generateCubeMesh();

	// OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_STENCIL_TEST);

	// Enable blending for transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_skyColor = glm::vec4(0.0f, 0.7f, 1.0f, 1.0f);

	m_defaultShader->Bind();
	/*m_defaultShader->SetUniform1f("fogStart", ChunkManager::LOAD_RADIUS * Chunk::CHUNK_SIZE - 10);
	m_defaultShader->SetUniform1f("fogEnd", ChunkManager::LOAD_RADIUS * Chunk::CHUNK_SIZE);
	m_defaultShader->SetUniform3f("fogColor", m_skyColor.x, m_skyColor.y, m_skyColor.z);*/

	initLighting();
	initDepthMap();

	m_initialized = true;
}

void Renderer::generateCubeMesh()
{
	m_cubeMesh = std::make_unique<Mesh>(g_cubeVertices, g_cubeNormals, g_cubeIndices, std::vector<glm::vec4>());
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

void Renderer::setupUI(Player& player, const glm::vec3& blockPos = glm::vec3(-10000.0f))
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	glm::vec3 playerPos = player.getPosition();

	// Information Panel
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_Always);

	ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	ImGui::Text("App average %.3f ms/frame (%.1f FPS)\n", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("");
	ImGui::Text("Player position: (%.2f, %.2f, %.2f)", playerPos.x, playerPos.y, playerPos.z);
	if (blockPos.y != -10000.0f) {
		ImGui::Text("Block position: (%.2f, %.2f, %.2f)", blockPos.x, blockPos.y, blockPos.z);
	}
	/*ImGui::Text("Light Azimuth: %.2f", m_lightAzimuth);
	ImGui::Text("Light Elevation: %.2f", m_lightElevation);*/
	ImGui::Text("Light Direction: (%.2f, %.2f, %.2f)", m_lightDir.x, m_lightDir.y, m_lightDir.z);
	ImGui::End();

	// Crosshair
	ImGui::SetNextWindowPos(ImVec2(window_width / 2.0f, window_height / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::Begin("Crosshair", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs);
	ImGui::Image((void*)(intptr_t)m_crosshairTexture, ImVec2(20, 20));
	ImGui::End();

	// Inventory Bar
	ImGui::SetNextWindowPos(ImVec2(window_width / 2.0f - 150, window_height - 80), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(300, 70), ImGuiCond_Always);

	ImGui::Begin("Inventory", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings);

	
	if (ImGui::BeginTable("InventoryTable", player.blockTypes.size(), ImGuiTableFlags_SizingFixedFit)) {

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec2 windowPos = ImGui::GetWindowPos();

		for (int i = 0; i < player.blockTypes.size(); ++i) {
			ImGui::TableNextColumn(); 

			ImGui::PushID(i);

			ImVec2 buttonPos = ImGui::GetCursorScreenPos();
			ImVec2 buttonSize = ImVec2(50, 50);
			ImVec2 buttonEnd = ImVec2(buttonPos.x + buttonSize.x, buttonPos.y + buttonSize.y);

			if (player.getSelectedBlock() == player.blockTypes[i]) {
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(255, 255, 255, 255));
			}
			draw_list->AddRect(buttonPos, buttonEnd, IM_COL32(0, 0, 0, 255), 0.0f, 0, 3.0f);

			ImVec4 color = ImVec4(g_cubeColors.at(player.blockTypes[i]).r, g_cubeColors.at(player.blockTypes[i]).g, g_cubeColors.at(player.blockTypes[i]).b, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_Button, color);
			ImGui::Button("", buttonSize);
			ImGui::PopStyleColor();

			ImGui::PopID();
		}

		ImGui::EndTable();  
	}

	ImGui::End();

}



void Renderer::update(Player& player, const ChunkManager& chunkManager)
{
	bool blockFound = player.blockFound();
	renderShadowMap(chunkManager);

	glClearColor(m_skyColor.r, m_skyColor.g, m_skyColor.b, m_skyColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	glStencilMask(0x00);
	renderChunks(chunkManager, player.getCamera().getViewMatrix(), player.getCamera().getProjectionMatrix());

	if (blockFound) {
		glm::vec3 blockPosition = player.getBlockPosition();
		setupUI(player, blockPosition);

		glDisable(GL_CULL_FACE);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		// Enable writing to the stencil buffer
		glStencilMask(0xFF); 

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		renderCube(voxl::BlockType::Grass, blockPosition, player.getCamera().getViewMatrix(), player.getCamera().getProjectionMatrix());
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

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
	else {
		setupUI(player);
	}

	renderUI();

	glfwSwapBuffers(window);
	glfwPollEvents();

}

void Renderer::renderCube(BlockType type, glm::vec3 position, glm::mat4 view, glm::mat4 projection)
{
	glm::mat4 scaledModel = glm::scale(glm::translate(glm::mat4(1.0f), position), glm::vec3(1.01f, 1.01f, 1.01f));
	
	renderMesh(*m_cubeMesh, *m_defaultShader, scaledModel, view, projection);
}


void Renderer::renderChunk(Chunk& chunk, glm::mat4 view, glm::mat4 projection, bool transparent)
{	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_depthMap);
	m_defaultShader->Bind();
	if (transparent) {
		if (chunk.getWaterMesh()) {
			m_defaultShader->SetUniformBool("useShadows", false);
			renderMesh(*chunk.getWaterMesh(), *m_defaultShader, glm::translate(glm::mat4(1.0), chunk.getPosition()), view, projection);
		}
	}
	else {
		m_defaultShader->SetUniformBool("useShadows", true);
		renderMesh(*chunk.getMesh(), *m_defaultShader, glm::translate(glm::mat4(1.0), chunk.getPosition()), view, projection);
	}
}

void Renderer::renderChunks(const ChunkManager& chunkManager, glm::mat4 view, glm::mat4 projection)
{
	for (auto& chunk : chunkManager.getChunks()) {
		renderChunk(*chunk.second, view, projection, false); // Render opaque
	}

	// Render transparent objects
	glDepthMask(GL_FALSE); 
	for (auto& chunk : chunkManager.getChunks()) {
		renderChunk(*chunk.second, view, projection, true); // Render transparent
	}
	glDepthMask(GL_TRUE); 
}

void Renderer::renderHighlight(glm::vec3 block, glm::mat4 view, glm::mat4 projection)
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), block);

	glm::mat4 scaleModel = glm::scale(model, glm::vec3(1.05f, 1.05f, 1.05f));

	renderMesh(*m_cubeMesh, *m_highlightShader, scaleModel, view, projection);
}


void Renderer::renderMesh(Mesh& mesh, Shader& shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
	glBindVertexArray(mesh.VAO);

	shader.Bind();

	// Set uniforms
	shader.SetUniformMat4f("model", model);
	shader.SetUniformMat4f("view", view);
	shader.SetUniformMat4f("projection", projection);
	if (shader.GetID() == m_defaultShader->GetID()) shader.SetUniformMat4f("lightSpaceMatrix", m_lightSpaceMatrix);


	glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, nullptr); 

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


void Renderer::initDepthMap()
{
	glGenFramebuffers(1, &m_depthMapFBO);

	glGenTextures(1, &m_depthMap);
	glBindTexture(GL_TEXTURE_2D, m_depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::initLighting()
{
	// Day/Night cycle parameters initialization
	m_cycleDuration = 30.0f; 
	m_minElevation = -1.5f; 
	m_maxElevation = 1.5f; 
	m_eastAzimuth = 0.0f; 
	m_westAzimuth = 2 * glm::pi<float>();

	float near_plane = 0.1f, far_plane = 300.0f;
	float shadowRadius = 100.0f;

	m_lightProjection = glm::ortho(-shadowRadius, shadowRadius, -shadowRadius, shadowRadius, near_plane, far_plane);

	// Define azimuth and elevation angles
	m_lightAzimuth = glm::pi<float>();
	m_lightElevation = -1.5f;

	// Calculate light direction
	m_lightDir.x = cos(m_lightElevation) * cos(m_lightAzimuth);
	m_lightDir.y = sin(m_lightElevation);
	m_lightDir.z = cos(m_lightElevation) * sin(m_lightAzimuth);

	glm::vec3 lightTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	m_lightPos = lightTarget + m_lightDir * m_lightDistance;

	m_lightView = glm::lookAt(m_lightPos, lightTarget, glm::vec3(0.0f, 1.0f, 0.0f));
	m_lightSpaceMatrix = m_lightProjection * m_lightView;

	m_defaultShader->Bind();
	m_defaultShader->SetUniform3f("lightDir", m_lightDir.x, m_lightDir.y, m_lightDir.z);
}

bool Renderer::isDay()
{
	return m_lightElevation >= 0.0f;
}


void Renderer::updateLighting(const glm::vec3& playerPosition, float deltaTime)
{
	

	static float elapsedTime = 0.0f;
	elapsedTime += deltaTime;
	float cycleProgress = fmod(elapsedTime, m_cycleDuration) / m_cycleDuration;

	// Update light elevation
	m_lightElevation = m_minElevation + (m_maxElevation - m_minElevation) * 0.5f * (1.0f + sin(2.0f * glm::pi<float>() * cycleProgress));

	// Update light azimuth
	m_lightAzimuth = m_eastAzimuth + (m_westAzimuth - m_eastAzimuth) * cycleProgress;

	// normalize light elevation todo try to smooth the day night transition
	float normalizedElevation = (m_lightElevation - m_minElevation) / (m_maxElevation - m_minElevation);
	if (!isDay()) {
		normalizedElevation = 0.0f;
	}

	float ambientLight = 0.25f * normalizedElevation;
	float lightColor = std::clamp(1.0f * normalizedElevation, 0.1f, 1.0f);

	m_skyColor = glm::vec4(0.1f, 0.7f, 1.0f, 1.0f);
	m_skyColor *= normalizedElevation;
	if (!isDay()) {
		m_skyColor = glm::vec4(0.1f, 0.7f, 1.0f, 1.0f) * 0.1f;
	}

	// Update light uniforms
	m_defaultShader->Bind();
	m_defaultShader->SetUniform3f("ambientLight", ambientLight, ambientLight, ambientLight);
	m_defaultShader->SetUniform3f("lightColor", lightColor, lightColor, lightColor);

	m_lightDir.x = cos(m_lightElevation) * cos(m_lightAzimuth);
	m_lightDir.y = abs(sin(m_lightElevation)); // Ensure light is always above the horizon
	m_lightDir.z = cos(m_lightElevation) * sin(m_lightAzimuth);

	m_defaultShader->SetUniform3f("lightDir", m_lightDir.x, m_lightDir.y, m_lightDir.z);
	if (m_lightElevation >= 0.0f) {
		m_defaultShader->SetUniformBool("isDay", true);
	}
	else {
		m_defaultShader->SetUniformBool("isDay", false);
	}

	// Update light position (relative to the player)
	glm::vec3 lightTarget = playerPosition;
	m_lightPos = playerPosition + m_lightDir * m_lightDistance;

	// Update shadow matrices
	m_lightView = glm::lookAt(m_lightPos, lightTarget, glm::vec3(0.0f, 1.0f, 0.0f));
	m_lightSpaceMatrix = m_lightProjection * m_lightView;

	// Pass updated matrices to the shadow shader
	m_shadowShader->Bind();
	m_shadowShader->SetUniformMat4f("lightSpaceMatrix", m_lightSpaceMatrix);
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

void Renderer::renderShadowMap(const ChunkManager& chunkManager)
{
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO); 
	glClear(GL_DEPTH_BUFFER_BIT); 

	// Front face culling to fix peter panning
	glCullFace(GL_FRONT);

	m_shadowShader.get()->Bind();

	// Render chunks to shadow map (TODO: optimization, only render chunk if it is within the shadow radius)
	for (auto& chunk : chunkManager.getChunks()) {
		Mesh& mesh = *chunk.second->getMesh();
		glBindVertexArray(mesh.VAO);

		// Set uniforms
		m_shadowShader.get()->SetUniformMat4f("model", glm::translate(glm::mat4(1.0), chunk.second->getPosition()));

		glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	glViewport(0, 0, window_width, window_height);
	// Back face culling again
	glCullFace(GL_BACK);
}

}; // namespace voxl