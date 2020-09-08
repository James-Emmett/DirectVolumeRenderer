#include "Input/Mouse.h"

MouseState Mouse::s_MouseState;

MouseState Mouse::GetState()
{
	return s_MouseState;
}

void Mouse::HandleEvent(const WindowEvent& event)
{
	switch (event.type)
	{
		case WindowEventType::MouseAbsolute:
		{
			s_MouseState.Position = Vector2((float)event.mouseAbsolute.x, (float)event.mouseAbsolute.y);
			break;
		}

		case WindowEventType::MouseRaw:
		{
			s_MouseState.Raw += Vector2((float)event.mouseRaw.x, (float)event.mouseRaw.y);
			break;
		}

		case WindowEventType::MouseButton:
		{
			if (event.mouseButton.button == 0)
			{
				s_MouseState.LeftButton = event.mouseButton.m_Value;
			}
			else if (event.mouseButton.button == 1)
			{
				s_MouseState.MiddleButton = event.mouseButton.m_Value;
			}
			else
			{
				s_MouseState.RightButton = event.mouseButton.m_Value;
			}
			break;
		}

		case WindowEventType::MouseWheel:
		{
			s_MouseState.ScrollValue = event.mouseWheel.delta;
			break;
		}
	}
}
