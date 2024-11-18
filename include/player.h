#pragma once

#include "camera.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <chunk_manager.h>

namespace voxl {

    class Player {
    public:
        Player(glm::vec3 position, Camera& camera);

        void update(float deltaTime);
        void processInput(GLFWwindow* window, float deltaTime);
        void processMouseMovement(double xpos, double ypos);

        bool rayCast(const ChunkManager& chunkManager, float maxDistance, glm::vec3& outBlockPosition) const;

        glm::vec3 getPosition() const { return m_position; }

        const Camera& getCamera() const { return m_camera; }

    private:
        glm::vec3 m_position;
        glm::vec3 m_velocity;

        bool isSprinting = false;
        bool f1Pressed = false;
        bool wireframeMode = false;

        Camera& m_camera;
		float m_speed;
        float m_defaultSpeed = 10.0f;
        float m_speedMultiplier = 1.0f;  

        void updateCamera();
    };

} // namespace voxl

