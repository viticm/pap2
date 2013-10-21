#include "stdafx.h"
#include "KSceneObject.h"
#include "KSO3World.h"
#include "KMath.h"
#include "KMonster.h"


KSceneObject::KSceneObject()
{
	m_RegionObjNode.m_pSceneObject = this;

	m_pCell = NULL;
	m_pRegion = NULL;
	m_pScene = NULL;

    m_pOwner = NULL;
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

#if defined(_EDITOR)
	m_nEditorZ = 0;
#endif	//_EDITOR

    m_pCell             = NULL;
    m_pRegion           = NULL;
    m_pScene            = NULL;
	
	return true;
}

BOOL KSceneObject::UnInit(void)
{
	return TRUE;
}

//返回值：
//TRUE表示该物体本游戏循环还未计算
//FALSE表示已经计算，不用再算了
//这种情况是由跨Region的引起的，当一个Character在一个Region中移动到另一个时，
//有可能在另一个Region中再次被Activate，所以在基类的Activate中判断了游戏循环数，
//以避免这种情况
BOOL KSceneObject::CheckGameLoop(void)
{
	if (m_nGameLoop < m_pOwner->m_nGameLoop)
	{
		m_nGameLoop = m_pOwner->m_nGameLoop;
		return TRUE;
	}


	return FALSE;
}

