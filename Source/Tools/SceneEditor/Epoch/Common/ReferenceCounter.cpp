// File: ReferenceCounter.cpp
// Desc:

#include"ReferenceCounter.h"

namespace Common
{

void ReferenceCounter::DeleteCount()
{
	if(m_Count)
	{
		delete m_Count;
		m_Count = 0;
	}
}

ReferenceCounter& ReferenceCounter::operator = (const ReferenceCounter& rc)
{
	if(&rc != this)
	{
		SubCount();
		m_Count = rc.m_Count;
		AddCount();
	}

	return *this;
}

void ReferenceCounter::SubCount()
{
	if(GetCount() == 0)
	{
		DeleteCount();
	}
	else
	{
		--(*m_Count);
		if(GetCount() == 0)
		{
			DeleteCount();
		}
	}
}

}  // namespace