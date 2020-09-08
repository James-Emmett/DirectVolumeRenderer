#pragma once
#include "Math/Vector2.h"
#include "Math/Color.h"

enum class PlayerIndex
{
	One,
	Two,
	Three,
	Four
};

enum class GamePadDeadZone
{
	None,
	Axes,
	Radial
};

struct GamePadFeedback
{
	float LeftMotor = 0.0f;
	float RightMotor = 0.0f;
	Color LedColor; // Playstation
};

enum class GamePadButtons
{
	A,
	B,
	X,
	Y,
	LB,
	RB,
	L3,
	R3,
	Start,
	Back,
	Up,
	Down,
	Left,
	Right,
};

struct GamePadThumbSticks
{
	Vector2 Left;
	Vector2 Right;
};

struct GamePadTriggers
{
	float Left = 0.0f;
	float Right = 0.0f;
};

struct GamePadState
{
	// Status 	
	bool IsConnected = false;

	// Buttons
	bool Buttons[14];

	// Sticks
	GamePadThumbSticks ThumbSticks;

	// Triggers
	GamePadTriggers Triggers;

	bool IsPressed(GamePadButtons button)
	{
		return Buttons[(int)button];
	}
};

class GamePad
{
public:
	// Returns the State RIGHT NOW essentially a Poll of the game pad!!!
	static const GamePadState GetState(PlayerIndex index, GamePadDeadZone deadZone = GamePadDeadZone::Radial, Vector2 deadZoneAmount = Vector2::Zero);
	// Used to set vibrations on the controller.
	static const void SetFeedback(PlayerIndex index, GamePadFeedback feedback);
	// Used to set vibrations on the controller.
	static const void SetFeedback(PlayerIndex index, float leftMotor, float rightMotor, Color led = Color::Black);
};