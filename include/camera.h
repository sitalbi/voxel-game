#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace voxl {

class Camera {
public:
	Camera(int width, int height, glm::vec3 up, float yaw, float pitch);
	Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;
	glm::vec3 getPosition() const;

	glm::vec3 getForward() const { return m_forward; }
	glm::vec3 getUp() const { return m_up; }
	glm::vec3 getRight() const { return m_right; }


	void setPosition(glm::vec3 position);

	void processMouseMovement(float xoffset, float yoffset);

	void updateCameraVectors();

private:
	int m_width, m_height;

	glm::vec3 m_position;
	glm::vec3 m_forward;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_worldUp;

	float m_yaw;
	float m_pitch;

	float m_farClippingPlane = 700.0f;
	float m_nearClippingPlane = 0.1f;
};

} // namespace voxl