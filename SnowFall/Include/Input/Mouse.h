#pragma once
#include "System/WindowEvent.h"
#include "Math/Vector2.h"

struct MouseState
{
	Vector2 Position;
	Vector2 Raw;
	int  ScrollValue;
	bool LeftButton;
	bool MiddleButton;
	bool RightButton;
};

class Mouse
{
	static MouseState s_MouseState;

public:
	static MouseState GetState();
	static void HandleEvent(const WindowEvent& event);
};
