#include "Input/Keyboard.h"

KeyboardState Keyboard::s_KeyboardState;

bool KeyboardState::IsKeyDown(Keys key)
{
    return keyState[(int)key] == true;
}

bool KeyboardState::IsKeyUp(Keys key)
{
    return keyState[(int)key] == false;
}

KeyboardState Keyboard::GetState()
{
	return s_KeyboardState;
}

void Keyboard::HandleEvent(const WindowEvent& event)
{
	if (event.type == WindowEventType::KeyEvent)
	{
		s_KeyboardState.keyState[event.key.key] = event.key.pressed;
	}
}
