#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct CameraSpecification
{
	glm::vec3 Position;
	glm::vec3 WorldUp;
	float Yaw;
	float Pitch;
	float Speed;
	float TurnSpeed;
};

class Camera
{
public:
	Camera(const CameraSpecification& spec);
	~Camera() = default;

	void OnUpdate(float deltaTime);

	glm::vec3 GetPosition() const { return m_Position; }
	glm::vec3 GetDirection() const { return glm::normalize(m_Front); }

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
