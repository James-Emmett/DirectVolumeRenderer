#include "Math/Matrix4.h"
#include "Math/Matrix3.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Quaternion.h"
#include "Math/Mathf.h"
#include <sstream>

#define MAT4_I(row, col) (row + (col * 4))
Matrix4 Matrix4::Identiy = Matrix4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
Matrix4 Matrix4::Zero = Matrix4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);


Matrix4::Matrix4()
{

}

Matrix4::Matrix4(const Matrix4& mat)
{
    m[0]  = mat.m[0]; m[1]  = mat.m[1];  m[2]  = mat.m[2];  m[3]  = mat.m[3]; // Col 0
    m[4]  = mat.m[4]; m[5]  = mat.m[5];  m[6]  = mat.m[6];  m[7]  = mat.m[7]; // Col 1
    m[8]  = mat.m[8]; m[9]  = mat.m[9];  m[10] = mat.m[10]; m[11] = mat.m[11]; // Col 2
    m[12] = mat.m[12]; m[13] = mat.m[13]; m[14] = mat.m[14]; m[15] = mat.m[15]; // Col 3
}

Matrix4::Matrix4(float m00, float m01, float m02, float m03, float m04, float m05, float m06, float m07,
                float m08, float m09, float m10, float m11, float m12, float m13, float m14, float m15)
{
    m[0]  = m00; m[1]  = m01; m[2]  = m02; m[3]  = m03; // Col 0
    m[4]  = m04; m[5]  = m05; m[6]  = m06; m[7]  = m07; // Col 1
    m[8]  = m08; m[9]  = m09; m[10] = m10; m[11] = m11; // Col 2
    m[12] = m12; m[13] = m13; m[14] = m14; m[15] = m15; // Col 3
}
            
Matrix4::~Matrix4()
{
    // Nothing
}

bool Matrix4::operator==(const Matrix4& mat) const
{
    for (unsigned int i = 0; i < 16; ++i)
    {
        if (!Mathf::IsEqual(m[i], mat.m[i]))
        {
            return false;
        }
    }
    return true;
}

bool Matrix4::operator!=(const Matrix4& mat) const
{
    for (unsigned int i = 0; i < 16; ++i)
    {
        if (!Mathf::IsEqual(m[i], mat.m[i]))
        {
            return true;
        }
    }
    return false;
}

float& Matrix4::operator[](const unsigned int i)
{
    return m[i];
}

float Matrix4::operator[](const unsigned int i) const
{
    return m[i];
}

Matrix4& Matrix4::operator=(const Matrix4& mat)
{
    if (this != &mat)
    {
        for (int i = 0; i < 16; ++i)
        {
            m[i] = mat.m[i];
        }
    }

    return *this;
}

Matrix4 Matrix4::operator+(const Matrix4& mat) const
{
    Matrix4 product;
    for (size_t i = 0; i < 16; i++)
    {
        product.m[i] = m[i] + mat.m[i];
    }
    return product;
}

Matrix4 Matrix4::operator-(const Matrix4& mat) const
{
    Matrix4 product;
    for (size_t i = 0; i < 16; i++)
    {
        product.m[i] = m[i] - mat.m[i];
    }
    return product;
}

Matrix4 Matrix4::operator*(const Matrix4& mat) const
{
    Matrix4 product;
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            int i = MAT4_I(r, c);
            product.m[i] = 0.0f;
            for (int mid = 0; mid < 4; mid++)
            {
                product.m[i] += m[MAT4_I(r, mid)] * mat.m[MAT4_I(mid, c)];
            }
        }
    }
    return product;
}

Matrix4 Matrix4::operator*(const float s)
{
    Matrix4 product;
    for (size_t i = 0; i < 16; i++)
    {
        product.m[i] = m[i] * s;
    }
    return product;
}

Matrix4& Matrix4::operator+=(const Matrix4& mat)
{
    for (size_t i = 0; i < 16; i++)
    {
        m[i] += mat.m[i];
    }
    return *this;
}

Matrix4& Matrix4::operator-=(const Matrix4& mat)
{
    for (size_t i = 0; i < 16; i++)
    {
        m[i] -= mat.m[i];
    }
    return *this;
}

