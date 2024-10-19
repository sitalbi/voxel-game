#include "camera.h"
#include "GLFW/glfw3.h"

voxl::Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
{
	m_position = position;
	m_worldUp = up;
	m_yaw = yaw;
	m_pitch = pitch;

	m_speed = 5.0f;

	updateCameraVectors();
}

glm::mat4 voxl::Camera::getViewMatrix() const
{
	return glm::lookAt(m_position, m_position + m_forward, m_up);
}

void voxl::Camera::moveForward(float deltaTime)
{
	m_position += m_forward * m_speed *  deltaTime;
}

void voxl::Camera::moveBack(float deltaTime)
{
	m_position -= m_forward * m_speed * deltaTime;
}

void voxl::Camera::moveRight(float deltaTime)
{
	m_position += m_right * m_speed * deltaTime;
}

void voxl::Camera::moveLeft(float deltaTime)
{
	m_position -= m_right * m_speed * deltaTime;
}

void voxl::Camera::moveUp(float deltaTime)
{
	m_position += m_up * m_speed * deltaTime;
}

void voxl::Camera::moveDown(float deltaTime)
{
	m_position -= m_up * m_speed * deltaTime;
}

void voxl::Camera::processMouseMovement(float xoffset, float yoffset) {
	float sensitivity = 0.1f; 
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	m_yaw += xoffset;
	m_pitch += yoffset;

	if (m_pitch > 89.0f) m_pitch = 89.0f;
	if (m_pitch < -89.0f) m_pitch = -89.0f;

	updateCameraVectors();
}


void voxl::Camera::updateCameraVectors() {
	glm::vec3 front;
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_forward = glm::normalize(front);

	m_right = glm::normalize(glm::cross(m_forward, m_worldUp));
	m_up = glm::normalize(glm::cross(m_right, m_forward));
}



