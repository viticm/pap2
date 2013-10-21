// File: Matrix.h
// Desc: 4 by 4 matrix

#ifndef MATRIX_H
#define MATRIX_H

#include"Vector3F.h"
#include"LineSeg.h"

namespace Maths
{

class DLLAPI Matrix
{
public:
	union
	{
		struct
		{
			float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
		};
		float m[4][4];
	};

public:
	static const Matrix Identity;

public:
	Matrix();
	Matrix(
		float in11, float in12, float in13, float in14,
        float in21, float in22, float in23, float in24,
        float in31, float in32, float in33, float in34,
        float in41, float in42, float in43, float in44
		);
	Matrix(const Matrix& mm);
	Matrix& operator = (const Matrix& mm);
	~Matrix() {}

	Matrix(const D3DMATRIX& dmtx);
	Matrix(const D3DXMATRIX& dxmtx);

	Matrix& operator = (const D3DMATRIX& dmtx);
	Matrix& operator = (const D3DXMATRIX& dxmtx);

	operator D3DMATRIX ();
	operator D3DXMATRIX ();

	operator D3DMATRIX* ();
	operator D3DXMATRIX* ();
	operator D3DMATRIX& ();
	operator D3DXMATRIX& ();

	void ToD3DMATRIX(D3DMATRIX& dmtx) const;
	void ToD3DXMATRIX(D3DXMATRIX& dxmtx) const;

	float* operator[] (int iRow) const { return (float*)m[iRow]; }

	Matrix& operator *= (const Matrix& mtx);

	// 旋转矩阵：

	void XRotate(float theta);
	void YRotate(float theta);
	void ZRotate(float theta);
	void AxisRotate(const Vector3F& mvAxis, float angle);
	void PointBasedAxisRotate(const Vector3F& point, const Vector3F& axis, float angle);  // 以过Point的axis为轴旋转角度angle

	void Translate(const Vector3F& mv);  // 平移矩阵。例：
	                                          // v1 * T(v2) == v1 + v2
	void Scale(const Vector3F& mv);  // 缩放矩阵
	void Inverse(const Matrix& mm);
	void Identilize();

	Vector3F GetLocation() const;  // This function is mainly used on Translation matrix
	Vector3F GetAxis(int iAxis) const;  // This function is mainly used on View matrix

	// position: the position of object in world space; orientation: 3 angles around the corresponding axises in world space
	void CreateWorldMatrix(const Vector3F& position, const Vector3F& orientation);

	// position: the position of camera in world space; orientation: 3 angles around the corresponding axises in world space
	void CreateViewMatrix(const Vector3F& position, const Vector3F& orientation);
	void CreateViewMatrix(const Vector3F& position, const Vector3F& camXAxis, const Vector3F& camYAxis, const Vector3F& camZAxis);
	void CreateViewMatrix(const Vector3F& position, const Vector3F& lookat, const Vector3F& up);

	// h: 水平; v: 垂直
	void CreateProjectionMatrixByFOV(const float near_plane, const float far_plane, const float fov_h, const float fov_v);
	void CreateProjectionMatrixByViewportDimensions(const float near_plane, const float far_plane, const float width, const float height);

	void CloneData(const Matrix& mm);

//private:
	
};

// 全局函数：

inline bool operator == (const Matrix& a, const Matrix& b)
{
	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
		{
			if(Maths::FNotEqual(a.m[i][j], b.m[i][j]))
			{
				return false;
			}
		}
	}

	return true;
}

inline bool operator != (const Matrix& a, const Matrix& b)
{
	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
		{
			if(Maths::FEqual(a.m[i][j], b.m[i][j]))
			{
				return false;
			}
		}
	}

	return true;
}

DLLAPI void Multiply(Matrix& mResult, const Matrix& a, const Matrix& b);
DLLAPI const Matrix operator * (const Matrix& a, const Matrix& b);

inline const Vector3F operator * (const Vector3F& mv, const Matrix& mm)
{
	return Vector3F(
		mv.x * mm._11 + mv.y * mm._21 + mv.z * mm._31 + mm._41,
		mv.x * mm._12 + mv.y * mm._22 + mv.z * mm._32 + mm._42,
		mv.x * mm._13 + mv.y * mm._23 + mv.z * mm._33 + mm._43
		);
}

inline const Vector3F operator * (const Matrix& mm, const Vector3F& mv)
{
	return mv * mm;
}

inline const LineSeg operator * (const LineSeg& mls, const Matrix& mm)
{
	return LineSeg(mls.v0 * mm, mls.v1 * mm);
}

inline const LineSeg operator * (const Matrix& mm, const LineSeg& mls)
{
	return mls * mm;
}

}  // namespace

#endif