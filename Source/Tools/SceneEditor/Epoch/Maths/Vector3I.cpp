// File: Vector3I.cpp
// Desc:

#include"Vector3I.h"

namespace Maths
{

static const Vector3I V3I_Zero(0, 0, 0);
static const Vector3I V3I_i(1, 0, 0);
static const Vector3I V3I_j(0, 1, 0);
static const Vector3I V3I_k(0, 0, 1);

Vector3I::operator D3DVECTOR ()
{
	D3DVECTOR ret;
	ret.x = static_cast<float>(x);
	ret.y = static_cast<float>(y);
	ret.y = static_cast<float>(y);

	return ret;
}

const Vector3I Vector3I::Zero()
{
	return V3I_Zero;
}

const Vector3I Vector3I::i()
{
	return V3I_i;
}

const Vector3I Vector3I::j()
{
	return V3I_j;
}

const Vector3I Vector3I::k()
{
	return V3I_k;
}

}  // namespace