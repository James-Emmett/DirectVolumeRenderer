#pragma once

class Vector3;
class Matrix3;
class Quaternion
{
	//--Cosntants--
public:
	static const Quaternion Identity;

public:
	float x, y, z, w;

public:
	Quaternion(float X = 0, float Y = 0, float Z = 0, float W = 1);
	Quaternion(const float& angle, const Vector3& axis);
	~Quaternion();

public:
	bool operator==(const Quaternion& a)const;
	bool operator!=(const Quaternion& a)const;

	Quaternion operator+(const Quaternion& a)const;
	Quaternion operator-(const Quaternion& a)const;
	Quaternion operator*(const Quaternion& a)const;
	Vector3    operator*(const Vector3& v)const;
	Quaternion operator*(float a)const;

	Quaternion& operator+=(const Quaternion& a);
	Quaternion& operator-=(const Quaternion& a);
	Quaternion& operator*=(const Quaternion& a);

public:
	void Set(float X, float Y, float Z, float W);
	void Set(const float& angle, const Vector3& axis);
	void ToAngleAxis(float& angle, Vector3& axis);
	bool IsUnit()const;

public:
	static float Angle(const Quaternion& a, const Quaternion& b);
	static float Dot(const Quaternion& a, const Quaternion& b);
	static Quaternion AngleAxis(float angle, const Vector3& axis);
	static Quaternion FromMatrix(const Matrix3& mat);
	static Quaternion Euler(const Vector3& euler);
	static Quaternion Lerp(const Quaternion& a, const Quaternion& b, float t);
	static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);
	static Quaternion Conjugate(const Quaternion& q);
	static Quaternion Inverse(const Quaternion& q);
	static Quaternion Normalize(const Quaternion& q);
	static Quaternion LookRotation(const Vector3& target, const Vector3& up);
	static Vector3 ToEuler(const Quaternion& q);
};