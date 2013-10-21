#include "stdafx.h"
#include "KSceneObject.h"
#include "KSO3World.h"
#include "KMath.h"
#include "KRegion.h"

KSceneObject::KSceneObject()
{
	m_RegionObjNode.m_pSceneObject = this;

	m_pCell = NULL;
	m_pRegion = NULL;
	m_pScene = NULL;
}

KSceneObject::~KSceneObject()
{
}

BOOL KSceneObject::Init()
{
	m_nX                = 0;
    m_nY                = 0;
    m_nZ                = 0;
    m_nTouchRange       = 0;
	m_nXCell            = 0;
    m_nYCell            = 0;
	m_nMass             = DEFAULT_MASS;
	m_nWidth            = 1;
    m_nLength           = 1;
    m_nHeight           = 1;
	m_nGameLoop         = 0;
	m_nFaceDirection    = 0;
    m_nPitchDirection   = 0;

#ifdef _CLIENT
    m_pRepresentObject = NULL;	// 表现逻辑对象
#endif


    m_pCell             = NULL;
    m_pRegion           = NULL;
    m_pScene            = NULL;
	
	return true;
}

void KSceneObject::UnInit()
{
}

BOOL KSceneObject::CheckGameLoop(void)
{
	if (m_nGameLoop < g_pSO3World->m_nGameLoop)
	{
		m_nGameLoop = g_pSO3World->m_nGameLoop;
		return true;
	}

	return false;
}


BOOL KSceneObject::GetAbsoluteCoordinate(int* pnX, int* pnY, int* pnZ)
{
	*pnX = m_nX;
	*pnY = m_nY;
	*pnZ = m_nZ;

    return true;
}

void KSceneObject::SetCell(KCell* pDstCell)
{
    m_pCell = pDstCell;
}
