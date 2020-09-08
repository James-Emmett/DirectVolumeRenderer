#pragma once

class Vector4;
class Color
{
public:
	static const Color White;
	static const Color Black;
	static const Color Clear;
	static const Color Red;
	static const Color Blue;
	static const Color Green;
	static const Color Orange;
	static const Color Purple;
	static const Color Yellow;
	static const Color Pink;
	static const Color Aqua;
	static const Color Grey;
	static const Color CornflowerBlue;

public:
	float r, g, b, a = 0.0f;

public:
	Color();
	Color(float R, float G, float B, float A = 1.0f);
	Color(const Vector4& vector);
	Color(const Color& color);
	~Color();

public:
	Color& operator=(const Color& color);
	Color& operator=(const Vector4& vector);

	Color  operator+(const Color& B)const;
	Color  operator-(const Color& B)const;
	Color  operator*(const Color& B)const;
	Color  operator*(const float B)const;
	Color  operator/(float B)const;

	Color& operator+=(const Color& B);
	Color& operator-=(const Color& B);
	Color& operator*=(const Color& B);
	Color& operator/=(float B);

	bool operator==(const Color& color)const;
	bool operator!=(const Color& color)const;

	friend Color operator*(float s, const Color& color);

public:
	Color Linear()const;
	Color SRGB()const;
	Color GrayScale()const;
	float Intensity()const;

public:
	static Color HSVToRGB(float H, float S, float V);
	static void  RGBToHSV(const Color& col, float& outH, float& outS, float& outV);
	static Color Lerp(const Color& A, const Color& B, float t);
};