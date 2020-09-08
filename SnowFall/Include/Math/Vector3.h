#pragma once
#include "Vector2.h"
#include <string>

class Vector3
{
public:
    static const Vector3 Forward;
	static const Vector3 Backward;
	static const Vector3 Up;
	static const Vector3 Down;
	static const Vector3 Left;
	static const Vector3 Right;
	static const Vector3 One;
	static const Vector3 Zero;

public:
    float x, y, z;

public:
    Vector3();
    Vector3(float value);
    Vector3(float x, float y, float z);
    Vector3(const Vector2& other);
    Vector3(const Vector3& other);

public:
    //  Returns true if both vectors are equal.
    bool        operator==(const Vector3& other)const;
    //  Returns true if both values are not equal.
    bool        operator!=(const Vector3& other)const;

    //  Return ref value at index.
    float&      operator[](int i);
    //  Return value at index.
    float       operator[](int i)const;
    //  Returns and assigns float to this vector.
    Vector3&    operator=(const float value);
    //  Returns and assigns vector to this vector.
    Vector3&    operator=(const Vector3& other);
    //  Returns this vector asigned with other vector
    Vector3&    operator=(const Vector2& other);
    //  Returns this vector added by value.
    Vector3&    operator+=(const Vector3& other);
    //  Returns this vector subtracted by value.
    Vector3&    operator-=(const Vector3& other);
    //  Returns this vector multipled by value.
    Vector3&    operator*=(const float value);
    //  Adds Two Vectors.
    Vector3     operator+(const Vector3& other)const;
    //  Subtracts one vector from another.
	Vector3     operator-(const Vector3& other)const;
    //  Multiplies vector by another.
    Vector3     operator*(const Vector3& other)const;
    //  Divides vector by another.
    Vector3     operator/(const Vector3& other)const;
    //  Multiplies vector by a number.
	Vector3     operator*(float value)const;
    //  Divides vector by a number.
	Vector3     operator/(float value)const;

    //  Multiplies Vector by Left hand float .
	friend Vector3 operator*(float value, const Vector3& other);
    // Returns Negated form of vector.
	friend Vector3 operator-(const Vector3& other);

public:
    //  Returns Length of this vector.
    float       Length()const;
    //  Returns Length of this vector squared.
    float       LengthSquared()const;
    //  Return vector with a magnitude of 1.
    Vector3&    Normalize();
    //  Returns formatted string for this vector.
    std::string ToString()const;

    //  Returns Angle between 2 vectors in degrees.
    static float    Angle(const Vector3& A, const Vector3 B);
    //  Dot Product of two vectors.
    static float    Dot(const Vector3& A, const Vector3& B);
    //  Cross Product of two vectors.
    static Vector3  Cross(const Vector3& A, const Vector3& B);
    //  Returns the distance between a and b.
    static float    Distance(const Vector3& A, const Vector3& B);
    //  Returns the distance between a and b Squared.
    static float    DistanceSquared(const Vector3& A, const Vector3& B);
    //  Retruns a vector clamped between 2 vectors
    static Vector3  Clamp(const Vector3& vector, Vector3 min, Vector3 max);
    //  Returns a vector with its magnitude clamped to length.
    static Vector3  ClampMagnitude(const Vector3& vector, float length);
    //	Returns a vector that is made from the largest components of two vectors.
    static Vector3  Max(const Vector3& A, const Vector3& B);
    //  Returns a vector that is made from the smallest components of two vectors.
    static Vector3  Min(const Vector3& A, const Vector3& B);
    //  Returns a Normalized Vector
    static Vector3  Normalize(const Vector3& vector);
    //  Makes vectors normalized and orthogonal to each other.
    static void     OrthoNormalize(Vector3& normal, Vector3& tangent);
    //  Projects vector A onto vector B.
    static Vector3  Project(const Vector3& A, const Vector3& B);
    //  Reflect vector aboutt he normal.
    static Vector3  Reflect(const Vector3& direction, const Vector3& normal);
    //  Linearly interpolates between two points.
    static Vector3  Lerp(const Vector3& A, const Vector3& B, float t);
    //  Return vector containing a point specified in barycentric cordiantes relative too a triangle.
    static void  Barycentric(const Vector3& A, const Vector3& B, const Vector3& C, const Vector3& P, float& u, float& v, float& w);
    //  Returns Point along a Catmull-Rom segment.
    static Vector3  CatmullRom(const Vector3& A, const Vector3& B, const Vector3& C, const Vector3& D, float t);
    //  Returns Point along a Quadratic Bezier Curve.
    static Vector3  Bezier(const Vector3& A, const Vector3& B, const Vector3& C, float t);
    //  Returns Point along a Cubic Bezier Curve.
    static Vector3  BezierCubic(const Vector3& A, const Vector3& B, const Vector3& C, const Vector3& D, float t);
};