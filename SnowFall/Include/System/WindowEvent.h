#pragma once
#include "Math/Vector2.h"
#include "System/Types.h"

enum class WindowEventType
{
	Closed,
	Resize,
	LostFocus,
	GainedFocus,
	KeyEvent,
	MouseAbsolute,
	MouseRaw,
	MouseButton,
	MouseWheel,
	Idle,
	Count
};

class WindowEvent
{
public:
	struct Key
	{
		unsigned int key;    // 1 of 256 virtural keys
		bool pressed = false; //True = pressed, false = released
	};

	struct MouseAbsolute
	{
		Uint32 x;
		Uint32 y;
	};

	struct MouseRaw
	{
		float x;
		float y;
	};

	struct MouseButton
	{
		unsigned int button;  //0 = left, 1 = mid, 2 = right
		bool m_Value;
	};

	struct MouseWheel
	{
		int delta; //How much the mouse has scrolled since last position
	};

	struct Resized
	{
		Uint32 width;
		Uint32 height;
	};


public:
	WindowEvent() {}
	~WindowEvent() {}

	//Anonymous unions, use to be questionable...
	union
	{
		Key           key;
		MouseAbsolute mouseAbsolute;
		MouseRaw      mouseRaw;
		MouseButton   mouseButton;
		MouseWheel    mouseWheel;
		Resized       resized;
		bool          isIdle;
	};

	WindowEventType type = WindowEventType::Count;
};