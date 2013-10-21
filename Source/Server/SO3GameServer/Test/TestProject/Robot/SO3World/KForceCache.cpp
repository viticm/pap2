#include "stdafx.h"
#include "KForceCache.h"

KForceCache::KForceCache(void)
{
	memset(m_ForceRelation, 0, sizeof(m_ForceRelation));
}

KForceCache::~KForceCache(void)
{
}

BOOL KForceCache::Load(char* pData)
{

	return TRUE;
}