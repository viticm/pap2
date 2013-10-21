#ifndef _TEST_DEF_H_
#define _TEST_DEF_H_
#include "Engine/KMutex.h"

struct MutextHelper
{
	MutextHelper(KMutex* pMutext)
	{
		ASSERT(pMutext);
		m_pMutex = pMutext;
		m_pMutex->Lock();
	}
	~MutextHelper()
	{
		m_pMutex->Unlock();
		m_pMutex = NULL;
	}
private:
	KMutex* m_pMutex;
};

#define SocpeLock(pMutex) MutextHelper mh(pMutex)
#endif