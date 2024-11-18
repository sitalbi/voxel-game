#pragma once

#include "camera.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <chunk_manager.h>

namespace voxl {

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

    private:
        glm::vec3 m_position;
        glm::vec3 m_velocity;

		glm::vec3 m_blockPosition;
		glm::vec3 m_blockNormal;

        bool isSprinting = false;
        bool f1Pressed = false;
		bool mouseLeftClicked = false;
		bool mouseRightClicked = false;
        bool wireframeMode = false;
		bool m_blockFound = false;

        Camera& m_camera;
		float m_speed;
        float m_defaultSpeed = 10.0f;
        float m_speedMultiplier = 1.0f;  

		ChunkManager& m_chunkManager;

        void updateCamera();
    };

} // namespace voxl

