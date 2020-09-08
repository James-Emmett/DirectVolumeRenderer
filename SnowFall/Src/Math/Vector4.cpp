#include "Math/Vector4.h"
#include "Math/Mathf.h"

Vector4::Vector4() :
	x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{
}

Vector4::Vector4(float value) :
	x(value), y(value), z(value), w(value)
{
}

Vector4::Vector4(float X, float Y, float Z, float W) :
	x(X), y(Y), z(Z), w(W)
{
}

Vector4::Vector4(const Vector2& other) :
	x(other.x), y(other.y), z(0.0f), w(0.0f)
{
}

Vector4::Vector4(const Vector3& other):
	x(other.x), y(other.y), z(other.z), w(0.0f)
{
}

Vector4::Vector4(const Vector4& other) :
	x(other.x), y(other.y), z(other.z), w(other.w)
{
}

bool Vector4::operator==(const Vector4& other)const
{
	return x == other.x && y == other.y && z == other.z && w == other.w;
}

bool Vector4::operator!=(const Vector4& other)const
{
	return !(x == other.x && y == other.y && z == other.z && w == other.w);
}

float& Vector4::operator[](int i)
{
	if(i == 0)
	{
		return x;
	}
	else if (i == 1)
	{
		return y;
	}
    else if (i == 2)
    {
        return z;
    }
	
	return w;
}

float Vector4::operator[](int i)const
{
	if(i == 0)
	{
		return x;
	}
	else if (i == 1)
	{
		return y;
	}
    else if (i == 2)
    {
        return z;
    }
	
	return w;
}

Vector4& Vector4::operator=(const float value)
{
	*this = Vector4(value);
	return *this;
}

Vector4& Vector4::operator=(const Vector4& other)
{
	x = other.x; 
	y = other.y; 
	z = other.z;
    w = other.w;
	return *this;
}

Vector4& Vector4::operator=(const Vector2& other)
{
	x = other.x; 
	y = other.y;
	return *this;
}

Vector4& Vector4::operator=(const Vector3& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

Vector4& Vector4::operator+=(const Vector4& other)
{
	x += other.x; 
	y += other.y; 
	z += other.z;
    w += other.w;
	return *this;
}

Vector4& Vector4::operator-=(const Vector4& other)
{
	x -= other.x; 
	y -= other.y; 
	z -= other.z;
	w -= other.w;
	return *this;
}

Vector4& Vector4::operator*=(const float value)
{
	x *= value; 
	y *= value; 
	z *= value;
    w *= value;
	return *this;
}

Vector4 Vector4::operator+(const Vector4& other)const
{
	return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
}

Vector4 Vector4::operator-(const Vector4& other)const
{
	return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
}

Vector4 Vector4::operator*(const Vector4& other)const
{
	return Vector4(x * other.x, y * other.y, z * other.z, w * other.w);
}

Vector4 Vector4::operator/(const Vector4& other)const
{
	return Vector4(x / other.x, y / other.y, z / other.z, w / other.w);
}

Vector4 Vector4::operator*(float value)const
{
	return Vector4(x * value, y * value, z * value, w * value);
}

Vector4 Vector4::operator/(float value)const
{
	float divisor = 1.0f / value;
	return Vector4(x * divisor, y * divisor, z * divisor, w * divisor);
}

Vector4 operator*(float value, const Vector4& other)
{
	return Vector4(other.x * value, other.y * value, other.z * value, other.w * value);
}

Vector4 operator-(const Vector4& other)
{
	return Vector4(-other.x, -other.y, -other.z, -other.w);
}

float Vector4::Length()const
{
	return Mathf::Sqrt(x * x + y * y + z * z + w * w);
}

float Vector4::LengthSquared()const
{
	return (x * x + y * y + z * z + w * w);
}

Vector4& Vector4::Normalize()
{
	float magSqr = (x * x + y * y + z * z + w * w);
	if (Mathf::IsZero(magSqr))
	{
		x = y = z = w = 0.0f;
		return *this;
	}

	float recip = Mathf::RecipSqrt(magSqr);
	x *= recip;
	y *= recip;
	z *= recip;
	w *= recip;

	return *this;
}

std::string Vector4::ToString()const
{
	std::string string = "Vector4: " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w);
	return string;
}

