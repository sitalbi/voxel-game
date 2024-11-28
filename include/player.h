#pragma once

#include "camera.h"
#include "cube.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <chunk_manager.h>
#include <memory>

namespace voxl {
class Player;
struct WindowContext {
    voxl::Camera* camera;
    voxl::Player* player;
};


class Player {
public:
    Player(glm::vec3 position, Camera& camera, ChunkManager& chunkManager);

    void update(float deltaTime);
    void processInput(GLFWwindow* window, float deltaTime);
    void processMouseMovement(double xpos, double ypos);

    bool rayCast(const ChunkManager& chunkManager, float maxDistance, glm::vec3& outBlockPosition, glm::vec3& outNormal) const;

    glm::vec3 getPosition() const { return m_position; }

    const Camera& getCamera() const { return m_camera; }

	glm::vec3 getBlockPosition() const { return m_blockPosition; }
    
	bool blockFound() const { return m_blockFound; }

	BlockType getSelectedBlock() const { return blockTypes[m_selectedBlock]; }

    const std::vector<BlockType> blockTypes = {
        BlockType::Grass,
        BlockType::Dirt,
        BlockType::Stone,
        BlockType::Sand,
        BlockType::Wood
    };

    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        // Retrieve the WindowContext instance from the user pointer
        WindowContext* context = static_cast<WindowContext*>(glfwGetWindowUserPointer(window));
        if (context && context->player) {
            context->player->handle_scroll(xoffset, yoffset);
        }
    }

    void handle_scroll(double xoffset, double yoffset);


private:
    glm::vec3 m_position;
	glm::vec3 m_direction;
	glm::vec3 m_cameraPosition;
    glm::vec3 m_velocity;
	glm::vec3 m_playerForward;

	glm::vec3 m_blockPosition;
	glm::vec3 m_blockNormal;

    bool isSprinting = false;
    bool f1Pressed = false;
	bool f2Pressed = false;
	bool mouseLeftClicked = false;
	bool mouseRightClicked = false;
    bool wireframeMode = false;
	bool m_blockFound = false;
	bool m_isGrounded = true;

	bool m_isFlying = true;

	float m_speed;
    float m_defaultSpeed = 5.0f;
	float m_speedMultiplier = 1.0f;
    float m_defaultSpeedMultiplier = 1.0f;  
	float m_gravity = -15.0f;
	float m_verticalVelocity = 0.0f;

	float m_height = 1.5f;
	float m_width = 0.25f;

    Camera& m_camera;
	ChunkManager& m_chunkManager;

	int m_selectedBlock = 0;

    void updateCamera();

	void handleCollisions(float dx, float dy, float dz);
};


} // namespace voxl

