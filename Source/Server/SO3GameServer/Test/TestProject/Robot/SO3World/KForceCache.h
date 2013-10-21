#ifndef _KFORCE_CACHE_H_
#define _KFORCE_CACHE_H_

#include "SO3GlobalDef.h"

class KForceCache
{
public:
	KForceCache(void);
	~KForceCache(void);

	BOOL Load(char* pData);

	KForceRelation*	GetRelation(DWORD dwForceID1, DWORD dwForceID2);

private:
	KForceRelation	m_ForceRelation[MAX_FORCE_COUNT][MAX_FORCE_COUNT];
};

#endif	//_KFORCE_CACHE_H_