Matrix4& Matrix4::operator*=(const Matrix4& mat)
{
    Matrix4 product = *this;

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            int i = MAT4_I(row, col);
            m[i] = 0.0f;
            for (int mid = 0; mid < 4; mid++)
            {
                m[i] += product.m[MAT4_I(row, mid)] * mat.m[MAT4_I(mid, col)];
            }
        }
    }

    return *this;
}

Matrix4& Matrix4::operator*=(const float s)
{
    for (size_t i = 0; i < 16; i++)
    {
        m[i] *= s;
    }
    return *this;
}

float& Matrix4::operator()(unsigned int row, unsigned int col)
{
    return m[MAT4_I(row, col)];
}

float Matrix4::operator()(unsigned int row, unsigned int col) const
{
    return m[MAT4_I(row, col)];
}

Matrix4 Matrix4::Minor(const Matrix4& mat)
{
    Matrix4 product;
    for (unsigned int row = 0; row < 4; row++)
    {
        for (unsigned int col = 0; col < 4; col++)
        {
            //Minors is the deterinant of the 3x3 matrix
            product[MAT4_I(row, col)] = Matrix3::Determinant(Get3x3(mat, row, col));
        }
    }
    return product;
}

Matrix4 Matrix4::Cofactor(const Matrix4& mat)
{
    Matrix4 minor = Minor(mat);
    Matrix4 product;
    product[0] = Mathf::Abs(minor[0]);
    product[1] = -minor[1];
    product[2] = Mathf::Abs(minor[2]);
    product[3] = -minor[3];

    product[4] = -minor[4];
    product[5] = Mathf::Abs(minor[5]);
    product[6] = -minor[6];
    product[7] = Mathf::Abs(minor[7]);

    product[8] = Mathf::Abs(minor[8]);
    product[9] = -minor[9];
    product[10] = Mathf::Abs(minor[10]);
    product[11] = -minor[11];

    product[12] = -minor[12];
    product[13] = Mathf::Abs(minor[13]);
    product[14] = -minor[14];
    product[15] = Mathf::Abs(minor[15]);
    return product;
}

Matrix4 Matrix4::Adjoint(const Matrix4& mat)
{
    // Still Need to speed this up.
    return Transpose(Cofactor(mat));
}

//http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche23.html
float Matrix4::Determinant(const Matrix4& mat)
{
    return mat[0] * mat[5] * mat[10] * mat[15] + mat[0] * mat[6] * mat[11] * mat[13] + mat[0] * mat[7] * mat[9] * mat[14]
        + mat[1] * mat[4] * mat[11] * mat[14] + mat[1] * mat[6] * mat[8] * mat[15] + mat[1] * mat[7] * mat[10] * mat[12]
        + mat[2] * mat[4] * mat[9] * mat[15] + mat[2] * mat[5] * mat[11] * mat[12] + mat[2] * mat[7] * mat[8] * mat[13]
        + mat[3] * mat[4] * mat[10] * mat[13] + mat[3] * mat[5] * mat[8] * mat[14] + mat[3] * mat[6] * mat[9] * mat[12]
        - mat[0] * mat[5] * mat[11] * mat[14] - mat[0] * mat[6] * mat[9] * mat[15] - mat[0] * mat[7] * mat[10] * mat[13]
        - mat[1] * mat[4] * mat[10] * mat[15] - mat[1] * mat[6] * mat[11] * mat[12] - mat[1] * mat[7] * mat[8] * mat[14]
        - mat[2] * mat[4] * mat[11] * mat[13] - mat[2] * mat[5] * mat[8] * mat[15] - mat[2] * mat[7] * mat[9] * mat[12]
        - mat[3] * mat[4] * mat[9] * mat[14] - mat[3] * mat[5] * mat[10] * mat[12] - mat[3] * mat[6] * mat[8] * mat[13];
}

