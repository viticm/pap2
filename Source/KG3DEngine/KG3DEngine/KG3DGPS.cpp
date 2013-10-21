////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DGPS.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-19 11:41:54
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KG3DGPS.h"
#include "KG3DGraphicsTool.h"
#include "KG3DSceneBlock.h"
#include "KG3DScene.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DGPS_Dummy::KG3DGPS_Dummy()
{
	m_vecCamLookAtPos.x = m_vecCamLookAtPos.y = m_vecCamLookAtPos.z = 0;
}

HRESULT KG3DGPS_Dummy::Init( D3DXVECTOR3 vecPos )
{
	m_vecCamLookAtPos = vecPos;
	return S_OK;
}

HRESULT KG3DGPS_Dummy::AdjustPos( D3DXVECTOR3 vecPos )
{
	m_vecCamLookAtPos = vecPos;
	return S_OK;
}

HRESULT KG3DGPS_Dummy::GetPos( D3DXVECTOR3& RefPos )
{
	RefPos = m_vecCamLookAtPos;
	return S_OK;
}

HRESULT KG3DGPS_Dummy::GetRegionIndexByPos( INT& nXIndex, INT& nZIndex )
{
	KG3DScene* pScene = g_cGraphicsTool.GetCurScene();
	KG_PROCESS_ERROR(NULL != pScene);

    nXIndex = GetSceneRegionIndex(m_vecCamLookAtPos.x);
	nZIndex = GetSceneRegionIndex(m_vecCamLookAtPos.z);

    return S_OK;
Exit0:
    return E_FAIL;
}
////////////////////////////////////////////////////////////////////////////////


