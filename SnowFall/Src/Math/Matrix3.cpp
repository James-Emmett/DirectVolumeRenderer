#include "Math/Matrix3.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Mathf.h"
#include <sstream>

#define MAT3_I(row, col) (row + (col * 3))

Matrix3::Matrix3()
{

}

Matrix3::Matrix3(const Matrix3& mat)
{
    m[0] = mat.m[0]; m[1] = mat.m[1];  m[2] = mat.m[2];  m[3] = mat.m[3]; // Col 0
    m[4] = mat.m[0]; m[5] = mat.m[5];  m[6] = mat.m[6];  m[7] = mat.m[7]; // Col 1
    m[8] = mat.m[0]; m[9] = mat.m[9];  m[10] = mat.m[10]; m[11] = mat.m[11]; // Col 2
    m[12] = mat.m[0]; m[13] = mat.m[10]; m[14] = mat.m[14]; m[15] = mat.m[15]; // Col 3
}

Matrix3::Matrix3(float m00, float m01, float m02, float m03, float m04, float m05, float m06, float m07,float m08)
{
    m[0] = m00; m[1] = m01; m[2] = m02; // 1st Col 
    m[3] = m03; m[4] = m04; m[5] = m05; // 2nd Col
    m[6] = m06; m[7] = m07; m[8] = m08; // 3rd Col
}

Matrix3::~Matrix3()
{
    // Nothing
}

bool Matrix3::operator==(const Matrix3& mat) const
{
    for (unsigned int i = 0; i < 9; ++i)
    {
        if (!Mathf::IsEqual(m[i], mat.m[i]))
        {
            return false;
        }
    }
    return true;
}

bool Matrix3::operator!=(const Matrix3& mat) const
{
    for (unsigned int i = 0; i < 9; ++i)
    {
        if (!Mathf::IsEqual(m[i], mat.m[i]))
        {
            return true;
        }
    }
    return false;
}

float& Matrix3::operator[](const unsigned int i)
{
    return m[i];
}

float Matrix3::operator[](const unsigned int i) const
{
    return m[i];
}

Matrix3& Matrix3::operator=(const Matrix3& mat)
{
    if (this != &mat)
    {
        for (int i = 0; i < 9; ++i)
        {
            m[i] = mat.m[i];
        }
    }

    return *this;
}

Matrix3 Matrix3::operator+(const Matrix3& mat) const
{
    Matrix3 product;
    for (size_t i = 0; i < 16; i++)
    {
        product.m[i] = m[i] + mat.m[i];
    }
    return product;
}

Matrix3 Matrix3::operator-(const Matrix3& mat) const
{
    Matrix3 product;
    for (size_t i = 0; i < 16; i++)
    {
        product.m[i] = m[i] - mat.m[i];
    }
    return product;
}

Matrix3 Matrix3::operator*(const Matrix3& mat) const
{
    Matrix3 product;
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            int i = r + (c * 4);
            product.m[i] = m[i] - mat.m[i];
            for (int mid = 0; mid < 4; mid++)
            {
                product.m[i] += m[MAT3_I(r, mid)] * mat.m[MAT3_I(mid, c)];
            }
        }
    }
    return product;
}

Matrix3 Matrix3::operator*(const float s)
{
    Matrix3 product;
    for (size_t i = 0; i < 16; i++)
    {
        product.m[i] = m[i] * s;
    }
    return product;
}

Matrix3& Matrix3::operator+=(const Matrix3& mat)
{
    for (size_t i = 0; i < 16; i++)
    {
        m[i] += mat.m[i];
    }
    return *this;
}

Matrix3& Matrix3::operator-=(const Matrix3& mat)
{
    for (size_t i = 0; i < 16; i++)
    {
        m[i] -= mat.m[i];
    }
    return *this;
}

Matrix3& Matrix3::operator*=(const Matrix3& mat)
{
    Matrix3 product = *this;

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            int i = MAT3_I(row, col);
            m[i] = 0.0f;
            for (int mid = 0; mid < 4; mid++)
            {
                m[i] += product.m[MAT3_I(row, mid)] * mat.m[MAT3_I(mid, col)];
            }
        }
    }

    return *this;
}

Matrix3& Matrix3::operator*=(const float s)
{
    for (size_t i = 0; i < 16; i++)
    {
        m[i] *= s;
    }
    return *this;
}

float& Matrix3::operator()(unsigned int row, unsigned int col)
{
    return m[MAT3_I(row, col)];
}

float Matrix3::operator()(unsigned int row, unsigned int col) const
{
    return m[MAT3_I(row, col)];
}