Matrix4 Matrix4::Inverse(const Matrix4& mat)
{
    float det = Determinant(mat);

    if (Mathf::IsZero(det))
    {
        return Matrix4();
    }

    float invDet = 1.0f / det;

    Matrix4 product;
    product[0]  = (mat[5] * mat[10] * mat[15] + mat[6] * mat[11] * mat[13] + mat[7] * mat[9] * mat[14] - mat[5] * mat[11] * mat[14] - mat[6] * mat[9] * mat[15] - mat[7] * mat[10] * mat[13]) * invDet;
    product[1]  = (mat[1] * mat[11] * mat[14] + mat[2] * mat[9] * mat[15] + mat[3] * mat[10] * mat[13] - mat[1] * mat[10] * mat[15] - mat[2] * mat[11] * mat[13] - mat[3] * mat[9] * mat[14]) * invDet;
    product[2]  = (mat[1] * mat[6] * mat[15] + mat[2] * mat[7] * mat[13] + mat[3] * mat[5] * mat[14] - mat[1] * mat[7] * mat[14] - mat[2] * mat[5] * mat[15] - mat[3] * mat[6] * mat[13]) * invDet;
    product[3]  = (mat[1] * mat[7] * mat[10] + mat[2] * mat[5] * mat[11] + mat[3] * mat[6] * mat[9] - mat[1] * mat[6] * mat[11] - mat[2] * mat[7] * mat[9] - mat[3] * mat[5] * mat[10]) * invDet;
    product[4]  = (mat[4] * mat[11] * mat[14] + mat[6] * mat[8] * mat[15] + mat[7] * mat[10] * mat[12] - mat[4] * mat[10] * mat[15] - mat[6] * mat[11] * mat[12] - mat[7] * mat[8] * mat[14]) * invDet;
    product[5]  = (mat[0] * mat[10] * mat[15] + mat[2] * mat[11] * mat[12] + mat[3] * mat[8] * mat[14] - mat[0] * mat[11] * mat[14] - mat[2] * mat[8] * mat[15] - mat[3] * mat[10] * mat[12]) * invDet;
    product[6]  = (mat[0] * mat[7] * mat[14] + mat[2] * mat[4] * mat[15] + mat[3] * mat[6] * mat[12] - mat[0] * mat[6] * mat[15] - mat[2] * mat[7] * mat[12] - mat[3] * mat[4] * mat[14]) * invDet;
    product[7]  = (mat[0] * mat[6] * mat[11] + mat[2] * mat[7] * mat[8] + mat[3] * mat[4] * mat[10] - mat[0] * mat[7] * mat[10] - mat[2] * mat[4] * mat[11] - mat[3] * mat[6] * mat[8]) * invDet;
    product[8]  = (mat[4] * mat[9] * mat[15] + mat[5] * mat[11] * mat[12] + mat[7] * mat[8] * mat[13] - mat[4] * mat[11] * mat[13] - mat[5] * mat[8] * mat[15] - mat[7] * mat[9] * mat[12]) * invDet;
    product[9]  = (mat[0] * mat[11] * mat[13] + mat[1] * mat[8] * mat[15] + mat[3] * mat[9] * mat[12] - mat[0] * mat[9] * mat[15] - mat[1] * mat[11] * mat[12] - mat[3] * mat[8] * mat[13]) * invDet;
    product[10] = (mat[0] * mat[5] * mat[15] + mat[1] * mat[7] * mat[12] + mat[3] * mat[4] * mat[13] - mat[0] * mat[7] * mat[13] - mat[1] * mat[4] * mat[15] - mat[3] * mat[5] * mat[12]) * invDet;
    product[11] = (mat[0] * mat[7] * mat[9] + mat[1] * mat[4] * mat[11] + mat[3] * mat[5] * mat[8] - mat[0] * mat[5] * mat[11] - mat[1] * mat[7] * mat[8] - mat[3] * mat[4] * mat[9]) * invDet;
    product[12] = (mat[4] * mat[10] * mat[13] + mat[5] * mat[8] * mat[14] + mat[6] * mat[9] * mat[12] - mat[4] * mat[9] * mat[14] - mat[5] * mat[10] * mat[12] - mat[6] * mat[8] * mat[13]) * invDet;
    product[13] = (mat[0] * mat[9] * mat[14] + mat[1] * mat[10] * mat[12] + mat[2] * mat[8] * mat[13] - mat[0] * mat[10] * mat[13] - mat[1] * mat[8] * mat[14] - mat[2] * mat[9] * mat[12]) * invDet;
    product[14] = (mat[0] * mat[6] * mat[13] + mat[1] * mat[4] * mat[14] + mat[2] * mat[5] * mat[12] - mat[0] * mat[5] * mat[14] - mat[1] * mat[6] * mat[12] - mat[2] * mat[4] * mat[13]) * invDet;
    product[15] = (mat[0] * mat[5] * mat[10] + mat[1] * mat[6] * mat[8] + mat[2] * mat[4] * mat[9] - mat[0] * mat[6] * mat[9] - mat[1] * mat[4] * mat[10] - mat[2] * mat[5] * mat[8]) * invDet;


    return product;
}

