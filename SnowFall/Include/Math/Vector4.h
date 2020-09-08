#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include <string>

class Vector4
{
public:
    static const Vector4 Forward;
	static const Vector4 Backward;
	static const Vector4 Up;
	static const Vector4 Down;
	static const Vector4 Left;
	static const Vector4 Right;
	static const Vector4 One;
	static const Vector4 Zero;

public:
    float x, y, z, w;

public:
    Vector4();
    Vector4(float value);
    Vector4(float x, float y, float z, float w);
	Vector4(const Vector2& other);
    Vector4(const Vector3& other);
    Vector4(const Vector4& other);

public:
    //  Returns true if both vectors are equal.
    bool        operator==(const Vector4& other)const;
    //  Returns true if both values are not equal.
    bool        operator!=(const Vector4& other)const;

    //  Return ref value at index.
    float&      operator[](int i);
    //  Return value at index.
    float       operator[](int i)const;
    //  Returns and assigns float to this vector.
    Vector4&    operator=(const float value);
    //  Returns and assigns vector to this vector.
    Vector4&    operator=(const Vector4& other);
    //  Returns this vector asigned with other vector
    Vector4&    operator=(const Vector2& other);
	//  Returns this vector asigned with other vector
    Vector4&    operator=(const Vector3& other);
    //  Returns this vector added by value.
    Vector4&    operator+=(const Vector4& other);
    //  Returns this vector subtracted by value.
    Vector4&    operator-=(const Vector4& other);
    //  Returns this vector multipled by value.
    Vector4&    operator*=(const float value);
    //  Adds Two Vectors.
    Vector4     operator+(const Vector4& other)const;
    //  Subtracts one vector from another.
	Vector4     operator-(const Vector4& other)const;
    //  Multiplies vector by another.
    Vector4     operator*(const Vector4& other)const;
    //  Divides vector by another.
    Vector4     operator/(const Vector4& other)const;
    //  Multiplies vector by a number.
	Vector4     operator*(float value)const;
    //  Divides vector by a number.
	Vector4     operator/(float value)const;

    //  Multiplies Vector by Left hand float .
	friend Vector4 operator*(float value, const Vector4& other);
    // Returns Negated form of vector.
	friend Vector4 operator-(const Vector4& other);

public:
    //  Returns Length of this vector.
    float       Length()const;
    //  Returns Length of this vector squared.
    float       LengthSquared()const;
    //  Return vector with a magnitude of 1.
    Vector4&    Normalize();
    //  Returns formatted string for this vector.
    std::string ToString()const;

    //  Returns Angle between 2 vectors in degrees.
    static float    Angle(const Vector4& A, const Vector4 B);
    //  Dot Product of two vectors.
    static float    Dot(const Vector4& A, const Vector4& B);
    //  Returns the distance between a and b.
    static float    Distance(const Vector4& A, const Vector4& B);
    //  Returns the distance between a and b Squared.
    static float    DistanceSquared(const Vector4& A, const Vector4& B);
    //  Retruns a vector clamped between 2 vectors
    static Vector4  Clamp(const Vector4& vector, Vector4 min, Vector4 max);
    //  Returns a vector with its magnitude clamped to length.
    static Vector4  ClampMagnitude(const Vector4& vector, float length);
    //	Returns a vector that is made from the largest components of two vectors.
    static Vector4  Max(const Vector4& A, const Vector4& B);
    // Returns a vector that is made from the smallest components of two vectors.
    static Vector4  Min(const Vector4& A, const Vector4& B);
    //  Makes vectors normalized and orthogonal to each other.
    static void     OrthoNormalize(Vector4& normal, Vector4& tangent);
    //  Projects vector A onto vector B.
    static Vector4  Project(const Vector4& A, const Vector4& B);
    //  Reflect vector aboutt he normal.
    static Vector4  Reflect(const Vector4& direction, const Vector4& normal);
    //  Linearly interpolates between two points.
    static Vector4  Lerp(const Vector4& A, const Vector4& B, float t);
    // Smooth steps the vector between min and max
    static Vector4 SmoothStep(const Vector4& min, const Vector4 max, float t);
    //  Returns Point along a Catmull-Rom segment.
    static Vector4  CatmullRom(const Vector4& A, const Vector4& B, const Vector4& C, const Vector4& D, float t);
    //  Returns Point along a Quadratic Bezier Curve.
    static Vector4  Bezier(const Vector4& A, const Vector4& B, const Vector4& C, float t);
    //  Returns Point along a Cubic Bezier Curve.
    static Vector4  BezierCubic(const Vector4& A, const Vector4& B, const Vector4& C, const Vector4& D, float t);
};