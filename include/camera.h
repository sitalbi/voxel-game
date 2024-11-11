#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace voxl {

class Camera {
public:
	static const int width = 1920;
	static const int height = 1080;

	Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;

	void moveForward(float deltaTime);
	void moveBack(float deltaTime);
	void moveRight(float deltaTime);
	void moveLeft(float deltaTime);
	void moveUp(float deltaTime);
	void moveDown(float deltaTime);

	void processMouseMovement(float xoffset, float yoffset);

	void updateCameraVectors();

private:
	glm::vec3 m_position;
	glm::vec3 m_forward;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_worldUp;

	float m_yaw;
	float m_pitch;
	float m_speed;
};

} // namespace voxl