float Vector4::Angle(const Vector4& A, const Vector4 B)
{
	return Mathf::Acos(Dot(A, B) / Mathf::Sqrt(A.LengthSquared() * B.LengthSquared()));
}

float Vector4::Dot(const Vector4& A, const Vector4& B)
{
	return A.x * B.x + A.y * B.y + A.z * B.z + A.w * B.w;
}

float Vector4::Distance(const Vector4& A, const Vector4& B)
{
	return (A - B).Length();
}

float Vector4::DistanceSquared(const Vector4& A, const Vector4& B)
{
	return (A - B).LengthSquared();
}

Vector4 Vector4::Clamp(const Vector4& vector, Vector4 min, Vector4 max)
{
	return Vector4(	Mathf::Clamp(vector.x, min.x, max.x),
					Mathf::Clamp(vector.y, min.y, max.y),
					Mathf::Clamp(vector.z, min.z, max.z),
					Mathf::Clamp(vector.w, min.w, max.w));
}

Vector4 Vector4::ClampMagnitude(const Vector4& vector, float length)
{
	Vector4 result = vector;

	float sqrMag = vector.LengthSquared();
	if (sqrMag > length * length)
	{
		float mag = Mathf::Sqrt(sqrMag);
		result = vector / mag;
		result *= length;
	}

	return result;
}

Vector4 Vector4::Max(const Vector4& A, const Vector4& B)
{
	return Vector4(Mathf::Max(A.x, B.x), Mathf::Max(A.y, B.y), Mathf::Max(A.z, B.z), Mathf::Max(A.w, B.w));
}

Vector4 Vector4::Min(const Vector4& A, const Vector4& B)
{
	return Vector4(Mathf::Min(A.x, B.x), Mathf::Min(A.y, B.y), Mathf::Min(A.z, B.z), Mathf::Min(A.w, B.w));
}

void Vector4::OrthoNormalize(Vector4& normal, Vector4& tangent)
{
	tangent = (tangent - Dot(tangent, normal) * normal).Normalize();
}

Vector4 Vector4::Project(const Vector4& A, const Vector4& B)
{
	return Dot(A, B) * B;
}

Vector4 Vector4::Reflect(const Vector4& direction, const Vector4& normal)
{
	return direction - 2.0f * (Vector4::Dot(direction, normal) * normal);
}

Vector4 Vector4::SmoothStep(const Vector4& min, const Vector4 max, float t)
{
    return Vector4(Mathf::SmoothStepKP(min.x, max.x, t),
	                Mathf::SmoothStepKP(min.y, max.y, t),
	                Mathf::SmoothStepKP(min.z, max.z, t),
	        	    Mathf::SmoothStepKP(min.w, max.w, t));
}

Vector4 Vector4::Lerp(const Vector4& A, const Vector4& B, float t)
{
	return (1.0f - t) * A + (B * t);
}

Vector4 Vector4::CatmullRom(const Vector4& A, const Vector4& B, const Vector4& C, const Vector4& D, float t)
{
	//Centroidal catmull rom (Tension is 0.5f)
	float tSqr = t * t;
	float tCubed = tSqr * t;
	return	0.5f * (2.0f * B + (C - A) * t +
				   (2.0f * A - 5.0f * B + 4.0f * C - D) * tSqr +
			       (3.0f * B - A - 3.0f * C + D) * tCubed);
}

Vector4 Vector4::Bezier(const Vector4& A, const Vector4& B, const Vector4& C, float t)
{
	t = Mathf::Clamp01(t);
	float	oneMinusT = 1.0f - t;
	return	oneMinusT * oneMinusT * A +
			2.0f * oneMinusT * t * B +
			t * t * C;
}

Vector4 Vector4::BezierCubic(const Vector4& A, const Vector4& B, const Vector4& C, const Vector4& D, float t)
{
	t = Mathf::Clamp01(t);
	float	oneMinusT = 1.0f - t;
	return	oneMinusT * oneMinusT * oneMinusT * A +
			3.0f * oneMinusT * oneMinusT * t * B +
			3.0f * oneMinusT * t * t * C +
			t * t * t * D;
}