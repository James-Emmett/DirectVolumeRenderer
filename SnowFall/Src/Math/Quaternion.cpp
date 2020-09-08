#include "Math/Quaternion.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include "Math/Mathf.h"
#include <cmath>

const Quaternion Quaternion::Identity = Quaternion();

//-----------------------------------------------------------------------------
Quaternion::Quaternion(float X, float Y, float Z, float W) :
	x(X), y(Y), z(Z), w(W)
{
}

//-----------------------------------------------------------------------------
Quaternion::Quaternion(const float& angle, const Vector3& axis)
{
	Set(angle, axis);
}

//-----------------------------------------------------------------------------
Quaternion::~Quaternion()
{
}

//-----------------------------------------------------------------------------
bool Quaternion::operator==(const Quaternion& a) const
{
	return (w == a.w) && (x == a.x) && (y == a.y) && (z == a.z);
}

//-----------------------------------------------------------------------------
bool Quaternion::operator!=(const Quaternion& a) const
{
	return !((w == a.w) && (x == a.x) && (y == a.y) && (z == a.z));
}

//-----------------------------------------------------------------------------
Quaternion Quaternion::operator+(const Quaternion& a) const
{
	return Quaternion(x + a.x, y + a.y, z + a.z, w + a.w);
}

//-----------------------------------------------------------------------------
Quaternion Quaternion::operator-(const Quaternion& a) const
{
	return Quaternion(x - a.x, y - a.y, z - a.z, w - a.w);
}

//-----------------------------------------------------------------------------
Quaternion Quaternion::operator*(const Quaternion& q) const
{
	Quaternion product;

	product.x = (q.w * x) + (q.x * w) + (q.y * z) - (q.z * y);
	product.y = (q.w * y) - (q.x * z) + (q.y * w) + (q.z * x);
	product.z = (q.w * z) + (q.x * y) - (q.y * x) + (q.z * w);
	product.w = (q.w * w) - (q.x * x) - (q.y * y) - (q.z * z);

	return product;
}

Vector3 Quaternion::operator*(const Vector3& v) const
{
	Vector3 u(x, y, z);
	float s = w;

	return 2.0f * Vector3::Dot(u, v) * u + (s * s - Vector3::Dot(u, u)) * v + 2.0f * s * Vector3::Cross(u, v);
}

//-----------------------------------------------------------------------------
Quaternion Quaternion::operator*(float a) const
{
	return Quaternion(x * a, y * a, z * a, w * a);
}

//-----------------------------------------------------------------------------
Quaternion& Quaternion::operator+=(const Quaternion& a)
{
	x += a.x; y += a.y;
	z += a.z; w += a.w;

	return *this;
}

//-----------------------------------------------------------------------------
Quaternion& Quaternion::operator-=(const Quaternion& a)
{
	x -= a.x; y -= a.y;
	z -= a.z; w -= a.w;

	return *this;
}

//-----------------------------------------------------------------------------
Quaternion& Quaternion::operator*=(const Quaternion& q)
{
	Quaternion a = *this;

	x = (q.w * a.x) + (q.x * a.w) + (q.y * a.z) - (q.z * a.y);
	y = (q.w * a.y) - (q.x * a.z) + (q.y * a.w) + (q.z * a.x);
	z = (q.w * a.z) + (q.x * a.y) - (q.y * a.x) + (q.z * a.w);
	w = (q.w * a.w) - (q.x * a.x) - (q.y * a.y) - (q.z * a.z);

	return *this;
}

//-----------------------------------------------------------------------------
void Quaternion::Set(float X, float Y, float Z, float W)
{
	x = X; y = Y; z = Z; w = W;
}

//-----------------------------------------------------------------------------
void Quaternion::Set(const float& angle, const Vector3& axis)
{
	float theta = angle * 0.5f;
	float sinAngle = Mathf::Sin(theta);
	Vector3 n = Vector3::Normalize(axis);

	w = Mathf::Cos(theta);
	x = n.x * sinAngle;
	y = n.y * sinAngle;
	z = n.z * sinAngle;
}

