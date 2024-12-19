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
	m_playerForward = camera.getForward();
	m_playerUp = camera.getUp();
	m_isFlying = true;
    if (m_isFlying) {
        m_speed = m_defaultSpeed * 2.0f;
        m_speedMultiplier = m_defaultSpeedMultiplier * 2.0f;
        m_defaultSpeedMultiplier = 2.0f;
    }
    else {
        m_speed = m_defaultSpeed;
        m_speedMultiplier = m_defaultSpeedMultiplier;
        m_defaultSpeedMultiplier = 1.0f;
    }
}

void Player::update(float deltaTime) {
    m_blockFound = rayCast(m_chunkManager, 10.0f, m_blockPosition, m_blockNormal);

    // Process user input to update velocity
    processInput(glfwGetCurrentContext(), deltaTime);

    if (!m_isFlying) {
        // Apply gravity if not grounded
        if (!m_isGrounded) {
			m_verticalVelocity += m_gravity * deltaTime;
        }
        else {
            m_verticalVelocity = 0.0f;
        }

        m_velocity.y = m_verticalVelocity;
    }

    if (isSprinting) {
		m_speedMultiplier = m_defaultSpeedMultiplier * 1.5f;
    }
    else {
        m_speedMultiplier = m_defaultSpeedMultiplier;
    }

    m_position.x += m_velocity.x * deltaTime;
	if (!m_isFlying) handleCollisions(m_velocity.x, 0.0f, 0.0f);
	m_position.y += m_velocity.y * deltaTime;
    if (!m_isFlying) handleCollisions(0.0f, m_velocity.y, 0.0f);
	m_position.z += m_velocity.z * deltaTime;
    if (!m_isFlying) handleCollisions(0.0f, 0.0f, m_velocity.z);

    // Update camera position
    updateCamera();

    // Update forward direction of the player
    m_playerForward = glm::normalize(glm::vec3(m_camera.getForward().x, 0.0f, m_camera.getForward().z));
}


void Player::processInput(GLFWwindow* window, float deltaTime) {
	m_velocity = glm::vec3(0.0f);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        m_velocity += m_playerForward * m_speed * m_speedMultiplier;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		m_velocity -= m_playerForward * m_speed * m_speedMultiplier;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		m_velocity -= m_camera.getRight() * m_speed * m_speedMultiplier;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        m_velocity += m_camera.getRight() * m_speed * m_speedMultiplier;
    }

    if (m_isFlying) {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			m_velocity += m_playerUp * m_speed * m_speedMultiplier;
        }
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
			m_velocity -= m_playerUp * m_speed * m_speedMultiplier;
		}
    }
    else {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && m_isGrounded) {
            m_verticalVelocity = 6.5f;
            m_isGrounded = false;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        isSprinting = true;
    }
    else {
        isSprinting = false;
    }

    // If mouse left is pressed
    onPressedMouse(GLFW_MOUSE_BUTTON_LEFT, [&]() {
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
        });

	// If mouse right is pressed
    onPressedMouse(GLFW_MOUSE_BUTTON_RIGHT, [&]() {
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

                    m_chunkManager.updateChunk(chunk);
                }
            }
        }
		});



    onPressedKey(GLFW_KEY_F1, [&]() {
        wireframeMode = !wireframeMode;
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_CULL_FACE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_CULL_FACE);
        }
    });

    onPressedKey(GLFW_KEY_F2, [&]() {
        m_isFlying = !m_isFlying;
        if (m_isFlying) {
            m_speed = m_defaultSpeed * 2.0f;
            m_speedMultiplier = m_defaultSpeedMultiplier * 2.0f;
            m_defaultSpeedMultiplier = 2.0f;
        }
        else {
            m_speed = m_defaultSpeed;
            m_speedMultiplier = m_defaultSpeedMultiplier;
            m_defaultSpeedMultiplier = 1.0f;
        }
        });


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
    m_cameraPosition = glm::vec3(m_position.x, m_position.y + m_height, m_position.z);
	m_camera.setPosition(m_cameraPosition);
}

