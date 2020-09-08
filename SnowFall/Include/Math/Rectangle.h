#pragma once
#include "Vector2.h"

template<typename T>
class Rect
{
public:
	T X, Y, Width, Height;

public:
	Rect(T x, T y, T width, T height)
	{
		X = x; Y = y;
		Width = width; Height = height;
	}

	T Top()const
	{
		return Y + Height;
	}

	T Bottom()const
	{
		return Y;
	}

	T Left()const
	{
		return X;
	}

	T Right()const
	{
		return X + Width;
	}

	bool Intersects(const Rect<T>& other)const
	{
		if (other.Right() < Left() || other.Left() > Right()) { return false; }
		if (other.Top() < Bottom() || other.Bottom() > Top()) { return false; }
		return true;
	}

	bool Contains(Vector2 point)const
	{
		if (point.x < Left() || point.x > Right()) { return false; }
		if (point.y < Bottom() || Bottom() > point.y) { return false; }
		return true;
	}
};