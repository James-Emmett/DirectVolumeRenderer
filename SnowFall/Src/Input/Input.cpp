#include "Input/Input.h"

// Global Vairbles are fine for input
KeyboardState   lastKeyboardState;
KeyboardState   keyboardState;
MouseState      lastMouseState;
MouseState      mouseState;
GamePadState    lastGamePadState[4];
GamePadState    gamePadState[4];
GamePadDeadZone g_DeadZone[4] = { GamePadDeadZone::Radial };
Vector2         g_DeadZoneAmount[4] = { Vector2::Zero };
bool            g_keypressed = false;

void Input::Update()
{
    lastKeyboardState = keyboardState;
    keyboardState = Keyboard::GetState();

    lastMouseState = mouseState;
    mouseState = Mouse::GetState();

    for (size_t i = 0; i < 4; i++)
    {
        lastGamePadState[i] = gamePadState[i];
        gamePadState[i] = GamePad::GetState((PlayerIndex)i, g_DeadZone[i], g_DeadZoneAmount[i]);
    }
}

bool Input::GetKey(Keys key)
{
    return keyboardState.IsKeyDown(key);
}

bool Input::GetKeyDown(Keys key)
{
    return keyboardState.IsKeyDown(key) && lastKeyboardState.IsKeyUp(key);
}

bool Input::GetKeyUp(Keys key)
{
    return keyboardState.IsKeyUp(key) && lastKeyboardState.IsKeyDown(key);
}

bool Input::AnyKey()
{
    return g_keypressed;
}

float Input::GetHorizontal()
{
    if (keyboardState.IsKeyDown(Keys::A) || keyboardState.IsKeyDown(Keys::Left))
    {
        return -1;
    }

    if (keyboardState.IsKeyDown(Keys::D) || keyboardState.IsKeyDown(Keys::Right))
    {
        return 1;
    }

    return 0;
}

float Input::GetVertical()
{
    if (keyboardState.IsKeyDown(Keys::W) || keyboardState.IsKeyDown(Keys::Up))
    {
        return 1;
    }

    if (keyboardState.IsKeyDown(Keys::S) || keyboardState.IsKeyDown(Keys::Down))
    {
        return -1;
    }

    return 0;
}

bool Input::GetButton(Button button, PlayerIndex index)
{
    if (button >= Button::GamePad_A)
    {
        return gamePadState[(Uint32)index].Buttons[(Uint32)button - (Uint32)Button::GamePad_A];
    }

    switch (button)
    {
    case Button::Mouse_Left:
        return mouseState.LeftButton;
        break;
    case Button::Mouse_Right:
        return mouseState.RightButton;
        break;
    case Button::Mouse_Middle:
        return mouseState.MiddleButton;
        break;

    case Button::Undefined:
    default:
        return false;
        break;
    }
    return false;
}

bool Input::GetButtonDown(Button button, PlayerIndex index)
{
    if (button >= Button::GamePad_A)
    {
        return gamePadState[(Uint32)index].Buttons[(Uint32)button - (Uint32)Button::GamePad_A] &&
            lastGamePadState[(Uint32)index].Buttons[(Uint32)button - (Uint32)Button::GamePad_A] == false;
    }

    switch (button)
    {
    case Button::Mouse_Left:
        return mouseState.LeftButton && lastMouseState.LeftButton == false;
        break;
    case Button::Mouse_Right:
        return mouseState.RightButton && lastMouseState.RightButton == false;
        break;
    case Button::Mouse_Middle:
        return mouseState.MiddleButton && lastMouseState.MiddleButton == false;
        break;

    case Button::Undefined:
    default:
        return false;
        break;
    }
    return false;
}

bool Input::GetButtonUp(Button button, PlayerIndex index)
{
    if (button >= Button::GamePad_A)
    {
        return gamePadState[(Uint32)index].Buttons[(Uint32)button - (Uint32)Button::GamePad_A] &&
            lastGamePadState[(Uint32)index].Buttons[(Uint32)button - (Uint32)Button::GamePad_A] == false;
    }

    switch (button)
    {
    case Button::Mouse_Left:
        return mouseState.LeftButton == false && lastMouseState.LeftButton;
        break;
    case Button::Mouse_Right:
        return mouseState.RightButton == false && lastMouseState.RightButton;
        break;
    case Button::Mouse_Middle:
        return mouseState.MiddleButton == false && lastMouseState.MiddleButton;
        break;

    case Button::Undefined:
    default:
        return false;
        break;
    }
    return false;
}

int Input::GetMouseScroll()
{
    return mouseState.ScrollValue - lastMouseState.ScrollValue;
}

Vector2 Input::GetMousePosition()
{
    return mouseState.Position;
}

Vector2 Input::GetMouseDelta()
{
    return mouseState.Raw - lastMouseState.Raw;
}

Vector2 Input::GetThumbStick(GamePadSticks thumbstick, PlayerIndex index)
{
    return (thumbstick == GamePadSticks::Left) ? gamePadState[(Uint32)index].ThumbSticks.Left : gamePadState[(Uint32)index].ThumbSticks.Right;
}

float Input::GetTrigger(GamePadTrigger trigger, PlayerIndex index)
{
    return (trigger == GamePadTrigger::Left) ? gamePadState[(Uint32)index].Triggers.Left : gamePadState[(Uint32)index].Triggers.Right;
}

void Input::SetFeedBack(GamePadFeedback& feedback, PlayerIndex index)
{
    GamePad::SetFeedback(index, feedback);
}

void Input::SetDeadZone(PlayerIndex index, GamePadDeadZone deadZoneType, Vector2 amount)
{
    g_DeadZone[(Uint32)index] = deadZoneType;
    g_DeadZoneAmount[(Uint32)index] = amount;
}

void Input::KeyPressed()
{
    g_keypressed = true;
}

void Input::Reset()
{
    g_keypressed = false;
}
