//Note:
/*
	Each game will have custom Actions that Input responds too, this class is only here
	to provide Low-Level support for input and managing the last frame and current frame input.
*/

#pragma once
#include "Keyboard.h"
#include "Mouse.h"
#include "GamePad.h"

// Simple Button enum
enum class Button
{
	Undefined = 0,
	Mouse_Left,
	Mouse_Right,
	Mouse_Middle,

	GamePad_A = 8,
	GamePad_B,
	GamePad_X,
	GamePad_Y,
	GamePad_LB,
	GamePad_RB,
	GamePad_L3,
	GamePad_R3,
	GamePad_Start,
	GamePad_Back,
	GamePad_Up,
	GamePad_Down,
	GamePad_Left,
	GamePad_Right,
};

enum class GamePadSticks
{
	Left,
	Right,
};

enum class GamePadTrigger
{
	Left,
	Right,
};

namespace Input
{
	void Update();
	bool GetKey(Keys key);
	bool GetKeyDown(Keys key);
	bool GetKeyUp(Keys key);
	bool AnyKey();
	float GetHorizontal();
	float GetVertical();
	bool GetButton(Button button, PlayerIndex index = PlayerIndex::One);
	bool GetButtonDown(Button button, PlayerIndex index = PlayerIndex::One);
	bool GetButtonUp(Button button, PlayerIndex index = PlayerIndex::One);
	int  GetMouseScroll();
	Vector2 GetMousePosition();
	Vector2 GetMouseDelta();
	Vector2 GetThumbStick(GamePadSticks thumbstick, PlayerIndex index);
	float GetTrigger(GamePadTrigger trigger, PlayerIndex index);
	void  SetFeedBack(GamePadFeedback& feedback, PlayerIndex index);
	void SetDeadZone(PlayerIndex index, GamePadDeadZone deadZoneType, Vector2 amount);

	// Ignore this its for AnyKey
	void KeyPressed();
	void Reset();
};