Matrix4 Matrix4::RotateX(float angle)
{
    float fsin = Mathf::Sin(angle);
    float fcos = Mathf::Cos(angle);

    Matrix4 product;
    product.m[0] = 1.0f;
    product.m[5] = fcos;
    product.m[6] = fsin;
    product.m[9] = -fsin;
    product.m[10] = fcos;
    product.m[15] = 1.0f;

    return product;
}

Matrix4 Matrix4::RotateY(float angle)
{
    float fSin = sinf(angle);
    float fCos = cosf(angle);

    Matrix4 product;
    product.m[0]  = fCos;
    product.m[2]  = -fSin;
    product.m[5]  = 1.0f;
    product.m[8]  = fSin;
    product.m[10] = fCos;
    product.m[15] = 1.0f;

    return product;
}

Matrix4 Matrix4::RotateZ(float angle)
{
    float fsin = Mathf::Sin(angle);
    float fcos = Mathf::Cos(angle);

    Matrix4 product;
    product.m[0] = fcos;
    product.m[1] = fsin;
    product.m[4] = -fsin;
    product.m[5] = fcos;
    product.m[10] = 1.0f;
    product.m[15] = 1.0f;

    return product;
}

Matrix4 Matrix4::Rotate(const Vector3& euler)
{
    return RotateZ(euler.z) * RotateX(euler.x) * RotateY(euler.y);
}

Matrix4 Matrix4::Rotate(const Quaternion& rotation)
{
    Quaternion q = Quaternion::Normalize(rotation);
    Matrix4 product;

    float xx = q.x * q.x;
    float yy = q.y * q.y;
    float zz = q.z * q.z;

    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;

    float xw = q.x * q.w;
    float yw = q.y * q.w;
    float zw = q.z * q.w;

    product.m[0] = 1.0f - 2.0f * yy - 2.0f * zz;
    product.m[1] = 2.0f * xy - 2.0f * zw;
    product.m[2] = 2.0f * xz + 2.0f * yw;
    product.m[3] = 0.0f;

    product.m[4] = 2.0f * xy + 2.0f * zw;
    product.m[5] = 1.0f - 2.0f * xx - 2.0f * zz;
    product.m[6] = 2.0f * yz - 2.0f * xw;
    product.m[7] = 0.0f;

    product.m[8] = 2.0f * xz - 2.0f * yw;
    product.m[9] = 2.0f * yz + 2.0f * xw;
    product.m[10] = 1.0f - 2.0f * xx - 2.0f * yy;
    product.m[11] = 0.0f;

    product.m[12] = 0.0f;
    product.m[13] = 0.0f;
    product.m[14] = 0.0f;
    product.m[15] = 1.0f;

    return product;

    //product.m[0] = 1.0f - (yy + zz);
    //product.m[4] = xy + wz;
    //product.m[8] = xz - wy;
    //product.m[12] = 0.0f;

    //product.m[1] = xy + wz;
    //product.m[5] = 1.0f - (xx + zz);
    //product.m[9] = yz - wx;
    //product.m[13] = 0.0f;

    //product.m[2] = xz - wy;
    //product.m[6] = yz + wx;
    //product.m[10] = 1.0f - (xx + yy);
    //product.m[14] = 0.0f;

    //product.m[3] = 0.0f;
    //product.m[7] = 0.0f;
    //product.m[11] = 0.0f;
    //product.m[15] = 1.0f;

    //float xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;

    //xs = q.x + q.x;
    //ys = q.y + q.y;
    //zs = q.z + q.z;
    //wx = q.w * xs;
    //wy = q.w * ys;
    //wz = q.w * zs;
    //xx = q.x * xs;
    //xy = q.x * ys;
    //xz = q.x * zs;
    //yy = q.y * ys;
    //yz = q.y * zs;
    //zz = q.z * zs;

    //product.m[0] = 1.0f - (yy + zz);
    //product.m[4] = xy - wz;
    //product.m[8] = xz + wy;
    //product.m[12] = 0.0f;

    //product.m[1] = xy + wz;
    //product.m[5] = 1.0f - (xx + zz);
    //product.m[9] = yz - wx;
    //product.m[13] = 0.0f;

    //product.m[2] = xz - wy;
    //product.m[6] = yz + wx;
    //product.m[10] = 1.0f - (xx + yy);
    //product.m[14] = 0.0f;

    //product.m[3] = 0.0f;
    //product.m[7] = 0.0f;
    //product.m[11] = 0.0f;
    //product.m[15] = 1.0f;

    //return product;
}

