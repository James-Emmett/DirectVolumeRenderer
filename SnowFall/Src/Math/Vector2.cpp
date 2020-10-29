#include "Math/Vector2.h"
#include "Math/Mathf.h"

const Vector2 Vector2::Up		= Vector2(0, 1);
const Vector2 Vector2::Down		= Vector2(0, -1);
const Vector2 Vector2::Left		= Vector2(-1, 0);
const Vector2 Vector2::Right	= Vector2(1, 0);
const Vector2 Vector2::One		= Vector2(1, 1);
const Vector2 Vector2::Zero		= Vector2(0, 0);

Vector2::Vector2() :
	x(0.0f), y(0.0f)
{
}

Vector2::Vector2(float value) :
	x(value), y(value)
{
}

Vector2::Vector2(float X, float Y) :
	x(X), y(Y)
{
}

Vector2::Vector2(const Vector2& other) :
	x(other.x), y(other.y)
{
}

bool Vector2::operator==(const Vector2& other)const
{
	return x == other.x && y == other.y;
}

bool Vector2::operator!=(const Vector2& other)const
{
	return !(x == other.x && y == other.y);
}

float& Vector2::operator[](int i)
{
	if(i == 0)
	{
		return x;
	}

	return y;
}

float Vector2::operator[](int i)const
{
	if(i == 0)
	{
		return x;
	}

	return y;
}

Vector2& Vector2::operator=(const float value)
{
	*this = Vector2(value);
	return *this;
}

Vector2& Vector2::operator=(const Vector2& other)
{
	x = other.x; 
	y = other.y; 
	return *this;
}

Vector2& Vector2::operator+=(const Vector2& other)
{
	x += other.x; 
	y += other.y; 
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& other)
{
	x -= other.x; 
	y -= other.y; 
	return *this;
}

Vector2& Vector2::operator*=(const float value)
{
	x *= value; 
	y *= value; 
	return *this;
}

