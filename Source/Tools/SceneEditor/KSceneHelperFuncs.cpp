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
#include "IEEditor.h"
#include "IEKG3DSceneSFXEditor.h"
#include "KSceneHelperFuncs.h"
#include "IEKG3DCollocator.h"

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
								   , LPCTSTR pMeshName, LPCTSTR pMtlName, LPCTSTR pAniName,DWORD dwReliveID,BOOL bUseDefaultObstacle,BOOL bGenerateObstacle )
{
	_ASSERTE(pManager);

	IEKG3DRepresentDoodad_BuildStruct BuildStruct;
	BuildStruct.dwDoodadTempleteID = dwDoodadTempleteID;
	BuildStruct.pDoodadName = pDoodadName;
	BuildStruct.pMeshName = pMeshName;
	BuildStruct.pMtlName = pMtlName;
	BuildStruct.pAniName = pAniName;
	BuildStruct.dwReliveID = dwReliveID;
	BuildStruct.bUseDefaultObstacle = bUseDefaultObstacle;
	BuildStruct.bGenerateObstacle = bGenerateObstacle;

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

namespace KG3D_SCENE_FUNCTIONS
{
	IEKG3DSceneEntity* GetUniqueSelectedEntity(IEKG3DSceneSFXEditor* pScene)
	{
		_ASSERTE(NULL != pScene);
		IEKG3DSceneSelectionTool* pSelTool = NULL;
		pScene->GetSelectionToolInterface(&pSelTool);
		if(NULL == pSelTool)
			return NULL;

		if(1 != pSelTool->GetSelectionCount())
			return NULL;

		IEKG3DSceneSelectionToolAccessProxy* pProxy = NULL;
		pSelTool->GetAccessProxy(&pProxy);
		if(NULL == pProxy)
			return NULL;

		return pProxy->Reset();
	}

	IEKG3DSceneEntity* GetUniqueSelectedEntity( IEKG3DSceneEditorBase* pScene )
	{
		_ASSERTE(NULL != pScene);
		IEKG3DSceneSelectionTool* pSelTool = NULL;
		pScene->GetSelectionToolInterface(&pSelTool);
		if(NULL == pSelTool)
			return NULL;

		if(1 != pSelTool->GetSelectionCount())
			return NULL;

		IEKG3DSceneSelectionToolAccessProxy* pProxy = NULL;
		pSelTool->GetAccessProxy(&pProxy);
		if(NULL == pProxy)
			return NULL;

		return pProxy->Reset();
	}
	IEKG3DSceneSelectionToolAccessProxy* GetSelectionProxy( IEKG3DSceneSFXEditor* pScene )
	{
		_ASSERTE(NULL != pScene);
		IEKG3DSceneSelectionTool* pSelTool = NULL;
		pScene->GetSelectionToolInterface(&pSelTool);
		if(NULL == pSelTool)
			return NULL;

		IEKG3DSceneSelectionToolAccessProxy* pProxy = NULL;
		pSelTool->GetAccessProxy(&pProxy);
		return pProxy;
	}

	IEKG3DSceneSelectionToolAccessProxy* GetSelectionProxy( IEKG3DSceneEditorBase* pScene )
	{
		_ASSERTE(NULL != pScene);
		IEKG3DSceneSelectionTool* pSelTool = NULL;
		pScene->GetSelectionToolInterface(&pSelTool);
		if(NULL == pSelTool)
			return NULL;

		IEKG3DSceneSelectionToolAccessProxy* pProxy = NULL;
		pSelTool->GetAccessProxy(&pProxy);
		return pProxy;
	}

	IEKG3DSceneEntity* GetFirstSelectedEntity( IEKG3DSceneSFXEditor* pScene )
	{
		_ASSERTE(NULL != pScene);
		IEKG3DSceneSelectionTool* pSelTool = NULL;
		pScene->GetSelectionToolInterface(&pSelTool);
		if(NULL == pSelTool)
			return NULL;

		IEKG3DSceneSelectionToolAccessProxy* pProxy = NULL;
		pSelTool->GetAccessProxy(&pProxy);
		if(NULL == pProxy)
			return NULL;

		return pProxy->Reset();
	}

	IEKG3DSceneEntity* GetFirstSelectedEntity( IEKG3DSceneSceneEditor* pScene )
	{
		_ASSERTE(NULL != pScene);
		IEKG3DSceneSelectionTool* pSelTool = NULL;
		pScene->GetSelectionToolInterface(&pSelTool);
		if(NULL == pSelTool)
			return NULL;

		IEKG3DSceneSelectionToolAccessProxy* pProxy = NULL;
		pSelTool->GetAccessProxy(&pProxy);
		if(NULL == pProxy)
			return NULL;

		return pProxy->Reset();
	}
	IEKG3DSceneEntity* GetFirstSelectedEntity( IEKG3DSceneModelEditor* pScene )
	{
		_ASSERTE(NULL != pScene);
		IEKG3DSceneSelectionTool* pSelTool = NULL;
		pScene->GetSelectionToolInterface(&pSelTool);
		if(NULL == pSelTool)
			return NULL;

		IEKG3DSceneSelectionToolAccessProxy* pProxy = NULL;
		pSelTool->GetAccessProxy(&pProxy);
		if(NULL == pProxy)
			return NULL;

		return pProxy->Reset();
	}
	ULONG GetSelectionCount( IEKG3DSceneSceneEditor* pScene )
	{
		_ASSERTE(NULL != pScene);
		_ASSERTE(NULL != pScene);
		IEKG3DSceneSelectionTool* pSelTool = NULL;
		pScene->GetSelectionToolInterface(&pSelTool);
		if(NULL == pSelTool)
			return 0;

		return pSelTool->GetSelectionCount();
	}

	VOID SceneFileSaveConfirmer::RecordSaveFiles( DWORD dwSaveOp, LPCTSTR strFileFullPath )
	{
		IEKG3DCollocator* pCollocator= KSH::GetEngineInterfaceEx<IEKG3DCollocator>();
		if(NULL == pCollocator)
			return;

		BOOL bIsMapFileExist = g_IsFileExist(strFileFullPath);

		m_vecFileWriteTimes.reserve(10);
		TypeSaveOptionDescArrayRef saveOpDesc = g_GetSaveOptionDesc();
		for (size_t i = 0; i < _countof(saveOpDesc); ++i)
		{
			if (! (dwSaveOp & saveOpDesc[i].dwOp))
				continue;

			if(saveOpDesc[i].dwOp == EM_SAVE_OP_REGION_INFO)	//暂时不管这个
				continue;
			
			TCHAR strRetPath[MAX_PATH * 2] = _T("");
			HRESULT hr = pCollocator->GetSavePath(saveOpDesc[i].dwOp, strFileFullPath, NULL, strRetPath, _countof(strRetPath));
			_ASSERTE(SUCCEEDED(hr));
			HANDLE hFile = ::CreateFile(strRetPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if(INVALID_HANDLE_VALUE == hFile)
			{
				TypeFileWrite fw = 
				{
					std::tstring(strRetPath),
					{0,0},	//文件不存在，记录空值
					FALSE,
				};
				
				m_vecFileWriteTimes.push_back(fw);
			}
			else
			{
				TypeFileWrite fw = 
				{
					std::tstring(strRetPath),
					{0,0},	
					FALSE,
				};
				::GetFileTime(hFile, NULL, NULL, &fw.ftLastWriteTime);
				m_vecFileWriteTimes.push_back(fw);

				::CloseHandle(hFile);
			}
		}
	}

	VOID SceneFileSaveConfirmer::ConfirmFileWrite()
	{
		std::vector<size_t> vecFailedIndices;
		vecFailedIndices.reserve(m_vecFileWriteTimes.size());
		
		BOOL bAllSucceeded = TRUE;
		for (size_t i = 0; i < m_vecFileWriteTimes.size(); ++i)
		{
			TypeFileWrite& oldTime = m_vecFileWriteTimes[i];
			HANDLE hFile = ::CreateFile(oldTime.strFullPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if(INVALID_HANDLE_VALUE != hFile)
			{
				FILETIME ftCurWriteTime = {0,0};
				::GetFileTime(hFile, NULL, NULL, &ftCurWriteTime);
				if (0 != ::CompareFileTime(&ftCurWriteTime, &oldTime.ftLastWriteTime))
				{
					oldTime.bWriteSucceeded = TRUE;
				}
				else
				{
					vecFailedIndices.push_back(i);
				}
				::CloseHandle(hFile);
			}
			else
			{
				vecFailedIndices.push_back(i);
			}
		}

		if (! vecFailedIndices.empty())
		{
			std::tstring strError = _T("以下文件写操作不成功:\r\n");
			strError.reserve(MAX_PATH * 2 * vecFailedIndices.size());
			for (size_t i = 0; i < vecFailedIndices.size(); ++i)
			{
				size_t uIndex = vecFailedIndices[i];
				strError += m_vecFileWriteTimes[uIndex].strFullPath;
				strError += _T("\r\n");
			}
			MessageBox(NULL, strError.c_str(), _T("3D场景部分保存有问题，试试重新保存是否能解决问题"), MB_OK);
		}
	}
};