//-----------------------------------------------------------------------------
void Quaternion::ToAngleAxis(float& angle, Vector3& axis)
{
	angle = Mathf::Acos(w) * 2.0f;
	float length = Mathf::Sqrt(1.0f - w * w);

	if (Mathf::IsZero(length))
	{
		axis = Vector3::Zero;
	}
	else
	{
		axis = Vector3(x * length, y * length, z * length);
	}
}

bool Quaternion::IsUnit()const
{
	return Mathf::IsZero(1.0f - w * w - x * x - y * y - z * z);
}

//-----------------------------------------------------------------------------
float Quaternion::Angle(const Quaternion& a, const Quaternion& b)
{
	Quaternion q = Inverse(a) * b;
	return 2.0f * Mathf::Atan2(Mathf::Sqrt(q.x * q.x + q.y * q.y + q.z * q.z), Mathf::Acos(q.w));
}

//-----------------------------------------------------------------------------
float Quaternion::Dot(const Quaternion& a, const Quaternion& b)
{
	return (a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z);
}

//-----------------------------------------------------------------------------
Quaternion Quaternion::AngleAxis(float angle, const Vector3& axis)
{
	return Quaternion(angle, axis);
}

//-----------------------------------------------------------------------------
Quaternion Quaternion::FromMatrix(const Matrix3& mat)
{
	Quaternion q;
	float tr = mat.Trace();
	float s = 0.0f;

	if (tr > 0.0f)
	{
		s = Mathf::Sqrt(tr + 1.0f) * 2.0f; //4*qw
		q.w = 0.25f * s; //w = 1/4 of s

		s = 1.0f / s; //s recip
		q.x = (mat(2, 1) - mat(1, 2) * s);
		q.y = (mat(0, 2) - mat(2, 0) * s);
		q.z = (mat(1, 0) - mat(0, 1) * s);
	}
	else if (mat(0, 0) > mat(1, 1) && mat(0, 0) > mat(2, 2))
	{
		s = Mathf::Sqrt(1.0f + mat(0, 0) - mat(1, 1) - mat(2, 2)) * 2.0f; //4*qx
		q.x = 0.25f * s; //x = 1/4 of s

		s = 1.0f / s; //s recip
		q.w = (mat(2, 1) - mat(1, 2) * s);
		q.y = (mat(0, 1) + mat(1, 0) * s);
		q.z = (mat(0, 2) + mat(2, 0) * s);
	}
	else if (mat(1, 1) > mat(2, 2))
	{
		s = Mathf::Sqrt(1.0f + mat(1, 1) - mat(0, 0) - mat(2, 2)) * 2.0f; //4*qy
		q.y = 0.25f * s; //y = 1/4 of s

		s = 1.0f / s; //s recip
		q.w = (mat(0, 2) - mat(2, 0) * s);
		q.x = (mat(0, 1) + mat(1, 0) * s);
		q.z = (mat(1, 2) + mat(2, 1) * s);
	}
	else
	{
		s = Mathf::Sqrt(1.0f + mat(2, 2) - mat(0, 0) - mat(1, 1)) * 2.0f; //4*qz
		q.z = 0.25f * s; //y = 1/4 of s

		s = 1.0f / s; //s recip
		q.w = (mat(1, 0) - mat(0, 1) * s);
		q.x = (mat(0, 2) + mat(2, 0) * s);
		q.y = (mat(1, 2) + mat(2, 1) * s);
	}

	return q;
}

//-----------------------------------------------------------------------------
Quaternion Quaternion::Euler(const Vector3& euler)
{
	float cx = Mathf::Cos(Mathf::DEG_TO_RAD * (0.5f * euler.x)); float sx = Mathf::Sin(Mathf::DEG_TO_RAD * (0.5f * euler.x));
	float cy = Mathf::Cos(Mathf::DEG_TO_RAD * (0.5f * euler.y)); float sy = Mathf::Sin(Mathf::DEG_TO_RAD * (0.5f * euler.y));
	float cz = Mathf::Cos(Mathf::DEG_TO_RAD * (0.5f * euler.z)); float sz = Mathf::Sin(Mathf::DEG_TO_RAD * (0.5f * euler.z));

	Quaternion q;

	q.x = sx * cy * cz + cx * sy * sz;
	q.y = cx * sy * cz - sx * cy * sz;
	q.z = cx * cy * sz + sx * sy * cz;
	q.w = cx * cy * cz - sx * sy * sz;

	return q;
}

