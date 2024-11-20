#include "camera.h"
#include "GLFW/glfw3.h"

namespace voxl {

Camera::Camera(int width, int height, glm::vec3 up, float yaw, float pitch)
{
	m_width = width;
	m_height = height;
	m_worldUp = up;
	m_yaw = yaw;
	m_pitch = pitch;

	updateCameraVectors();
}


glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(m_position, m_position + m_forward, m_up);
}

glm::mat4 Camera::getProjectionMatrix() const
{
	return glm::perspective(glm::radians(60.0f), (float)m_width / (float)m_height, m_nearClippingPlane, m_farClippingPlane);
}

glm::vec3 Camera::getPosition() const
{
	return m_position;
}

void Camera::setPosition(glm::vec3 position)
{
	m_position = position;
}

void Camera::processMouseMovement(float xoffset, float yoffset) {
	float sensitivity = 0.1f; 
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	m_yaw += xoffset;
	m_pitch += yoffset;

	if (m_pitch > 89.0f) m_pitch = 89.0f;
	if (m_pitch < -89.0f) m_pitch = -89.0f;

	updateCameraVectors();
}


void Camera::updateCameraVectors() {
	glm::vec3 front;
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_forward = glm::normalize(front);

	m_right = glm::normalize(glm::cross(m_forward, m_worldUp));
	m_up = glm::normalize(glm::cross(m_right, m_forward));
}

} // namespace voxl