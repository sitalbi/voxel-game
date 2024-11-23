#include "glad/glad.h"
#include "player.h"
#include "chunk.h"
#include <iostream>

namespace voxl {

Player::Player(glm::vec3 position, Camera& camera, ChunkManager& chunkManager) : m_position(position), m_camera(camera), m_chunkManager(chunkManager)
{
	m_speed = m_defaultSpeed;
	m_blockPosition = glm::vec3(0.0f);
	updateCamera();
}

void Player::update(float deltaTime) {
    m_blockFound = rayCast(m_chunkManager, 10.0f, m_blockPosition, m_blockNormal);

	processInput(glfwGetCurrentContext(), deltaTime);

    if (isSprinting) {
        m_speedMultiplier = 3.0f;
    }
    else {
        m_speedMultiplier = 1.0f;
    }

	m_position += m_velocity * deltaTime;

	updateCamera();

}

void Player::processInput(GLFWwindow* window, float deltaTime) {
	m_velocity = glm::vec3(0.0f);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        m_velocity += m_camera.getForward() * m_speed * m_speedMultiplier;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        m_velocity -= m_camera.getForward() * m_speed * m_speedMultiplier;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        m_velocity -= m_camera.getRight() * m_speed * m_speedMultiplier;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        m_velocity += m_camera.getRight() * m_speed * m_speedMultiplier;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        m_velocity += m_camera.getUp() * m_speed * m_speedMultiplier;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        m_velocity -= m_camera.getUp() * m_speed * m_speedMultiplier;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        isSprinting = true;
    }
    else {
        isSprinting = false;
    }

    // If mouse left is pressed
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!mouseLeftClicked) {
            if (m_blockFound) {
                glm::vec3 newBlockPosition = m_blockPosition + m_blockNormal;

                Chunk* chunk = m_chunkManager.getChunk(newBlockPosition.x, newBlockPosition.y, newBlockPosition.z);

                if (chunk != nullptr) {
                    int blockX = glm::mod(glm::floor(newBlockPosition.x), static_cast<float>(Chunk::CHUNK_SIZE));
                    int blockY = glm::mod(glm::floor(newBlockPosition.y), static_cast<float>(Chunk::CHUNK_HEIGHT));
                    int blockZ = glm::mod(glm::floor(newBlockPosition.z), static_cast<float>(Chunk::CHUNK_SIZE));

                    glm::ivec3 localBlockPos = glm::ivec3(blockX, blockY, blockZ);
                    if (localBlockPos.x >= 0 && localBlockPos.y >= 0 && localBlockPos.z >= 0 &&
                        localBlockPos.x < Chunk::CHUNK_SIZE && localBlockPos.y < Chunk::CHUNK_HEIGHT && localBlockPos.z < Chunk::CHUNK_SIZE) {
                        chunk->setBlockType(localBlockPos.x, localBlockPos.y, localBlockPos.z, getSelectedBlock());

                        m_chunkManager.updateChunk(chunk);
                    }
                }
            }
			mouseLeftClicked = true;
        } 
	}
	else {
		mouseLeftClicked = false;
	}

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        if (!mouseRightClicked) {
            if (m_blockFound) {
                Chunk* chunk = m_chunkManager.getChunk(m_blockPosition.x, m_blockPosition.y, m_blockPosition.z);

                if (chunk != nullptr) {
                    int blockX = glm::mod(glm::floor(m_blockPosition.x), static_cast<float>(Chunk::CHUNK_SIZE));
                    int blockY = glm::mod(glm::floor(m_blockPosition.y), static_cast<float>(Chunk::CHUNK_HEIGHT));
                    int blockZ = glm::mod(glm::floor(m_blockPosition.z), static_cast<float>(Chunk::CHUNK_SIZE));

                    glm::ivec3 localBlockPos = glm::ivec3(blockX, blockY, blockZ);
                    if (localBlockPos.x >= 0 && localBlockPos.y >= 0 && localBlockPos.z >= 0 &&
                        localBlockPos.x < Chunk::CHUNK_SIZE && localBlockPos.y < Chunk::CHUNK_HEIGHT && localBlockPos.z < Chunk::CHUNK_SIZE) {
                        chunk->setBlockType(localBlockPos.x, localBlockPos.y, localBlockPos.z, BlockType::None);
                        //chunk->generateMesh();
						m_chunkManager.updateChunk(chunk);
                    }
                }
            }
            mouseRightClicked = true;
        }
    }
    else {
        mouseRightClicked = false;
    }
	


    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
        if (!f1Pressed) {
            wireframeMode = !wireframeMode;
            if (wireframeMode) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDisable(GL_CULL_FACE);
            }
            else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glEnable(GL_CULL_FACE);
            }
            f1Pressed = true;
        }
    }
    else {
        f1Pressed = false;
    }

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void Player::processMouseMovement(double xpos, double ypos) {
    m_camera.processMouseMovement(static_cast<float>(xpos), static_cast<float>(ypos));
}

