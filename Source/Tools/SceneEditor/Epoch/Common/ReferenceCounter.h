// File: ReferenceCounter.h
// Desc:

#ifndef REFERENCECOUNTER_H
#define REFERENCECOUNTER_H

#include"DLLAPI.h"

namespace Common
{

class DLLAPI ReferenceCounter
{
private:
	unsigned int* m_Count;

protected:
	void DeleteCount();

public:
	ReferenceCounter(unsigned int InitNum = 1): m_Count(new unsigned int(InitNum)) {}
	ReferenceCounter(const ReferenceCounter& rc): m_Count(rc.m_Count) { AddCount(); }
	ReferenceCounter& operator = (const ReferenceCounter& rc);
	virtual ~ReferenceCounter() { SubCount(); }
	
	void AddCount() { ++(*m_Count); }
	void SubCount();
	unsigned int GetCount() const { return *m_Count; }
};

}  // namespace

#endif