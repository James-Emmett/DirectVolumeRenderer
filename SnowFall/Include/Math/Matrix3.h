//Note
/*
	3X3 matrix, Column Major, Left-Handed (Positive rotation is clockwise)
	Data Layout is LINEAR

	Linear memory layout:
	[ a b c ]
	[ d e f ]
	[ g h i ]


	Multiplication is thus:

	[ a b c ]     [x]
	[ d e f ] *   [y] = [x*a + y*b + z*c] and so on
	[ g h i ]     [z]

	Multiplication Order in c++ is: T * R * S

	Data can be accessed as: row + (col * 4)
*/
#pragma once
#include "Vector3.h"
#include "Quaternion.h"

class Matrix3
{
public:
	static Matrix3 Identiy;
	static Matrix3 Zero;
	float m[16] = { 0 };

public:
	Matrix3();
	Matrix3(const Matrix3& mat);
	Matrix3(float m00, float m01, float m02, // 1st col 
		float m03, float m04, float m05, // 2nd col
		float m06, float m07, float m08);// 3rd Col        
	~Matrix3();

public:
	bool	operator==(const Matrix3& mat)const;
	bool	operator!=(const Matrix3& mat)const;
	float& operator[](const unsigned int i);
	float	operator[](const unsigned int i)const;
	Matrix3& operator=(const Matrix3& mat);
	Matrix3	operator+(const Matrix3& mat)const;
	Matrix3 operator-(const Matrix3& mat)const;
	Matrix3 operator*(const Matrix3& mat)const;
	Matrix3 operator*(const float s);

	Matrix3& operator+=(const Matrix3& mat);
	Matrix3& operator-=(const Matrix3& mat);
	Matrix3& operator*=(const Matrix3& mat);
	Matrix3& operator*=(const float s);

	float& operator()(unsigned int row, unsigned int col);
	float	operator()(unsigned int row, unsigned int col)const;

public:
	//	Creates Matrix of Minors
	static Matrix3 Minor(const Matrix3& mat);
	//	Creates Cofactor of matrix, lattice + -
	static Matrix3 Cofactor(const Matrix3& mat);
	//	Creates Adjoint of matix (transpose of cofactor)
	static Matrix3 Adjoint(const Matrix3& mat);
	//	The determinant of the matrix. (Read Only)
	static float   Determinant(const Matrix3& mat);
	//  Creates a inversed matrix
	static Matrix3 Inverse(const Matrix3& mat);
	//  Create a Rotation Matrix on X axis from Euler in Radians.
	static Matrix3 RotateX(float angle);
	//  Create a Rotation Matrix on Y axis from Euler in Radians.
	static Matrix3 RotateY(float angle);
	//  Create a Rotation Matrix on Z axis from Euler in Radians.
	static Matrix3 RotateZ(float angle);
	//  Create a Rotation Matrix from Euler Angles.
	static Matrix3 Rotate(const Vector3& euler);
	//  Create a Rotation Matrix from quaternion.
	static Matrix3 Rotate(const Quaternion& rotation);
	//  Creates a scaling matrix.
	static Matrix3 Scale(const Vector3& scale);
	//	Creates a Transpose of the matrix
	static Matrix3 Transpose(const Matrix3& mat);

	Vector3  Transform(const Vector3& vector)const;

	//--Accessors--
	void    SetRow(int index, const Vector3& vector);
	void    SetColumn(int index, const Vector3& vector);
	Vector3 GetRow(int index)const;
	Vector3 GetColumn(int index)const;
	float	Trace()const;

	std::string ToString()const;
};