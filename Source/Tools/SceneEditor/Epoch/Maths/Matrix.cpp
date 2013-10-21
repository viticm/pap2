// File: Matrix.cpp
// Desc:

#include"Matrix.h"
#include<assert.h>

namespace Maths
{

void Multiply(Matrix& mResult, const Matrix& a, const Matrix& b)
{
	mResult = a * b;
}

const Matrix operator * (const Matrix& a, const Matrix& b)
{
	Matrix out;

    for(int j = 0; j < 4; j++)         // transform by columns first
	{
        for (int i = 0; i < 4; i++)     // then by rows
		{
            out.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
		}
	}

	return out;
}

Matrix::Matrix()
{
	Identilize();
}

Matrix::Matrix(
		float in11, float in12, float in13, float in14,
        float in21, float in22, float in23, float in24,
        float in31, float in32, float in33, float in34,
        float in41, float in42, float in43, float in44
		)
{
	_11 = in11; _12 = in12; _13 = in13; _14 = in14;
	_21 = in21; _22 = in22; _23 = in23; _24 = in24;
	_31 = in31; _32 = in32; _33 = in33; _34 = in34;
	_41 = in41; _42 = in42; _43 = in43; _44 = in44;
}

Matrix::Matrix(const Matrix& mm)
{
	CloneData(mm);
}

Matrix& Matrix::operator = (const Matrix& mm)
{
	if(&mm != this)
	{
		CloneData(mm);
	}

	return *this;
}

Matrix::Matrix(const D3DMATRIX& dmtx)
{
	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
		{
			m[i][j] = dmtx.m[i][j];
		}
	}
}

Matrix::Matrix(const D3DXMATRIX& dxmtx)
{
	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
		{
			m[i][j] = dxmtx.m[i][j];
		}
	}
}

Matrix& Matrix::operator = (const D3DMATRIX& dmtx)
{
	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
		{
			m[i][j] = dmtx.m[i][j];
		}
	}
	
	return *this;
}

Matrix& Matrix::operator = (const D3DXMATRIX& dxmtx)
{
	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
		{
			m[i][j] = dxmtx.m[i][j];
		}
	}
	
	return *this;
}

Matrix::operator D3DMATRIX ()
{
	D3DMATRIX ret;

	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
		{
			ret.m[i][j] = m[i][j];
		}
	}

	return ret;
}

Matrix::operator D3DXMATRIX ()
{
	D3DXMATRIX ret;

	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
		{
			ret.m[i][j] = m[i][j];
		}
	}

	return ret;
}

Matrix::operator D3DMATRIX* ()
{
	return (D3DMATRIX*)this;
}

Matrix::operator D3DXMATRIX* ()
{
	return (D3DXMATRIX*)this;
}

Matrix::operator D3DMATRIX& ()
{
	return *this;
}

Matrix::operator D3DXMATRIX& ()
{
	return *this;
}

void Matrix::ToD3DMATRIX(D3DMATRIX& dmtx) const
{
	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
		{
			dmtx.m[i][j] = m[i][j];
		}
	}
}

void Matrix::ToD3DXMATRIX(D3DXMATRIX& dxmtx) const
{
	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
		{
			dxmtx.m[i][j] = m[i][j];
		}
	}
}

Matrix& Matrix::operator *= (const Matrix& mtx)
{
	if(&mtx != this)
	{
		*this = *this * mtx;
	}

	return *this;
}

void Matrix::XRotate(float theta)
{
	float c = cosf(theta);
	float s = sinf(theta);

	Identilize();

	_22 = c;
	_23 = s;
	_32 = -s;
	_33 = c;
}

void Matrix::YRotate(float theta)
{
	float c = cosf(theta);
	float s = sinf(theta);

	Identilize();

	_11 = c;
	_13 = -s;
	_31 = s;
	_33 = c;
}

void Matrix::ZRotate(float theta)
{
	float c = cosf(theta);
	float s = sinf(theta);

	Identilize();

	_11 = c;
	_12 = s;
	_21 = -s;
	_22 = c;
}

void Matrix::AxisRotate(const Vector3F& mvAxis, float angle)
{
	Vector3F mvNormalizedAxis = mvAxis;
	mvNormalizedAxis.Normalize();

	float c = cosf(angle);
	float s = sinf(angle);
	float x = mvNormalizedAxis.x, y = mvNormalizedAxis.y, z = mvNormalizedAxis.z;

	_11 = x * x * (1 - c) + c;
	_21 = x * y * (1 - c) - (z * s);
	_31 = x * z * (1 - c) + (y * s);
	_41 = 0;

	_12 = y * x * (1 - c) + (z * s);
	_22 = y * y * (1 - c) + c;
	_32 = y * z * (1 - c) - (x * s);
	_42 = 0;

	_13 = z * x * (1 - c) - (y * s);
	_23 = z * y * (1 - c) + (x * s);
	_33 = z * z * (1 - c) + c;
	_43 = 0;

	_14 = 0;
	_24 = 0;
	_34 = 0;
	_44 = 1;
}

void Matrix::PointBasedAxisRotate(const Vector3F& point, const Vector3F& axis, float angle)
{
	Matrix mtxTranslate, mtxRotate;

	mtxTranslate.Translate(-point);
	mtxRotate.AxisRotate(axis, angle);
	*this  = mtxTranslate * mtxRotate;
	mtxTranslate.Translate(point);
	*this = *this * mtxTranslate;
}