//-----------------------------------------------------------------------------
Quaternion Quaternion::Lerp(const Quaternion& a, const Quaternion& b, float t)
{
	return Normalize(Quaternion((a * (1.0f - t)) + (b * t)));
}

//----------------------------------------------------------------------------
//  http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/
Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t)
{
	float dot = Dot(a, b);

	if (Mathf::IsEqual(dot, 1.0f))
	{
		return Lerp(a, b, t);
	}

	dot = Mathf::Clamp(dot, -1.0f, 1.0f); //Need to stay within Acos domain
	float theta_0 = Mathf::Acos(dot); //Get the theta angle
	float theta_r = theta_0 * t; //theta_r = the angle of the result

	//Make b orthogonal = c
	Quaternion c = b - (a * dot);
	c = Normalize(c);

	//Equivlent to vector rotation in 2D!
	return a * Mathf::Cos(theta_r) + c * Mathf::Sin(theta_r);
}

//-----------------------------------------------------------------------------
Quaternion Quaternion::Conjugate(const Quaternion& q)
{
	return Quaternion(-q.x, -q.y, -q.z, q.w);
}

//-----------------------------------------------------------------------------
Quaternion Quaternion::Inverse(const Quaternion& q)
{
	//Formula: q = (q*)/||Q||^2.
	float lengthSqr = q.w * q.w + q.x * q.x + q.y * q.y + q.x * q.z;
	if (Mathf::IsZero(lengthSqr))
	{
		return Identity;
	}

	//Conjugate multiplied by reciprical of length == inverse
	float lsRecip = 1.0f / lengthSqr;
	return Quaternion(-lsRecip * q.x, -lsRecip * q.y, -lsRecip * q.z, q.w);
}

//-----------------------------------------------------------------------------
Quaternion Quaternion::Normalize(const Quaternion& q)
{
	//--Get the length--
	float length = q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z;
	if (Mathf::IsZero(length)) { return Quaternion::Identity; }

	//--Multiply by recip sqrt--
	return q * Mathf::RecipSqrt(length);
}

//-----------------------------------------------------------------------------
Quaternion Quaternion::LookRotation(const Vector3& target, const Vector3& up)
{
	Vector3 forward = target; Vector3 u = up;
	Vector3::OrthoNormalize(forward, u); //Returns ortho basis of the 2
	Vector3 right = Vector3::Cross(up, forward);

	Quaternion q;
	q.w = Mathf::Sqrt(1.0f + right.x + u.y + forward.z);
	float w_recip = 1.0f / (4.0f * q.w);
	q.x = (up.z - forward.y) * w_recip;
	q.y = (up.x - right.z) * w_recip;
	q.z = (right.y - up.x) * w_recip;

	return q;
}

Vector3 Quaternion::ToEuler(const Quaternion& q)
{
	float xx = q.x * q.x;
	float yy = q.y * q.y;
	float zz = q.z * q.z;

	float xy = q.x * q.y;
	float xz = q.x * q.z;
	float yz = q.y * q.z;

	float xw = q.x * q.w;
	float yw = q.y * q.w;
	float zw = q.z * q.w;

	Vector3 vector;

	vector.x = -2.0f * (yz + xw);

	if (Mathf::Abs(vector.x) < 0.99999999)
	{
		vector.x = Mathf::Asin(vector.x);
		vector.y = Mathf::Atan2(xz - yw, 0.5f - xx - yy);
		vector.z = Mathf::Atan2(xy - zw, 0.5f - xx - zz);
	}
	else
	{
		vector.z = Mathf::PI_OVER_2 * vector.x;
		vector.x = Mathf::Atan2(-q.x * q.z - q.w * q.y, 0.5f - yy - zz);
		vector.z = 0.0f;
	}

	return Mathf::RAD_TO_DEG * vector;
}
