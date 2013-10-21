// File: Polygon.h
// Desc:

#ifndef POLYGON_H
#define POLYGON_H

#include"..\Common\DLLAPI.h"

namespace Maths
{

template <class Vector>
class DLLAPI Polygon
{
public:
	int m_iMaxNumVectors;  // 最多有多少个Vector
	int m_iCurrentNumVectors;  // 当前有多少个Vector
	Vector* m_vList;  // 存储Vector的链表

public:
	Polygon(): m_iMaxNumVectors(0), m_iCurrentNumVectors(0), m_vList(0) {}
	Polygon(int iMaxNumVectors): m_iMaxNumVectors(iMaxNumVectors), m_iCurrentNumVectors(0), m_vList(new Vector[iMaxNumVectors]) {}
	Polygon(const Polygon<Vector>& mp);
	Polygon& operator = (const Polygon<Vector>& mp);
	~Polygon();

private:
	void Clone(const Polygon<Vector>& mp);
	void Destroy();
};

// 成员函数：

template <class Vector>
Polygon<Vector>::Polygon(const Polygon<Vector>& mp)
{
	m_vList = 0;
	Clone(mp);
}

template <class Vector>
Polygon<Vector>& Polygon<Vector>::operator = (const Polygon<Vector>& mp)
{
	if(&mp != this)
	{
		Destroy();
		Clone(mp);
	}
	
	return *this;
}

template <class Vector>
Polygon<Vector>::~Polygon()
{
	Destroy();
}

template <class Vector>
void Polygon<Vector>::Clone(const Polygon<Vector>& mp)
{
	if(!mp.m_vList)
	{
		return;
	}

	if(m_vList)
	{
		Destroy();
	}

	m_iMaxNumVectors = mp.m_iMaxNumVectors
	m_iCurrentNumVectors = mp.m_iCurrentNumVectors;
	m_vList = new Vector[m_iMaxNumVectors];
	for(int i = 0; i < m_iCurrentNumVectors; ++i)
	{
		m_vList[i] = mp.m_vList[i];
	}

}

template <class Vector>
void Polygon<Vector>::Destroy()
{
	if(m_vList)
	{
		delete [] m_vList;
		m_vList = 0;
	}

	m_iMaxNumVectors = m_iCurrentNumVectors = 0;
}

}

#endif