Vector2 Vector2::operator+(const Vector2& other)const
{
	return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(const Vector2& other)const
{
	return Vector2(x - other.x, y - other.y);
}

Vector2 Vector2::operator*(const Vector2& other)const
{
	return Vector2(x * other.x, y * other.y);
}

Vector2 Vector2::operator/(const Vector2& other)const
{
	return Vector2(x / other.x, y / other.y);
}

Vector2 Vector2::operator*(float value)const
{
	return Vector2(x * value, y * value);
}

Vector2 Vector2::operator/(float value)const
{
	float divisor = 1.0f / value;
	return Vector2(x * divisor, y * divisor);
}

Vector2 operator*(float value, const Vector2& other)
{
	return Vector2(other.x * value, other.y * value);
}

Vector2 operator-(const Vector2& other)
{
	return Vector2(-other.x, -other.y);
}

float Vector2::Length()const
{
	return Mathf::Sqrt(x * x + y * y);
}

float Vector2::LengthSquared()const
{
	return (x * x + y * y);
}

Vector2& Vector2::Normalize()
{
	float magSqr = (x * x + y * y);
	if (Mathf::IsZero(magSqr))
	{
		x = y = 0.0f;
		return *this;
	}

	float recip = Mathf::RecipSqrt(magSqr);
	x *= recip;
	y *= recip;

	return *this;
}

std::string Vector2::ToString()const
{
	std::string string = "Vector2: " + std::to_string(x) + ", " + std::to_string(y) + ", ";
	return string;
}

float Vector2::Angle(const Vector2& A, const Vector2 B)
{
	return Mathf::Acos(Dot(A, B) / Mathf::Sqrt(A.LengthSquared() * B.LengthSquared()));
}

float Vector2::Dot(const Vector2& A, const Vector2& B)
{
	return A.x * B.x + A.y * B.y;
}

Vector2 Vector2::Perpendicular(const Vector2& A)
{
    return Vector2(-A.y, A.x);
}

float Vector2::Distance(const Vector2& A, const Vector2& B)
{
	return (A - B).Length();
}

float Vector2::DistanceSquared(const Vector2& A, const Vector2& B)
{
	return (A - B).LengthSquared();
}

Vector2 Vector2::Clamp(const Vector2& vector, Vector2 min, Vector2 max)
{
	return Vector2(	Mathf::Clamp(vector.x, min.x, max.x),
					Mathf::Clamp(vector.y, min.y, max.y));
}

Vector2 Vector2::ClampMagnitude(const Vector2& vector, float length)
{
	Vector2 result = vector;

	float sqrMag = vector.LengthSquared();
	if (sqrMag > length * length)
	{
		float mag = Mathf::Sqrt(sqrMag);
		result = vector / mag;
		result *= length;
	}

	return result;
}

Vector2 Vector2::Max(const Vector2& A, const Vector2& B)
{
	return Vector2(Mathf::Max(A.x, B.x), Mathf::Max(A.y, B.y));
}

Vector2 Vector2::Min(const Vector2& A, const Vector2& B)
{
	return Vector2(Mathf::Min(A.x, B.x), Mathf::Min(A.y, B.y));
}

void Vector2::OrthoNormalize(Vector2& normal, Vector2& tangent)
{
	tangent = (tangent - Dot(tangent, normal) * normal).Normalize();
}

Vector2 Vector2::Normalize(const Vector2& vector)
{
	Vector2 product = Vector2::Zero;
	float magSqr = (vector.x * vector.x + vector.y * vector.y);
	if (Mathf::IsZero(magSqr))
	{
		return product;
	}

	float recip = Mathf::RecipSqrt(magSqr);
	product.x = vector.x * recip;
	product.y = vector.y * recip;

	return product;
}

Vector2 Vector2::Project(const Vector2& A, const Vector2& B)
{
	return Dot(A, B) * B;
}

Vector2 Vector2::Reflect(const Vector2& direction, const Vector2& normal)
{
	return direction - 2.0f * (Vector2::Dot(direction, normal) * normal);
}

Vector2 Vector2::Lerp(const Vector2& A, const Vector2& B, float t)
{
	return (1.0f - t) * A + (B * t);
}

void Vector2::Barycentric(const Vector2& A, const Vector2& B, const Vector2& C, const Vector2& P, float& u, float& v, float& w)
{
	//Get the 2 triangle edges and vector to the point
	Vector2 v0 = B - A, v1 = C - A, v2 = P - A;
	if ((v0 == v1) && (v1 == v2) && (v0 == v2)) { return; } //div 0

	float d00 = Dot(v0, v0);
	float d01 = Dot(v0, v1);
	float d11 = Dot(v1, v1);
	float d20 = Dot(v2, v0);
	float d21 = Dot(v2, v1);
	float denom = 1.0f / (d00 * d11 - d01 * d01);

	u = (d11 * d20 - d01 * d21) * denom;
	v = (d00 * d21 - d01 * d20) * denom;
	w = 1.0f - u - v;
}

Vector2 Vector2::CatmullRom(const Vector2& A, const Vector2& B, const Vector2& C, const Vector2& D, float t)
{
	float tSqr = t * t;
	float tCubed = tSqr * t;
	return	(0.5f * (2.0f * B + (C - A) * t +
			(2.0f * A - 5.0f * B + 4.0f * C - D) * tSqr +
			(3.0f * B - A - 3.0f * C + D) * tCubed));
}

Vector2 Vector2::Bezier(const Vector2& A, const Vector2& B, const Vector2& C, float t)
{
	t = Mathf::Clamp01(t);
	float	oneMinusT = 1.0f - t;
	return	oneMinusT * oneMinusT * A +
			2.0f * oneMinusT * t * B +
			t * t * C;
}

Vector2 Vector2::BezierCubic(const Vector2& A, const Vector2& B, const Vector2& C, const Vector2& D, float t)
{
	t = Mathf::Clamp01(t);
	float	oneMinusT = 1.0f - t;
	return	oneMinusT * oneMinusT * oneMinusT * A +
			3.0f * oneMinusT * oneMinusT * t * B +
			3.0f * oneMinusT * t * t * C +
			t * t * t * D;
}