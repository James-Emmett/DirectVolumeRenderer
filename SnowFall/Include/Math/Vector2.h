#pragma once
#include <string>

class Vector2
{
public:
	static const Vector2 Up;
	static const Vector2 Down;
	static const Vector2 Left;
	static const Vector2 Right;
	static const Vector2 One;
	static const Vector2 Zero;

public:
    float x, y;

public:
    Vector2();
    Vector2(float value);
    Vector2(float x, float y);
    Vector2(const Vector2& other);

public:
    //  Returns true if both vectors are equal.
    bool        operator==(const Vector2& other)const;
    //  Returns true if both values are not equal.
    bool        operator!=(const Vector2& other)const;

    //  Return ref value at index.
    float&      operator[](int i);
    //  Return value at index.
    float       operator[](int i)const;
    //  Returns and assigns float to this vector.
    Vector2&    operator=(const float value);
    //  Returns and assigns vector to this vector.
    Vector2&    operator=(const Vector2& other);
    //  Returns this vector added by value.
    Vector2&    operator+=(const Vector2& other);
    //  Returns this vector subtracted by value.
    Vector2&    operator-=(const Vector2& other);
    //  Returns this vector multipled by value.
    Vector2&    operator*=(const float value);
    //  Adds Two Vectors.
    Vector2     operator+(const Vector2& other)const;
    //  Subtracts one vector from another.
	Vector2     operator-(const Vector2& other)const;
    //  Multiplies vector by another.
    Vector2     operator*(const Vector2& other)const;
    //  Divides vector by another.
    Vector2     operator/(const Vector2& other)const;
    //  Multiplies vector by a number.
	Vector2     operator*(float value)const;
    //  Divides vector by a number.
	Vector2     operator/(float value)const;

    //  Multiplies Vector by Left hand float .
	friend Vector2 operator*(float value, const Vector2& other);
    // Returns Negated form of vector.
	friend Vector2 operator-(const Vector2& other);

public:
    //  Returns Length of this vector.
    float       Length()const;
    //  Returns Length of this vector squared.
    float       LengthSquared()const;
    //  Return vector with a magnitude of 1.
    Vector2&    Normalize();
    //  Returns formatted string for this vector.
    std::string ToString()const;

    //  Returns Angle between 2 vectors in degrees.
    static float    Angle(const Vector2& A, const Vector2 B);
    //  Dot Product of two vectors.
    static float    Dot(const Vector2& A, const Vector2& B);
    //  Returns vector A Perpendicular to vector B.
    static Vector2  Perpendicular(const Vector2& A);
    //  Returns the distance between a and b.
    static float    Distance(const Vector2& A, const Vector2& B);
    //  Returns the distance between a and b Squared.
    static float    DistanceSquared(const Vector2& A, const Vector2& B);
    //  Retruns a vector clamped between 2 vectors
    static Vector2  Clamp(const Vector2& vector, Vector2 min, Vector2 max);
    //  Returns a vector with its magnitude clamped to length.
    static Vector2  ClampMagnitude(const Vector2& vector, float length);
    //	Returns a vector that is made from the largest components of two vectors.
    static Vector2  Max(const Vector2& A, const Vector2& B);
    // Returns a vector that is made from the smallest components of two vectors.
    static Vector2  Min(const Vector2& A, const Vector2& B);
    //  Makes vectors normalized and orthogonal to each other.
    static void     OrthoNormalize(Vector2& normal, Vector2& tangent);
    // Makes this vector normalized.
    static Vector2  Normalize(const Vector2& vector);
    //  Projects vector A onto vector B.
    static Vector2  Project(const Vector2& A, const Vector2& B);
    //  Reflect vector aboutt he normal.
    static Vector2  Reflect(const Vector2& direction, const Vector2& normal);
    //  Linearly interpolates between two points.
    static Vector2  Lerp(const Vector2& A, const Vector2& B, float t);
    //  Return vector containing a point specified in barycentric cordiantes relative too a triangle.
    static void  Barycentric(const Vector2& A, const Vector2& B, const Vector2& C, const Vector2& P, float& u, float& v, float& w);
    //  Returns Point along a Catmull-Rom segment.
    static Vector2  CatmullRom(const Vector2& A, const Vector2& B, const Vector2& C, const Vector2& D, float t);
    //  Returns Point along a Quadratic Bezier Curve.
    static Vector2  Bezier(const Vector2& A, const Vector2& B, const Vector2& C, float t);
    //  Returns Point along a Cubic Bezier Curve.
    static Vector2  BezierCubic(const Vector2& A, const Vector2& B, const Vector2& C, const Vector2& D, float t);
};