Matrix4 Matrix4::LookAt(const Vector3& from, const Vector3& to, const Vector3& up)
{
    //Create the new Basis vectors!
    Vector3 Z = (to - from).Normalize();
    Vector3 X = Vector3::Cross(up, Z).Normalize();
    Vector3 Y = Vector3::Cross(Z, X);
    Matrix4 product;

    product.m[0] = X.x;
    product.m[1] = Y.x;
    product.m[2] = Z.x;
    product.m[3] = 0.0f;

    product.m[4] = X.y;
    product.m[5] = Y.y;
    product.m[6] = Z.y;
    product.m[7] = 0.0f;

    product.m[8] = X.z;
    product.m[9] = Y.z;
    product.m[10] = Z.z;
    product.m[11] = 0.0f;

    //Project!
    product.m[12] = -(Vector3::Dot(X, from));
    product.m[13] = -(Vector3::Dot(Y, from));
    product.m[14] = -(Vector3::Dot(Z, from));
    product.m[15] = 1.0f;

    return product;
}

Matrix4 Matrix4::Orthographic(int width, int height, float zNear, float zFar)
{
    if (zNear == zFar) { zFar = zNear + 0.1f; }
    if (width <= 0.0f) { width = 1; }
    if (height <= 0.0f) { height = 1; }

    Matrix4 product;
    product.m[0] = 2.0f / width;
    product.m[1] = 0.0f;
    product.m[2] = 0.0f;
    product.m[3] = 0.0f;

    product.m[4] = 0.0f;
    product.m[5] = 2.0f / height;
    product.m[6] = 0.0f;
    product.m[7] = 0.0f;

    product.m[8] = 0.0f;
    product.m[9] = 0.0f;
    product.m[10] = 1.0f / (zFar - zNear);
    product.m[11] = 0.0f;

    product.m[12] = 0.0f;
    product.m[13] = 0.0f;
    product.m[14] = -zNear / (zNear - zFar);
    product.m[15] = 1.0f;

    return product;
}

Matrix4 Matrix4::PerspectiveFov(float fov, float aspect, float zNear, float zFar)
{
    float tanY = Mathf::Tan(fov / 2.0f);
    if (0.0f == tanY) tanY = 0.001f;
    float yScale = 1.0f / tanY;

    if (0.0f == aspect) aspect = 0.001f;
    float xScale = yScale / aspect;

    if (0.0f == aspect) { aspect = 0.001f; }
    if (zNear == zFar)   zFar = zNear + 0.1f;

    float range = zFar / (zFar - zNear);

    Matrix4 product;

    product.m[0] = xScale;
    product.m[1] = 0.0f;
    product.m[2] = 0.0f;
    product.m[3] = 0.0f;

    product.m[4] = 0.0f;
    product.m[5] = yScale;
    product.m[6] = 0.0f;
    product.m[7] = 0.0f;

    product.m[8] = 0.0f;
    product.m[9] = 0.0f;
    product.m[10] = range;
    product.m[11] = 1.0f; // Copies Z to W for Z divide, +1 as Left-Handed

    product.m[12] = 0.0f;
    product.m[13] = 0.0f;
    product.m[14] = -range * zNear;
    product.m[15] = 0.0f;

    return product;
}

