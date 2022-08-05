#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch, float speed, float turnSpeed);
	~Camera() = default;

	void OnUpdate(float deltaTime);

	glm::mat4 CalculateViewMatrix() const;

private:
	void Recalculate();

private:
	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_Up;
	glm::vec3 m_Right;
	glm::vec3 m_WorldUp;

	float m_Yaw, m_Pitch;
	float m_Speed, m_TurnSpeed;

	glm::vec2 m_LastMousePosition;
};
