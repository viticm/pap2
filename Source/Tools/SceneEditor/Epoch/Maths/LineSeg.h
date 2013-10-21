// File: LineSeg.h
// Desc:

#ifndef LINESEG_H
#define LINESEG_H

#include"Vector3F.h"

namespace Maths
{

class DLLAPI LineSeg
{
public:
	Vector3F v0, v1;

public:
	LineSeg(): v0(Vector3F::Zero()), v1(Vector3F::Zero()) {}
	LineSeg(const Vector3F& a, const Vector3F& b): v0(a), v1(b) {}
	LineSeg(const LineSeg& mls): v0(mls.v0), v1(mls.v1) {}
	LineSeg& operator = (const LineSeg& mls);
	~LineSeg() {}
};

inline LineSeg& LineSeg::operator = (const LineSeg& mls)
{
	if(&mls != this)
	{
		v0 = mls.v0; v1 = mls.v1;
	}

	return *this;
}

}

#endif