Matrix3 Matrix3::Minor(const Matrix3& mat)
{
    Matrix3 product;
    product[0] = (mat[4] * mat[8] - mat[5] * mat[7]);
    product[1] = (mat[3] * mat[8] - mat[5] * mat[6]);
    product[2] = (mat[3] * mat[7] - mat[4] * mat[6]);

    product[3] = (mat[1] * mat[8] - mat[2] * mat[7]);
    product[4] = (mat[0] * mat[8] - mat[2] * mat[6]);
    product[5] = (mat[0] * mat[7] - mat[1] * mat[6]);

    product[6] = (mat[1] * mat[5] - mat[2] * mat[4]);
    product[7] = (mat[0] * mat[5] - mat[2] * mat[3]);
    product[8] = (mat[0] * mat[4] - mat[1] * mat[3]);

    return product;
}

Matrix3 Matrix3::Cofactor(const Matrix3& mat)
{
    Matrix3 product;
    product[0] = Mathf::Abs((mat[4] * mat[8] - mat[5] * mat[7]));
    product[1] = -(mat[3] * mat[8] - mat[5] * mat[6]);
    product[2] = Mathf::Abs((mat[3] * mat[7] - mat[4] * mat[6]));

    product[3] = -(mat[1] * mat[8] - mat[2] * mat[7]);
    product[4] = Mathf::Abs((mat[0] * mat[8] - mat[2] * mat[6]));
    product[5] = -(mat[0] * mat[7] - mat[1] * mat[6]);

    product[6] = Mathf::Abs((mat[1] * mat[5] - mat[2] * mat[4]));
    product[7] = -(mat[0] * mat[5] - mat[2] * mat[3]);
    product[8] = Mathf::Abs((mat[0] * mat[4] - mat[1] * mat[3]));

    return product;
}

Matrix3 Matrix3::Adjoint(const Matrix3& mat)
{
    //Claculate the Matrix of minors, apply the
    //lattice of + and - (cofactor) then transpose.
    //I do all in 1 step!

    Matrix3 product;

    product[0] = Mathf::Abs((mat[4] * mat[8] - mat[5] * mat[7]));
    product[3] = -(mat[3] * mat[8] - mat[5] * mat[6]);
    product[6] = Mathf::Abs((mat[3] * mat[7] - mat[4] * mat[6]));

    product[1] = -(mat[1] * mat[8] - mat[2] * mat[7]);
    product[4] = Mathf::Abs((mat[0] * mat[8] - mat[2] * mat[6]));
    product[7] = -(mat[0] * mat[7] - mat[1] * mat[6]);

    product[2] = Mathf::Abs((mat[1] * mat[5] - mat[2] * mat[4]));
    product[5] = -(mat[0] * mat[5] - mat[2] * mat[3]);
    product[8] = Mathf::Abs((mat[0] * mat[4] - mat[1] * mat[3]));

    return product;
}

//http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche23.html
float Matrix3::Determinant(const Matrix3& mat)
{
    return  mat[0] * (mat[4] * mat[8] - mat[5] * mat[7])
            - mat[1] * (mat[3] * mat[8] - mat[5] * mat[6])
            + mat[2] * (mat[3] * mat[7] - mat[4] * mat[6]);
}

Matrix3 Matrix3::Inverse(const Matrix3& mat)
{
    float det = mat[0] * (mat[4] * mat[8] - mat[5] * mat[7])
                - mat[1] * (mat[3] * mat[8] - mat[5] * mat[6])
                + mat[2] * (mat[3] * mat[7] - mat[4] * mat[6]);

    Matrix3 product;

    product[0] = Mathf::Abs((mat[4] * mat[8] - mat[5] * mat[7]));
    product[3] = -(mat[3] * mat[8] - mat[5] * mat[6]);
    product[6] = Mathf::Abs((mat[3] * mat[7] - mat[4] * mat[6]));

    product[1] = -(mat[1] * mat[8] - mat[2] * mat[7]);
    product[4] = Mathf::Abs((mat[0] * mat[8] - mat[2] * mat[6]));
    product[7] = -(mat[0] * mat[7] - mat[1] * mat[6]);

    product[2] = Mathf::Abs((mat[1] * mat[5] - mat[2] * mat[4]));
    product[5] = -(mat[0] * mat[5] - mat[2] * mat[3]);
    product[8] = Mathf::Abs((mat[0] * mat[4] - mat[1] * mat[3]));

    return product * (1.0f / det);
}

Matrix3 Matrix3::RotateX(float angle)
{
    float fsin = Mathf::Sin(angle);
    float fcos = Mathf::Cos(angle);

    Matrix3 product;
    product.m[0] = 1.0f;
    product.m[4] = fcos;
    product.m[5] = fsin;
    product.m[7] = -fsin;
    product.m[8] = fcos;

    return product;
}