Matrix4 Matrix4::Perspective(float width, float height, float zNear, float zFar)
{
    float twoNear = zNear * zNear;
    float range = zFar / (zFar - zNear);

    Matrix4 product;

    product.m[0] = twoNear / width;
    product.m[1] = 0.0f;
    product.m[2] = 0.0f;
    product.m[3] = 0.0f;

    product.m[4] = 0.0f;
    product.m[5] = twoNear / height;
    product.m[6] = 0.0f;
    product.m[7] = 0.0f;

    product.m[8] = 0.0f; // N/A as its just 1 anyways?
    product.m[9] = 0.0f; // N/A ^
    product.m[10] = range;
    product.m[11] = 1.0f; // Copies Z to W for Z divide, +1 as Left-Handed

    product.m[12] = 0.0f;
    product.m[13] = 0.0f;
    product.m[14] = -range * zNear;
    product.m[15] = 0.0f;

    return product;
}

Matrix4 Matrix4::PerspectiveOffCenter(float left, float right, float top, float bottom, float zNear, float zFar)
{
    float twoNear      = zNear + zNear;
    float recipWidth    = 1.0f / (right - left);
    float recipHeight   = 1.0f / (top - bottom);
    float range         = zFar / (zFar - zNear);

    Matrix4 product;

    product.m[0] = twoNear * recipWidth; // multiply as its reciprical anyways
    product.m[1] = 0.0f;
    product.m[2] = 0.0f;
    product.m[3] = 0.0f;

    product.m[4] = 0.0f;
    product.m[5] = twoNear * recipHeight;// multiply as its reciprical anyways
    product.m[6] = 0.0f;
    product.m[7] = 0.0f;

    product.m[8] = -(left + right) * recipWidth;
    product.m[9] = -(top + bottom) * recipHeight;
    product.m[10] = range;
    product.m[11] = 1.0f; // Copies Z to W for Z divide, +1 as Left-Handed

    product.m[12] = 0.0f;
    product.m[13] = 0.0f;
    product.m[14] = -range * zNear;
    product.m[15] = 0.0f;

    return product;
}

Matrix4 Matrix4::Scale(const Vector3& scale)
{
    Matrix4 product;
    product.m[0]  = scale.x;
    product.m[5]  = scale.y;
    product.m[10] = scale.z;
    product.m[15] = 1.0f;
    return product;
}

Matrix4 Matrix4::Translate(const Vector3& vector)
{
    Matrix4 product;
    product.m[0] = 1.0f; product.m[1] = 0.0f; product.m[2] = 0.0f;	product.m[3] = 0.0f;
    product.m[4] = 0.0f; product.m[5] = 1.0f; product.m[6] = 0.0f;	product.m[7] = 0.0f;
    product.m[8] = 0.0f; product.m[9] = 0.0f; product.m[10] = 1.0f;	product.m[11] = 0.0f;
    product.m[12] = vector.x; product.m[13] = vector.y; product.m[14] = vector.z;	product.m[15] = 1.0f;
    return product;
}

Matrix4 Matrix4::TRS(const Vector3& pos, const Vector3& rotation, const Vector3& scale)
{
    return Translate(pos) * Rotate(rotation) * Scale(scale);
}

Matrix4 Matrix4::TRS(const Vector3& pos, const Quaternion& rotation, const Vector3& scale)
{
    return Translate(pos) * Rotate(rotation) * Scale(scale);
}

Matrix4 Matrix4::Transpose(const Matrix4& mat)
{
    Matrix4 transpose;
    for (unsigned int row = 0; row < 4; ++row)
    {
        for (unsigned int col = 0; col < 4; ++col)
        {
            transpose.m[MAT4_I(row, col)] = mat.m[MAT4_I(col, row)];
        }
    }

    return transpose;
}

