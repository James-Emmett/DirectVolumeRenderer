//Note
/*
	4X4 matrix, Column Major, Left-Handed (Positive rotation is clockwise)
    Data Layout is LINEAR

	Linear memory layout:
	[ a b c d ]
	[ e f g h ]
	[ i j k l ]
    [ m n o p ]

	Multiplication is thus:

	[ a b c d ]     [x]
	[ e f g h ] *   [y] = [x*a + y*b + z*c + w*d]
	[ i j k l ]     [z]
    [ m n o p ]     [w]

	Multiplication Order in c++ is: T * R * S

	Data can be accessed as: row + (col * 4)
*/
#pragma once
#include "Matrix3.h"
#include "Vector4.h"

class Matrix4
{
public:
    static Matrix4 Identiy;
    static Matrix4 Zero;
	float m[16] = { 0 };

public:
	Matrix4();
	Matrix4(const Matrix4& mat);
	Matrix4(float m00, float m01, float m02, float m03, // 1st Col
            float m04, float m05, float m06, float m07, // 2nd Col
            float m08, float m09, float m10, float m11, // 3rd Col
            float m12, float m13, float m14, float m15);// 4th Col        
    ~Matrix4();

public:
	bool	operator==(const Matrix4& mat)const;
	bool	operator!=(const Matrix4& mat)const;
	float&	operator[](const unsigned int i);
	float	operator[](const unsigned int i)const;
	Matrix4& operator=(const Matrix4& mat);
	Matrix4	operator+(const Matrix4& mat)const;
	Matrix4 operator-(const Matrix4& mat)const;
	Matrix4 operator*(const Matrix4& mat)const;
	Matrix4 operator*(const float s);

	Matrix4& operator+=(const Matrix4& mat);
	Matrix4& operator-=(const Matrix4& mat);
	Matrix4& operator*=(const Matrix4& mat);
	Matrix4& operator*=(const float s);

	float&	operator()(unsigned int row, unsigned int col);
	float	operator()(unsigned int row, unsigned int col)const;

public:
	//	Creates Matrix of Minors
	static Matrix4 Minor(const Matrix4& mat);
	//	Creates Cofactor of matrix, lattice + -
	static Matrix4 Cofactor(const Matrix4& mat);
	//	Creates Adjoint of matix (transpose of cofactor)
	static Matrix4 Adjoint(const Matrix4& mat);
	//	The determinant of the matrix. (Read Only)
	static float   Determinant(const Matrix4& mat);
    //  Creates a inversed matrix
	static Matrix4 Inverse(const Matrix4& mat);
    //  Create a Rotation Matrix on X axis from Euler in Radians.
    static Matrix4 RotateX(float angle);
    //  Create a Rotation Matrix on Y axis from Euler in Radians.
    static Matrix4 RotateY(float angle);
    //  Create a Rotation Matrix on Z axis from Euler in Radians.
    static Matrix4 RotateZ(float angle);
    //  Create a Rotation Matrix from Euler Angles.
    static Matrix4 Rotate(const Vector3& euler);
    //  Create a Rotation Matrix from quaternion.
    static Matrix4 Rotate(const Quaternion& rotation);
    // Create a Look At, "view" matrix
    static Matrix4 LookAt(const Vector3& from, const Vector3& to, const Vector3& up);
    //  Create an orthogonal projection matrix.
	static Matrix4 Orthographic(int width, int height, float zNear, float zFar);
    //  Create Perspective Matrix usign the fov and aspect ratio
	static Matrix4 PerspectiveFov(float fov, float aspect, float zNear, float zFar);
	//  Create Perspective Matrix using width, height, near and far.
	static Matrix4 Perspective(float width, float height, float zNear, float zFar);
	//  Create Perspective Matrix off center using l, r, t, b, n, z, rarer?
	static Matrix4 PerspectiveOffCenter(float left, float right, float top, float bottom, float zNear, float zFar);
    //  Creates a scaling matrix.
    static Matrix4 Scale(const Vector3& scale);
    //  Create a translation matrix.
    static Matrix4 Translate(const Vector3& vector);
    //  Create a Translation, Rotation(Euler) and Scaleing Matrix
    static Matrix4 TRS(const Vector3& pos, const Vector3& rotation, const Vector3& scale);
    //  Create a Translation, Rotation(Quat) and Scaleing Matrix
    static Matrix4 TRS(const Vector3& pos, const Quaternion& rotation, const Vector3& scale);
	//	Creates a Transpose of the matrix
	static Matrix4 Transpose(const Matrix4& mat);

	Vector3  Transform(const Vector3& vector)const;
	Vector3  TransformPoint(const Vector3& point)const;

	//--Accessors--
	void    SetRow(int index, const Vector4& vector);
	void    SetColumn(int index, const Vector4& vector);
	Vector3 GetRow(int index)const;
	Vector3 GetColumn(int index)const;
	void	Decompose(Vector3& position, Quaternion& rotation, Vector3& scale)const;
	float	Trace()const;

	std::string ToString()const;

private:
	static Matrix3 Get3x3(const Matrix4& mat, unsigned int row, unsigned int col);
};