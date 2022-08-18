#include "Camera.h"

#include "Input.h"

Camera::Camera(const CameraSpecification& spec)
	: m_Position(spec.Position), m_Front({ 0.0f, 0.0f, -1.0f }), m_Up(glm::vec3(0.0f)), m_Right(glm::vec3(0.0f)), m_WorldUp(spec.WorldUp),
	m_Yaw(spec.Yaw), m_Pitch(spec.Pitch), m_Speed(spec.Speed), m_TurnSpeed(spec.TurnSpeed), m_LastMousePosition(glm::vec2(0.0f))
{
	Recalculate();
}

void Camera::OnUpdate(float deltaTime)
{
	float velocity = m_Speed * deltaTime;

	if (Input::IsKeyPressed(W))
		m_Position += m_Front * velocity;

	if (Input::IsKeyPressed(S))
		m_Position -= m_Front * velocity;

	if (Input::IsKeyPressed(A))
		m_Position -= m_Right * velocity;

	if (Input::IsKeyPressed(D))
		m_Position += m_Right * velocity;

	if (Input::IsKeyPressed(Space))
		m_Position += m_Up * velocity;

	if (Input::IsKeyPressed(LeftShift))
		m_Position += -m_Up * velocity;

	const glm::vec2& mousePos = Input::GetMousePosition();

	if (m_LastMousePosition != mousePos)
	{
		glm::vec2 delta = (mousePos - m_LastMousePosition) * 0.003f;
		delta *= m_TurnSpeed;
		m_Yaw += delta.x;
		m_Pitch += delta.y;
		m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);
		Recalculate();
	}

	m_LastMousePosition = mousePos;
}

glm::mat4 Camera::CalculateViewMatrix() const
{
	return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

void Camera::Recalculate()
{
	m_Front.x = glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
	m_Front.y = glm::sin(glm::radians(m_Pitch));
	m_Front.z = glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
	m_Front = glm::normalize(m_Front);

	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}
