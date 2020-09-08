#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Math/Mathf.h"

const Vector3 Vector3::Forward	= Vector3(0, 0, 1);
const Vector3 Vector3::Backward = Vector3(0, 0, -1);
const Vector3 Vector3::Up		= Vector3(0, 1, 0);
const Vector3 Vector3::Down		= Vector3(0, -1, 0);
const Vector3 Vector3::Left		= Vector3(-1, 0, 0);
const Vector3 Vector3::Right	= Vector3(1, 0, 0);
const Vector3 Vector3::One		= Vector3(1, 1, 1);
const Vector3 Vector3::Zero		= Vector3(0, 0, 0);

Vector3::Vector3() :
	x(0.0f), y(0.0f), z(0.0f)
{
}

Vector3::Vector3(float value) :
	x(value), y(value), z(value)
{
}

Vector3::Vector3(float X, float Y, float Z) :
	x(X), y(Y), z(Z)
{
}

Vector3::Vector3(const Vector2& other) :
	x(other.x), y(other.y), z(0.0f)
{
}

Vector3::Vector3(const Vector3& other) :
	x(other.x), y(other.y), z(other.z)
{
}

bool Vector3::operator==(const Vector3& other)const
{
	return x == other.x && y == other.y && z == other.z;
}

bool Vector3::operator!=(const Vector3& other)const
{
	return !(x == other.x && y == other.y && z == other.z);
}

float& Vector3::operator[](int i)
{
	if(i == 0)
	{
		return x;
	}
	else if (i == 1)
	{
		return y;
	}
	
	return z;
}

float Vector3::operator[](int i)const
{
	if(i == 0)
	{
		return x;
	}
	else if (i == 1)
	{
		return y;
	}
	
	return z;
}

Vector3& Vector3::operator=(const float value)
{
	*this = Vector3(value);
	return *this;
}

Vector3& Vector3::operator=(const Vector3& other)
{
	x = other.x; 
	y = other.y; 
	z = other.z;
	return *this;
}

Vector3& Vector3::operator=(const Vector2& other)
{
	x = other.x; 
	y = other.y;
	return *this;
}

Vector3& Vector3::operator+=(const Vector3& other)
{
	x += other.x; 
	y += other.y; 
	z += other.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& other)
{
	x -= other.x; 
	y -= other.y; 
	z -= other.z;
	return *this;
}

Vector3& Vector3::operator*=(const float value)
{
	x *= value; 
	y *= value; 
	z *= value;
	return *this;
}