bool Player::rayCast(const ChunkManager& chunkManager, float maxDistance, glm::vec3& outBlockPosition, glm::vec3& outNormal) const
{
	std::unordered_set<BlockType> nonSelectableBlockTypes = { BlockType::Water, BlockType::None };
    glm::vec3 rayOrigin = m_camera.getPosition();
    glm::vec3 rayDirection = glm::normalize(m_camera.getForward());
    float step = 0.1f;

    glm::vec3 currentPos;
    for (float distance = 0.0f; distance < maxDistance; distance += step) {
        currentPos = rayOrigin + rayDirection * distance;

        // Check if the chunk exists
        Chunk* chunk = chunkManager.getChunk(currentPos.x, currentPos.y, currentPos.z);
        if (chunk == nullptr) {
            continue;
        }

        // Get the local block position within the chunk
		int blockX = glm::mod(glm::floor(currentPos.x), static_cast<float>(Chunk::CHUNK_SIZE));
		int blockY = glm::mod(glm::floor(currentPos.y), static_cast<float>(Chunk::CHUNK_HEIGHT));
		int blockZ = glm::mod(glm::floor(currentPos.z), static_cast<float>(Chunk::CHUNK_SIZE));

		glm::ivec3 blockPos = glm::ivec3(blockX, blockY, blockZ);

        // Check if there is a block at this position
        if (nonSelectableBlockTypes.find(chunk->cubes[blockPos.x][blockPos.y][blockPos.z]) == nonSelectableBlockTypes.end()) {
            // Determine which face is hit based on ray direction
            glm::vec3 blockCenter = chunk->getPosition() + glm::vec3(blockPos) + glm::vec3(0.5f);
            glm::vec3 delta = currentPos - blockCenter;

            if (fabs(delta.x) > fabs(delta.y) && fabs(delta.x) > fabs(delta.z)) {
                outNormal = glm::vec3(delta.x > 0 ? 1.0f : -1.0f, 0.0f, 0.0f);
            }
            else if (fabs(delta.y) > fabs(delta.x) && fabs(delta.y) > fabs(delta.z)) {
                outNormal = glm::vec3(0.0f, delta.y > 0 ? 1.0f : -1.0f, 0.0f);
            }
            else {
                outNormal = glm::vec3(0.0f, 0.0f, delta.z > 0 ? 1.0f : -1.0f);
            }

            outBlockPosition = blockCenter;
            return true;
        }
    }
    return false;
}





void Player::handle_scroll(double xoffset, double yoffset)
{
    if (yoffset < 0) {
		m_selectedBlock++;
		if (m_selectedBlock >= blockTypes.size()) {
			m_selectedBlock = 0;
		}
	}
    if (yoffset > 0) {
		m_selectedBlock--;
		if (m_selectedBlock < 0) {
			m_selectedBlock = blockTypes.size() - 1;
		}
	}
}

void Player::updateCamera() {
	m_camera.setPosition(m_position);
}

} // namespace voxl
