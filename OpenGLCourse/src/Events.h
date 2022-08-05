#pragma once

#include "KeyCodes.h"
#include "MouseCodes.h"


enum class EventType
{
	KeyPressed,
	KeyReleased,
	MouseButtonPressed,
	MouseButtonReleased,
	MouseScrolled,
	MouseMoved
};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\

class Event
{
public:
	virtual ~Event() = default;

	bool Handled = false;

	virtual EventType GetEventType() const = 0;
};

class EventDispatcher
{
public:
	EventDispatcher(Event& event)
		: m_Event(event) {}

	template<typename T, typename F>
	bool Dispatch(const F& func)
	{
		if (m_Event.GetEventType() == T::GetStaticType())
		{
			m_Event.Handled |= func(static_cast<T&>(m_Event));
			return true;
		}

		return false;
	}

private:
	Event& m_Event;
};

class KeyPressedEvent : public Event
{
public:
	KeyPressedEvent(KeyCode key, bool repeated)
		: m_Key(key), m_Repeated(repeated) {}

	KeyCode GetKey() const { return m_Key; }
	bool IsRepeated() const { return m_Repeated; }

	EVENT_CLASS_TYPE(KeyPressed)

private:
	KeyCode m_Key;
	bool m_Repeated;
};

class KeyReleasedEvent : public Event
{
public:
	KeyReleasedEvent(KeyCode key)
		: m_Key(key) {}

	KeyCode GetKey() const { return m_Key; }

	EVENT_CLASS_TYPE(KeyReleased)

private:
	KeyCode m_Key;
};

class MouseButtonPressedEvent : public Event
{
public:
	MouseButtonPressedEvent(MouseCode button)
		: m_Button(button) {}

	MouseCode GetMouseButton() const { return m_Button; }

	EVENT_CLASS_TYPE(MouseButtonPressed)

private:
	MouseCode m_Button;
};

class MouseButtonReleasedEvent : public Event
{
public:
	MouseButtonReleasedEvent(MouseCode button)
		: m_Button(button) {}

	MouseCode GetMouseButton() const { return m_Button; }

	EVENT_CLASS_TYPE(MouseButtonReleased)

private:
	MouseCode m_Button;
};

class MouseScrolledEvent : public Event
{
public:
	MouseScrolledEvent(float offsetX, float offsetY)
		: m_OffsetX(offsetX), m_OffsetY(offsetY) {}

	float GetOffSetX() const { return m_OffsetX; }
	float GetOffSetY() const { return m_OffsetY; }

	EVENT_CLASS_TYPE(MouseScrolled)

private:
	float m_OffsetX, m_OffsetY;
};

class MouseMovedEvent : public Event
{
public:
	MouseMovedEvent(float x, float y)
		: m_MouseX(x), m_MouseY(y) {}

	float GetX() const { return m_MouseX; }
	float GetY() const { return m_MouseY; }

	EVENT_CLASS_TYPE(MouseMoved)

private:
	float m_MouseX, m_MouseY;
};