void Matrix::Translate(const Vector3F& mv)
{
	Identilize();

	_41 = mv.x;
	_42 = mv.y;
	_43 = mv.z;
}

void Matrix::Scale(const Vector3F& mv)
{
	Identilize();

	_11 = mv.x;
	_22 = mv.y;
	_33 = mv.z;
}

void Matrix::Inverse(const Matrix& mm)
{
	_11 = mm._11;
	_12 = mm._21;
	_13 = mm._31;
	_21 = mm._12;
	_22 = mm._22;
	_23 = mm._32;
	_31 = mm._13;
	_32 = mm._23;
	_33 = mm._33;

	_14 = 0;
	_24 = 0;
	_34 = 0;
	_44 = 1;

	Vector3F temp = mm.GetLocation();

	_41 = -(temp.x * mm._11 + temp.y * mm._12 + temp.z * mm._13);
	_42 = -(temp.y * mm._21 + temp.y * mm._22 + temp.z * mm._23);
	_43 = -(temp.z * mm._31 + temp.z * mm._32 + temp.z * mm._33);
}

void Matrix::Identilize()
{
	_11 = 1.f;	_22 = 1.f;	_33 = 1.f;	_44 = 1.f;
	_12 = 0.f;  _13 = 0.f;  _14 = 0.f;	_21 = 0.f;
	_23 = 0.f;	_24 = 0.f;	_31 = 0.f;	_32 = 0.f;
	_34 = 0.f;	_41 = 0.f;	_42 = 0.f;	_43 = 0.f;
}

Vector3F Matrix::GetLocation() const
{
	return Vector3F(_41, _42, _43);
}

Vector3F Matrix::GetAxis(int iAxis) const
{
	return Vector3F(m[0][iAxis], m[1][iAxis], m[2][iAxis]);  // BE WARE: this algorithm is only compatible with D3D
}

void Matrix::CreateWorldMatrix(const Vector3F& position, const Vector3F& orientation)
{
	Identilize();

	Matrix T, Rx, Ry, Rz;

	T.Translate(position);
	Rx.XRotate(orientation.x);
	Ry.YRotate(orientation.y);
	Rz.ZRotate(orientation.z);

	*this = Rx * Ry * Rz * T;
}

void Matrix::CreateViewMatrix(const Vector3F& position, const Vector3F& orientation)
{
	Identilize();

	Matrix T, Rx, Ry, Rz;

	T.Translate(-position);
	Rx.XRotate(-orientation.x);
	Ry.YRotate(-orientation.y);
	Rz.ZRotate(-orientation.z);

	*this = T * Rx * Ry * Rz;
}

void Matrix::CreateViewMatrix(const Vector3F& position, const Vector3F& camXAxis, const Vector3F& camYAxis, const Vector3F& camZAxis)
{
	_11 = camXAxis.x;             _12 = camYAxis.x;             _13 = camZAxis.x;             _14 = 0.f;
	_21 = camXAxis.y;             _22 = camYAxis.y;             _23 = camZAxis.y;             _24 = 0.f;
	_31 = camXAxis.z;             _32 = camYAxis.z;             _33 = camZAxis.z;             _34 = 0.f;
	_41 = -(camXAxis * position); _42 = -(camYAxis * position); _43 = -(camZAxis * position); _44 = 1.f;
}

void Matrix::CreateViewMatrix(const Vector3F& position, const Vector3F& lookat, const Vector3F& up)
{
	Vector3F camXAxis, camYAxis, camZAxis;

	camZAxis = lookat - position;
	camZAxis.Normalize();

	camXAxis = up ^ camZAxis;
	camXAxis.Normalize();

	camYAxis = camZAxis ^ camXAxis;
	camYAxis.Normalize();

	CreateViewMatrix(position, camXAxis, camYAxis, camZAxis);
}

void Matrix::CreateProjectionMatrixByFOV(const float near_plane, const float far_plane, const float fov_h, const float fov_v)
{
	float h, w, Q;

    w = 1.f / tanf(fov_h * 0.5f);  // 1/tan(x) == cot(x)
    h = 1.f / tanf(fov_v * 0.5f);   // 1/tan(x) == cot(x)
    Q = far_plane / (far_plane - near_plane);

    Identilize();
	_44 = 0.f;

    _11 = w;
    _22 = h;
    _33 = Q;
    _43 = -Q * near_plane;
    _34 = 1;
}

void Matrix::CreateProjectionMatrixByViewportDimensions(const float near_plane, const float far_plane, const float width, const float height)
{
	float h, w, Q;

    w = 2 * near_plane / width;  // 1/tan(x) == cot(x)
    h = 2 * near_plane / height;   // 1/tan(x) == cot(x)
    Q = far_plane / (far_plane - near_plane);

    Identilize();
	_44 = 0.f;

    _11 = w;
    _22 = h;
    _33 = Q;
    _43 = -Q * near_plane;
    _34 = 1;
}

void Matrix::CloneData(const Matrix& mm)
{
	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
		{
			m[i][j] = mm.m[i][j];
		}
	}
}


}  // namespace