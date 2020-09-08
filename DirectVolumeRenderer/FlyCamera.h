//Note:
/*
	Simple camera that flys around using W,S,A,D || Arrows
	and mouse look.
*/

#pragma once
#include "World/Component/Camera.h"
#include "Input/Input.h"

class FlyCamera : public Camera
{
public:
	float m_MouseSensitivity = 1.5f;
	float m_Speed = 1.0f;
	float m_SpeedMultiplier = 1.5f;

public:
	void Update(float deltaTime)
	{
		Vector3 delta = Vector3(Input::GetHorizontal(), 0, Input::GetVertical());
		Vector2 mouseLook = Input::GetMouseDelta() * 0.5f;
		float moveSpeed = 1;

		if (Input::GetKey(Keys::LeftShift))
		{
			moveSpeed *= 2;
		}

		if (Input::GetButton(Button::Mouse_Right))
		{
			//--Update camera--
			// Rotation is  hack to fix the roll drift :/
			Rotate(mouseLook.x, mouseLook.y);
		}

		Move(delta * moveSpeed * deltaTime);
		Camera::Update(deltaTime);
	}
};