Vector3 Vector3::operator+(const Vector3& other)const
{
	return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3 Vector3::operator-(const Vector3& other)const
{
	return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3 Vector3::operator*(const Vector3& other)const
{
	return Vector3(x * other.x, y * other.y, z * other.z);
}

Vector3 Vector3::operator/(const Vector3& other)const
{
	return Vector3(x / other.x, y / other.y, z / other.z);
}

Vector3 Vector3::operator*(float value)const
{
	return Vector3(x * value, y * value, z * value);
}

Vector3 Vector3::operator/(float value)const
{
	float divisor = 1.0f / value;
	return Vector3(x * divisor, y * divisor, z * divisor);
}

Vector3 operator*(float value, const Vector3& other)
{
	return Vector3(other.x * value, other.y * value, other.z * value);
}

Vector3 operator-(const Vector3& other)
{
	return Vector3(-other.x, -other.y, -other.z);
}

float Vector3::Length()const
{
	return Mathf::Sqrt(x * x + y * y + z * z);
}

float Vector3::LengthSquared()const
{
	return (x * x + y * y + z * z);
}

Vector3& Vector3::Normalize()
{
	float magSqr = (x * x + y * y + z * z);
	if (Mathf::IsZero(magSqr))
	{
		x = y = z = 0.0f;
		return *this;
	}

	float recip = Mathf::RecipSqrt(magSqr);
	x *= recip;
	y *= recip;
	z *= recip;

	return *this;
}

std::string Vector3::ToString()const
{
	std::string string = "Vector3: " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z);
	return string;
}

float Vector3::Angle(const Vector3& A, const Vector3 B)
{
	return Mathf::Acos(Dot(A, B) / Mathf::Sqrt(A.LengthSquared() * B.LengthSquared()));
}

float Vector3::Dot(const Vector3& A, const Vector3& B)
{
	return A.x * B.x + A.y * B.y + A.z * B.z;
}

Vector3 Vector3::Cross(const Vector3& A, const Vector3& B)
{
	return Vector3(	A.y * B.z - A.z * B.y,
					A.z * B.x - A.x * B.z,
					A.x * B.y - A.y * B.x);
}

float Vector3::Distance(const Vector3& A, const Vector3& B)
{
	return (A - B).Length();
}

float Vector3::DistanceSquared(const Vector3& A, const Vector3& B)
{
	return (A - B).LengthSquared();
}

Vector3 Vector3::Clamp(const Vector3& vector, Vector3 min, Vector3 max)
{
	return Vector3(	Mathf::Clamp(vector.x, min.x, max.x),
					Mathf::Clamp(vector.y, min.y, max.y),
					Mathf::Clamp(vector.z, min.z, max.z));
}

Vector3 Vector3::ClampMagnitude(const Vector3& vector, float length)
{
	Vector3 result = vector;

	float sqrMag = vector.LengthSquared();
	if (sqrMag > length * length)
	{
		float mag = Mathf::Sqrt(sqrMag);
		result = vector / mag;
		result *= length;
	}

	return result;
}

Vector3 Vector3::Max(const Vector3& A, const Vector3& B)
{
	return Vector3(Mathf::Max(A.x, B.x), Mathf::Max(A.y, B.y), Mathf::Max(A.z, B.z));
}

Vector3 Vector3::Min(const Vector3& A, const Vector3& B)
{
	return Vector3(Mathf::Min(A.x, B.x), Mathf::Min(A.y, B.y), Mathf::Min(A.z, B.z));
}

Vector3 Vector3::Normalize(const Vector3& vector)
{
	Vector3 normal = Vector3::Zero;
	float magSqr = (vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	if (Mathf::IsZero(magSqr))
	{
		return normal;
	}

	float recip = Mathf::RecipSqrt(magSqr);
	normal = vector * recip;
	return normal;
}

void Vector3::OrthoNormalize(Vector3& normal, Vector3& tangent)
{
	tangent = (tangent - Dot(tangent, normal) * normal).Normalize();
}

Vector3 Vector3::Project(const Vector3& A, const Vector3& B)
{
	return Dot(A, B) * B;
}

Vector3 Vector3::Reflect(const Vector3& direction, const Vector3& normal)
{
	return direction - 2.0f * (Vector3::Dot(direction, normal) * normal);
}

Vector3 Vector3::Lerp(const Vector3& A, const Vector3& B, float t)
{
	return (1.0f - t) * A + (B * t);
}

void Vector3::Barycentric(const Vector3& A, const Vector3& B, const Vector3& C, const Vector3& P, float& u, float& v, float& w)
{
	//Get the 2 triangle edges and vector to the point
	Vector3 v0 = B - A, v1 = C - A, v2 = P - A;
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

Vector3 Vector3::CatmullRom(const Vector3& A, const Vector3& B, const Vector3& C, const Vector3& D, float t)
{
	float tSqr = t * t;
	float tCubed = tSqr * t;
	return	(0.5f * (2.0f * B + (C - A) * t +
			(2.0f * A - 5.0f * B + 4.0f * C - D) * tSqr +
			(3.0f * B - A - 3.0f * C + D) * tCubed));
}

Vector3 Vector3::Bezier(const Vector3& A, const Vector3& B, const Vector3& C, float t)
{
	t = Mathf::Clamp01(t);
	float	oneMinusT = 1.0f - t;
	return	oneMinusT * oneMinusT * A +
			2.0f * oneMinusT * t * B +
			t * t * C;
}

Vector3 Vector3::BezierCubic(const Vector3& A, const Vector3& B, const Vector3& C, const Vector3& D, float t)
{
	t = Mathf::Clamp01(t);
	float	oneMinusT = 1.0f - t;
	return	oneMinusT * oneMinusT * oneMinusT * A +
			3.0f * oneMinusT * oneMinusT * t * B +
			3.0f * oneMinusT * t * t * C +
			t * t * t * D;
}