Matrix3 Matrix3::RotateY(float angle)
{
    float fSin = sinf(angle);
    float fCos = cosf(angle);

    Matrix3 product;
    product.m[0] = fCos;
    product.m[2] = -fSin;
    product.m[4] = 1.0f;
    product.m[6] = fSin;
    product.m[8] = fCos;
    return product;
}

Matrix3 Matrix3::RotateZ(float angle)
{
    float fsin = Mathf::Sin(angle);
    float fcos = Mathf::Cos(angle);

    Matrix3 product;
    product.m[0] = fcos;
    product.m[1] = -fsin;
    product.m[3] = fsin;
    product.m[4] = fcos;
    product.m[8] = 1.0f;

    return product;
}

Matrix3 Matrix3::Rotate(const Vector3& euler)
{
    return RotateZ(euler.z) * RotateX(euler.x) * RotateY(euler.y);
}

Matrix3 Matrix3::Rotate(const Quaternion& rotation)
{
    Matrix3 product;

    float s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;

    //Normalised rotation s = 2.0f as 2.0/1.0f = 2.0f;
    s = 2.0f / (rotation.x * rotation.x + rotation.y * rotation.y + rotation.z * rotation.z + rotation.w * rotation.w);

    xs = s * rotation.x;  ys = s * rotation.y; zs = s * rotation.z;
    wx = xs * rotation.w; wy = ys * rotation.w; wz = zs * rotation.w;
    xx = xs * rotation.x; xy = ys * rotation.x; xz = zs * rotation.x;
    yy = ys * rotation.y; yz = zs * rotation.y; zz = zs * rotation.z;

    product.m[0] = 1.0f - (yy + zz);
    product.m[3] = xy - wz;
    product.m[6] = xz + wy;

    product.m[1] = xy + wz;
    product.m[4] = 1.0f - (xx + zz);
    product.m[7] = yz - wx;

    product.m[2] = xz - wy;
    product.m[5] = yz - wx;
    product.m[8] = 1.0f - (xx + yy);

    return product;
}

Matrix3 Matrix3::Scale(const Vector3& scale)
{
    Matrix3 product;
    product.m[0] = scale.x;
    product.m[5] = scale.y;
    product.m[10] = scale.z;
    return product;
}

Matrix3 Matrix3::Transpose(const Matrix3& mat)
{
    Matrix3 transpose;
    for (unsigned int row = 0; row < 3; ++row)
    {
        for (unsigned int col = 0; col < 3; ++col)
        {
            transpose.m[MAT3_I(row, col)] = mat.m[MAT3_I(col, row)];
        }
    }

    return transpose;
}

Vector3 Matrix3::Transform(const Vector3& vector) const
{
    Vector3 product;
    product.x = vector.x * m[0] + vector.y * m[3] + vector.z * m[6];
    product.y = vector.x * m[1] + vector.y * m[4] + vector.z * m[5];
    product.z = vector.x * m[6] + vector.y * m[7] + vector.z * m[8];
    return product;
}

void Matrix3::SetRow(int index, const Vector3& vector)
{
    m[MAT3_I(index, 0)] = vector.x;
    m[MAT3_I(index, 1)] = vector.y;
    m[MAT3_I(index, 2)] = vector.z;
}

void Matrix3::SetColumn(int index, const Vector3& vector)
{
    m[MAT3_I(0, index)] = vector.x;
    m[MAT3_I(1, index)] = vector.y;
    m[MAT3_I(2, index)] = vector.z;
}

Vector3 Matrix3::GetRow(int index) const
{
    Vector3 vector;
    vector.x = m[MAT3_I(index, 0)];
    vector.y = m[MAT3_I(index, 1)];
    vector.z = m[MAT3_I(index, 2)];
    return vector;
}

Vector3 Matrix3::GetColumn(int index)const
{
    Vector3 vector;
    vector.x = m[MAT3_I(0, index)];
    vector.y = m[MAT3_I(1, index)];
    vector.z = m[MAT3_I(2, index)];
    return vector;
}

float Matrix3::Trace()const
{
    return m[0] + m[4] + m[8];
}

std::string Matrix3::ToString()const
{
    std::stringstream ss;
    ss << "Matrix3: " << "\n"
        << m[0] << "  " << m[3] << "  " << m[6]  << "\n"
        << m[1] << "  " << m[4] << "  " << m[7]  << "\n"
        << m[2] << "  " << m[5] << "  " << m[8] << "\n";

    return std::string(ss.str().c_str());
}