// File: Triangle.h
// Desc:

#ifndef TRIANGLE_H
#define TRIANGLE_H

namespace Makeit
{

template <class Vector>
class Triangle
{
public:
	Vector v[3];

public:
	Triangle() {}
	Triangle(Vector v0, Vector v1, Vector v2);
	Triangle(const Triangle<Vector>& mt);
	Triangle<Vector>& operator = (const Triangle<Vector>& mt);
	~Triangle() {}
};

// ³ÉÔ±º¯Êý£º

template <class Vector>
Triangle<Vector>::Triangle(Vector v0, Vector v1, Vector v2)
{
	v[0] = v0; v[1] = v1; v[2] = v2;
}

template <class Vector>
Triangle<Vector>::Triangle(const Triangle<Vector>& mt)
{
	for(int i =0; i < 3; ++i)
	{
		v[i] = mt.v[i];
	}
}

template <class Vector>
Triangle<Vector>& Triangle<Vector>::operator = (const Triangle<Vector>& mt)
{
	if(&mt != this)
	{
		for(int i = 0; i < 3; ++i)
		{
			v[i] = mt.v[i];
		}
	}
	
	return *this;
}

}

#endif