// AABB collision detection
void Player::handleCollisions(float dx, float dy, float dz)
{
	// normalize the direction
	glm::vec3 direction = glm::normalize(glm::vec3(dx, dy, dz));

    float x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4, y5;
	x1 = glm::floor(m_position.x - m_width);
	y1 = glm::floor(m_position.y);
	z1 = glm::floor(m_position.z - m_width);
	x2 = glm::floor(m_position.x + m_width);
	y2 = glm::floor(m_position.y + m_height);
	z2 = glm::floor(m_position.z + m_width);

    // Stuck check
    while (m_chunkManager.isSolidBlock(floor(m_position.x), floor(m_position.y), floor(m_position.z))) {
        m_position.y += 1.0f;
        y1 = floor(m_position.y - m_position.y);
        y2 = floor((m_position.y + (m_height + m_height + 2)));
        if (floor(m_position.y) > Chunk::CHUNK_HEIGHT) break;
    }

    // Vertical collision
    // down
    if (direction.y != 0) {
        if (m_chunkManager.isSolidBlock(x1, y1, z1) || m_chunkManager.isSolidBlock(x2, y1, z1) ||
            m_chunkManager.isSolidBlock(x2, y1, z2) || m_chunkManager.isSolidBlock(x1, y1, z2)) {
            if (direction.y < 0) {
                m_position.y = y1 + 1;
                m_velocity.y = 0.0f;
                m_isGrounded = true;
            }
        }
        else {
            m_isGrounded = false;
        }
    }

    // up
	if (m_chunkManager.isSolidBlock(x1, y2, z1) || m_chunkManager.isSolidBlock(x2, y2, z1) ||
		m_chunkManager.isSolidBlock(x2, y2, z2) || m_chunkManager.isSolidBlock(x1, y2, z2)) {
		if (direction.y > 0) {
			m_position.y = y2 - m_height - 0.01f;
			m_velocity.y = 0.0f;
		}
	}

	// Horizontal collision
	x3 = glm::floor(m_position.x - direction.x - m_width);
	y3 = glm::floor(m_position.y - direction.y);
	z3 = glm::floor(m_position.z - direction.z - m_width);
	x4 = glm::floor(m_position.x - direction.x + m_width);
	y4 = glm::floor(m_position.y - direction.y + m_height);
	z4 = glm::floor(m_position.z - direction.z + m_width);
	y5 = glm::round(y3 + (y4 - y3) / 2); // previous y position
	
	// right
	if (m_chunkManager.isSolidBlock(x1, y3, z3) || m_chunkManager.isSolidBlock(x1, y3, z4) ||
		m_chunkManager.isSolidBlock(x1, y4, z3) || m_chunkManager.isSolidBlock(x1, y4, z4) ||
        m_chunkManager.isSolidBlock(x1, y5, z3) || m_chunkManager.isSolidBlock(x1, y5, z4)) {
		
        if (direction.x < 0) {
            m_position.x = x3 + m_width;
            m_velocity.x = 0.0f;
        }
	}

	// left
    if (m_chunkManager.isSolidBlock(x2, y3, z3) || m_chunkManager.isSolidBlock(x2, y3, z4) ||
        m_chunkManager.isSolidBlock(x2, y4, z3) || m_chunkManager.isSolidBlock(x2, y4, z4) ||
        m_chunkManager.isSolidBlock(x2, y5, z3) || m_chunkManager.isSolidBlock(x2, y5, z4)) {

        if (direction.x > 0) {
            m_position.x = (x4+1) - m_width - 0.01f;
            m_velocity.x = 0.0f;
        }
    }

	// forward
    if (m_chunkManager.isSolidBlock(x3, y3, z1) || m_chunkManager.isSolidBlock(x4, y3, z1) ||
        m_chunkManager.isSolidBlock(x4, y4, z1) || m_chunkManager.isSolidBlock(x3, y4, z1) ||
        m_chunkManager.isSolidBlock(x3, y5, z1) || m_chunkManager.isSolidBlock(x4, y5, z1)) {

        if (direction.z < 0) {
            m_position.z = z3 + m_width;
            m_velocity.z = 0.0f;
        }
    }

	// backward
    if (m_chunkManager.isSolidBlock(x3, y3, z2) || m_chunkManager.isSolidBlock(x4, y3, z2) ||
        m_chunkManager.isSolidBlock(x4, y4, z2) || m_chunkManager.isSolidBlock(x3, y4, z2) ||
        m_chunkManager.isSolidBlock(x3, y5, z2) || m_chunkManager.isSolidBlock(x4, y5, z2)) {

        if (direction.z > 0) {
            m_position.z = (z4+1) - m_width - 0.01f;
            m_velocity.z = 0.0f;
        }
    }
}

void Player::onPressedMouse(int button, const std::function<void()>& callback)
{
	bool isPressed = glfwGetMouseButton(glfwGetCurrentContext(), button) == GLFW_PRESS;
	if (isPressed && !m_mouseButtonStates[button]) {
		callback();
	}
	m_mouseButtonStates[button] = isPressed;
}

void Player::onPressedKey(int key, const std::function<void()>& callback)
{
    bool isPressed = glfwGetKey(glfwGetCurrentContext(), key) == GLFW_PRESS;
    if (isPressed && !m_keyStates[key]) {
        callback();
    }
    m_keyStates[key] = isPressed;

}

} // namespace voxl
