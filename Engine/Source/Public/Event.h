#pragma once

enum class EventType
{
	KeyPressed,
	KeyReleased,
	MouseMovement,
	MouseScrolled
};

class Event
{
public:
	virtual ~Event() = default;
	virtual EventType GetEventType() const = 0;
};