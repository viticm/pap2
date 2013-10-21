// File: Vector3F.cpp
// Desc:

#include"Vector3F.h"

namespace Maths
{

static const Vector3F V3F_Zero(0.f, 0.f, 0.f);
static const Vector3F V3F_XAxis(1.f, 0.f, 0.f);
static const Vector3F V3F_YAxis(0.f, 1.f, 0.f);
static const Vector3F V3F_ZAxis(0.f, 0.f, 1.f);

Vector3F& Vector3F::operator = (const D3DVECTOR& dv)
{
	x = dv.x;
	y = dv.y;
	z = dv.z;

	return *this;
}

Vector3F& Vector3F::operator = (const D3DXVECTOR3& dxv3)
{
	x = dxv3.x;
	y = dxv3.y;
	z = dxv3.z;

	return *this;
}

Vector3F::operator D3DVECTOR ()
{
	D3DVECTOR ret;
	ret.x = x;
	ret.y = y;
	ret.z = z;

	return ret;
}

Vector3F::operator D3DVECTOR* ()
{
	return (D3DVECTOR*)this;
}

Vector3F::operator D3DXVECTOR3* ()
{
	return (D3DXVECTOR3*)this;
}

Vector3F::operator D3DVECTOR& ()
{
	return *this;
}

Vector3F::operator D3DXVECTOR3& ()
{
	return *this;
}

void Vector3F::ToD3DVECTOR(D3DVECTOR& dv) const
{
	dv.x = x;
	dv.y = y;
	dv.z = z;
}

void Vector3F::ToD3DXVECTOR3(D3DXVECTOR3& dv) const
{
	dv.x = x;
	dv.y = y;
	dv.z = z;
}

const Vector3F Vector3F::Zero()
{
	return V3F_Zero;
}

const Vector3F Vector3F::XAxis()
{
	return V3F_XAxis;
}

const Vector3F Vector3F::YAxis()
{
	return V3F_YAxis;
}

const Vector3F Vector3F::ZAxis()
{
	return V3F_ZAxis;
}

}  // namespace