////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneHelperFuncs.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-4-21 9:40:56
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KSceneHelperFuncs.h"
#include "IEOther.h"
#include "IEEditor.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
////////////////////////////////////////////////////////////////////////////////


IEKG3DRepresentObject* BuildTrafficPoint( IEKG3DEngineManager* pManager, DWORD dwSetID, DWORD dwType, LPCTSTR lpName )
{
	_ASSERTE(pManager);

	IEKG3DRepresentTrafficPoint_BuildStruct BuildStruct;

	BuildStruct.dwSetID = dwSetID;
	BuildStruct.dwType = dwType;
	BuildStruct.pTrafficSetName = lpName;

	IEKG3DRepresentObject* pRet = NULL;
	HRESULT hr = pManager->BuildKG3DRepresentObject(NULL, REPRESENTOBJECT_TRAFFICPOINT
		, reinterpret_cast<DWORD_PTR>(&BuildStruct), 0, &pRet);
	if (SUCCEEDED(hr))
	{
		return pRet;
	}
	return NULL;
}

IEKG3DRepresentObject* BuildWayPoint( IEKG3DEngineManager* pManager, DWORD dwSetID, LPCTSTR lpName )
{
	_ASSERTE(pManager);

	IEKG3DRepresentWayPoint_BuildStruct BuildStruct;

	BuildStruct.dwSetID = dwSetID;
	BuildStruct.tcsName = lpName;

	IEKG3DRepresentObject* pRet = NULL;
	HRESULT hr = pManager->BuildKG3DRepresentObject(NULL, REPRESENTOBJECT_WAYPOINT
		, reinterpret_cast<DWORD_PTR>(&BuildStruct), 0, &pRet);
	if (SUCCEEDED(hr))
	{
		return pRet;
	}
	return NULL;
}

IEKG3DRepresentObject* BuildRefreshPoint( IEKG3DEngineManager* pManager, DWORD dwType, DWORD dwSetID, LPCTSTR lpName )
{
	_ASSERTE(pManager);

	IEKG3DRepresentRefreshPoint_BuildStruct BuildStruct;

	BuildStruct.dwType = dwType;
	BuildStruct.dwID = dwSetID;
	BuildStruct.tcsName = lpName;

	IEKG3DRepresentObject* pRet = NULL;
	HRESULT hr = pManager->BuildKG3DRepresentObject(NULL, REPRESENTOBJECT_REFRESHPOINT
		, reinterpret_cast<DWORD_PTR>(&BuildStruct), 0, &pRet);
	if (SUCCEEDED(hr))
	{
		return pRet;
	}
	return NULL;
}

IEKG3DRepresentObject* BuildRepresentDoodad( IEKG3DEngineManager* pManager, DWORD dwDoodadTempleteID, LPCTSTR pDoodadName
								   , LPCTSTR pMeshName, LPCTSTR pMtlName, LPCTSTR pAniName,DWORD dwReliveID )
{
	_ASSERTE(pManager);

	IEKG3DRepresentDoodad_BuildStruct BuildStruct;
	BuildStruct.dwDoodadTempleteID = dwDoodadTempleteID;
	BuildStruct.pDoodadName = pDoodadName;
	BuildStruct.pMeshName = pMeshName;
	BuildStruct.pMtlName = pMtlName;
	BuildStruct.pAniName = pAniName;
	BuildStruct.dwReliveID = dwReliveID;

	IEKG3DRepresentObject* pRet = NULL;
	HRESULT hr = pManager->BuildKG3DRepresentObject(NULL, REPRESENTOBJECT_DOODAD
		, reinterpret_cast<DWORD_PTR>(&BuildStruct), 0, &pRet);
	if (SUCCEEDED(hr))
	{
		return pRet;
	}
	return NULL;
}

IEKG3DRepresentObject* BuildRepresentNPC(IEKG3DEngineManager* pManager, DWORD dwNPCTempleteID, LPCTSTR pNPCName
								, LPCTSTR pMdlName, LPCTSTR pAniName, LPCTSTR pFaceMesh, LPCTSTR pFaceMtl
								, int nLevel, int nType/*, int nAIType*/
								, float fScale, DWORD dwRepresentID, float fAlertRange, float fAttackRange
								, DWORD dwReliveID,DWORD dwRandomID,DWORD dwAISet
								)
{
	_ASSERTE(pManager);
	IEKG3DRepresentNPC_BuildStruct BuildStruct;
	BuildStruct.dwNPCTempleteID = dwNPCTempleteID;
	BuildStruct.strNPCName = pNPCName;
	BuildStruct.strMdlName = pMdlName;
	BuildStruct.strAniName = pAniName;
	BuildStruct.strFaceMesh = pFaceMesh;
	BuildStruct.strFaceMtl = pFaceMtl;
	BuildStruct.nLevel = nLevel;
	BuildStruct.nType = nType;
	//BuildStruct.nAIType = nAIType;
	BuildStruct.fScale = fScale;
	BuildStruct.dwRepresentID = dwRepresentID;
	BuildStruct.fAlertRange = fAlertRange;
	BuildStruct.fAttackRange = fAttackRange;
	BuildStruct.dwReliveID = dwReliveID;
	BuildStruct.dwRandomID = dwRandomID;
	BuildStruct.dwAISet = dwAISet;

	IEKG3DRepresentObject* pRet = NULL;
	HRESULT hr = pManager->BuildKG3DRepresentObject(NULL, REPRESENTOBJECT_NPC
		, reinterpret_cast<DWORD_PTR>(&BuildStruct), 0, &pRet);
	if (SUCCEEDED(hr))
	{
		return pRet;
	}
	return NULL;

}