Vector3 Matrix4::Transform(const Vector3& vector) const
{
    Vector3 product;
    product.x = vector.x * m[0] + vector.y * m[4] + vector.z * m[8];
    product.y = vector.x * m[1] + vector.y * m[5] + vector.z * m[9];
    product.z = vector.x * m[2] + vector.y * m[6] + vector.z * m[10];
    return product;
}

Vector3 Matrix4::TransformPoint(const Vector3& point) const
{
    Vector3 product;
    product.x = point.x * m[0] + point.y * m[4] + point.z * m[8] + m[12];
    product.y = point.x * m[1] + point.y * m[5] + point.z * m[9] + m[13];
    product.z = point.x * m[2] + point.y * m[6] + point.z * m[10] + m[14];
    return product;
}

void Matrix4::SetRow(int index, const Vector4& vector)
{
    m[MAT4_I(index, 0)] = vector.x;
    m[MAT4_I(index, 1)] = vector.y;
    m[MAT4_I(index, 2)] = vector.z;
    m[MAT4_I(index, 3)] = vector.w;
}

void Matrix4::SetColumn(int index, const Vector4& vector)
{
    m[MAT4_I(0, index)] = vector.x;
    m[MAT4_I(1, index)] = vector.y;
    m[MAT4_I(2, index)] = vector.z;
    m[MAT4_I(3, index)] = vector.w;
}

Vector3 Matrix4::GetRow(int index) const
{
    Vector3 vector;
    vector.x = m[MAT4_I(index, 0)];
    vector.y = m[MAT4_I(index, 1)];
    vector.z = m[MAT4_I(index, 2)];
    return vector;
}

Vector3 Matrix4::GetColumn(int index)const
{
    Vector3 vector;
    vector.x = m[MAT4_I(0, index)];
    vector.y = m[MAT4_I(1, index)];
    vector.z = m[MAT4_I(2, index)];
    return vector;
}

void Matrix4::Decompose(Vector3& position, Quaternion& rotation, Vector3& scale) const
{
    // Extract Translation
    position = GetRow(3);

    // Extract Scale, Get magnitude also removes the sign?
    scale.x = m[15] * Mathf::Sqrt(m[0] * m[0] + m[1] * m[1] + m[2] * m[2]);
    scale.y = m[15] * Mathf::Sqrt(m[4] * m[4] + m[5] * m[5] + m[6] * m[6]);
    scale.z = m[15] * Mathf::Sqrt(m[8] * m[8] + m[9] * m[9] + m[10] * m[10]);

    // Grab Rotation
    Matrix3 matrix;
    matrix.m[0] = m[0] / scale.x; matrix.m[1] = m[1] / scale.x; matrix.m[2] = m[2] / scale.x;
    matrix.m[3] = m[3] / scale.y; matrix.m[4] = m[4] / scale.y; matrix.m[5] = m[5] / scale.y;
    matrix.m[6] = m[6] / scale.z; matrix.m[7] = m[7] / scale.y; matrix.m[8] = m[8] / scale.y;

    // Should work?
    rotation = Quaternion::FromMatrix(matrix);
}

float Matrix4::Trace() const
{
    return m[0] + m[5] + m[10] + m[15];
}

std::string Matrix4::ToString()const
{
    std::stringstream ss;
    ss  << "Matrix4: " << "\n"
        << m[0] << "  " << m[4] << "  " << m[8] << "  " << m[12] << "\n"
        << m[1] << "  " << m[5] << "  " << m[9] << "  " << m[13] << "\n"
        << m[2] << "  " << m[6] << "  " << m[10] << "  " << m[14] << "\n"
        << m[3] << "  " << m[7] << "  " << m[11] << "  " << m[15] << "\n";

    return std::string(ss.str().c_str());
}

Matrix3 Matrix4::Get3x3(const Matrix4& mat, unsigned int row, unsigned int col)
{
    // Row column elimination to find the 3x3 for detemrinant.
    Matrix3 product;
    int index = 0;

    for (unsigned int i = 0; i < 4; i++)
    {
        for (unsigned int j = 0; j < 4; j++)
        {
            if (i == row || j == col)
            {
                //skip
                continue;
            }

            product[++index] = mat.m[MAT4_I(row, col)];
        }
    }
    return product;
}
