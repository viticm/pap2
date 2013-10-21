#include "StdAfx.h"
#include "filetype.h"
#include "file.h"
#include "KSO3World.h"
#include "KScene.h"
#include "KPatrolPath.h"
#include "./3DEngine/KSceneEditorDocLogical.h"
#include "./3DEngine/SceneEditorCommon.h"
#include "./3DEngine/InputProxy.h"
#include "Functions.h"
#include "ProgressForm.h"


CDllHolder g_ModuleRepresent;
CDllHolder g_ModuleUI;
IKSO3RepresentInterface* g_pRepresent;
IKSO3RepresentHandler* g_pRepresentHandler = NULL;
ISO3UI* g_pUI;
//BOOL g_bUseSO3Logical = TRUE;
BOOL g_bSO3LogicInited = FALSE;
BOOL g_bKG3DEngineInited = FALSE;
extern TCHAR g_EnginePath[MAX_PATH];
//extern CProgressForm g_ProgressDlg;
extern CStatic g_ProgressName;
extern CProgressCtrl g_ProgressProgress;


BOOL InitUI()
{
	BOOL bRetCode = FALSE;
	fnCreateSO3UI pfnCreateSO3UI = NULL;

	KGLOG_PROCESS_ERROR(g_GetEngineIEInterface() && "init 3d engine first!");
	KGLOG_PROCESS_ERROR(g_pRepresent && "init represent first!");

	UnInitUI();

#ifdef _DEBUG
	g_ModuleUI.Init("SO3UID.dll");
#else
	g_ModuleUI.Init("SO3UI.dll");
#endif

	pfnCreateSO3UI = (fnCreateSO3UI)GetProcAddress(g_ModuleUI.GetDllHandle(), FN_CREATE_SO3UI);
	KGLOG_PROCESS_ERROR(pfnCreateSO3UI);

	g_pUI = pfnCreateSO3UI();
	KGLOG_PROCESS_ERROR(g_pUI);

#if 0
	Param.hInst = m_hInst;
	Param.hWnd = NULL;
	Param.fnAttribute = AttributeStringToID;
	Param.fnRequire = RequireStringToID;
	Param.p3DEngineManager = g_GetEngineIEInterface();
	Param.p3DModelManager = g_GetEngineIEInterface()->Get3DModelTable();
	Param.p3DUI = g_GetEngineIEInterface()->Get3DUI();
	Param.pRepresent = g_pRepresent->GetRepresentHandelr();
	g_pUI->Init(&Param);
#endif

	return TRUE;
Exit0:
	return FALSE;
}

void UnInitUI()
{
	if (g_pUI)
	{
		g_pUI->Exit();
		g_pUI = NULL;
	}
	g_ModuleUI.UnInit();
}

BOOL InitRepresent()
{
	BOOL bRetCode = FALSE;
	fnCreateSO3Represent pfnCreateSO3Represent = NULL;

	UnInitRepresent();

#ifdef _DEBUG
	g_ModuleRepresent.Init("SO3RepresentD.dll");
#else
	g_ModuleRepresent.Init("SO3Represent.dll");
#endif

	pfnCreateSO3Represent = (fnCreateSO3Represent)GetProcAddress(g_ModuleRepresent.GetDllHandle(), FN_CREATE_SO3REPRESENT);
	KGLOG_PROCESS_ERROR(pfnCreateSO3Represent);

	g_pRepresent = pfnCreateSO3Represent();
	KGLOG_PROCESS_ERROR(g_pRepresent);

	return TRUE;
Exit0:
	return FALSE;
}

void UnInitRepresent()
{
	SAFE_RELEASE(g_pRepresent);
	g_ModuleRepresent.UnInit();
}

void InitKeyState(IEKG3DSceneEditorBase* pBase, HWND hWnd, CInputProxy* pInputProxy)
{
	_ASSERTE(pInputProxy);

// 	static PROXYKEYMAP KEYMAP[] =
// 	{	//           C  S  A  L  M  R  ?    Trigger     ID  Prev Ig Exclusive
// 		{'X'	   , 0, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN , 16,  0, 0, 1},	//压感调整
// 		{'Z'	   , 0, 1, 0, 1, 0, 0, 0, WM_LBUTTONDOWN , 17,  0, 0, 1},	//内外圈同时调整
// 		{'Z'	   , 0, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN , 18,  0, 0, 1},	//外圈调整
// 
// 		{VK_MENU   , 0, 0, 1, 0, 1, 0, 0, WM_MOUSEMOVE,   1,  0, 0, 0},	//alt+MBT, 旋转摄像机镜头
// 		{VK_CONTROL, 1, 0, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   2,  0, 0, 0},	//ctrl+MBT, 快速缩放视角
// 		{VK_MBUTTON, 0, 0, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   3,  0, 0, 0},	//MBT, PAN视图
// 		{VK_MBUTTON, 0, 1, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   4,  0, 0, 0},	//SHIFT+MBT, 以选定点为中心旋转视图
// 		{VK_LBUTTON, 1, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, 5,  0, 0, 0},	//CTRL＋LBT，反向当前操作
// 
// 		{'W'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , 6,  0, 2, 0},	//前进 忽略鼠标键
// 		{VK_LBUTTON, 0, 0, 0, 1, 0, 1, 0, WM_MOUSEMOVE  , 6,  0, 0, 0},	//按住左右键不放，就直走
// 		{'S'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , 7,  0, 2, 0},	//后退 忽略鼠标键
// 		{'A'	   , 0, 0, 0, 1, 0, 0, 0, WM_KEYDOWN    , 8,  0, 1, 0},	//左转 
// 		{'D'	   , 0, 0, 0, 1, 0, 0, 0, WM_KEYDOWN    , 9,  0, 1, 0},	//右转 
// 		{'A'	   , 0, 0, 0, 0, 1, 0, 0, WM_KEYDOWN    , 8,  0, 1, 0},	//左转 
// 		{'D'	   , 0, 0, 0, 0, 1, 0, 0, WM_KEYDOWN    , 9,  0, 1, 0},	//右转 
// 		{'A'	   , 0, 0, 0, 0, 0, 1, 0, WM_KEYDOWN    ,10,  0, 1, 0},	//左移 忽略键盘
// 		{'D'	   , 0, 0, 0, 0, 0, 1, 0, WM_KEYDOWN    ,11,  0, 1, 0},	//右移 忽略键盘
// 
// 		{VK_RBUTTON, 0, 0, 0, 0, 0, 1, 0, WM_MOUSEMOVE , 12,  0, 0, 0},	//按住右键不放，就转动摄像机
// 		{' '	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , 13,  0, 3, 0},	//跳，忽略所有
// 		{'A'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , 14,  0, 1, 0},	//左转，同时转动摄像机
// 		{'D'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , 15,  0, 1, 0},	//右转
// 
// 		{VK_LBUTTON, 0, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, 19,  0, 0, 0},	//左键按下，可以作选择、改变地形等很多动作
// 		{VK_SHIFT,   0, 1, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, 20,  0, 0, 0},	//平滑
// 
// 		{VK_ADD,	 0, 0, 0, 0, 0, 0, 0, WM_LBUTTONDOWN, 21,  0, 3, 0},	//加速
// 		{VK_SUBTRACT,0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN,     22,  0, 3, 0},	//减速
// 
// 		{VK_LBUTTON, 1, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, 23,  0, 0, 0},	//在当前选择范围中，再增加选择内容
// 		{VK_LBUTTON, 0, 0, 1, 1, 0, 0, 0, WM_LBUTTONDOWN, 24,  0, 0, 0},	//在当前选择范围中，再减少选择内容
// 
// 		{0		   , 0, 0, 0, 0, 0, 0, 0, 0             , 0,  0, 0, 0},	//结束
// 		//           C  S  A  L  M  R  ?    Trigger     ID  Prev Ig Exclusive
// 	};

	static PROXYKEYMAP KEYMAP[] =
	{	//           C  S  A  L  M  R  ?  Trigger		  ID     Prev Ig   Exclusive
		{VK_RBUTTON, 0, 0, 0, 0, 0, 1, 0, WM_MOUSEMOVE,   1,     0,   0,   0},	// 鼠标右键, 旋转摄像机镜头
		//{VK_LBUTTON, 0, 0, 0, 1, 0, 0, 0, WM_MOUSEMOVE,   1001,  0,   0,   0},	// 鼠标左键, 快速缩放视角
		{VK_MBUTTON, 0, 0, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   3,     0,   0,   0},	// 鼠标中键, PAN 视图
		{0		   , 0, 0, 0, 0, 0, 0, 0, 0             , 0,     0,   0},	    // 结束
	};

	pInputProxy->Init(pBase, CWnd::FromHandle(hWnd), KEYMAP);
}

int ShowProgress(LPCTSTR pName, float fPencent)
{
	g_ProgressName.SetWindowText(pName);
	g_ProgressProgress.SetPos((int)(fPencent * 100));
	return 0;
}

void BeforeChange(KSceneEditorDocLogical* m_lpLogicalData)
{
	//////////////////////////////////////////////////////////////////////////
	{
		DWORD dwCount = 0;
		list<KSceneEditorDocLogical::WayPointSet*>::iterator i = m_lpLogicalData->m_listWayPointSet.begin();
		while (i!=m_lpLogicalData->m_listWayPointSet.end())
		{
			KSceneEditorDocLogical::WayPointSet* pPath = *i;

			pPath->nIndex = dwCount+1;

			i++;
			dwCount++;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_lpLogicalData->m_listNPCInfo.begin();
		while (i!=m_lpLogicalData->m_listNPCInfo.end())
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = *i;
			if(pNpc->pWayPointSet)
			{
				pNpc->nPathIndex = pNpc->pWayPointSet->nIndex;
			}
			else
			{
				pNpc->nPathIndex = 0;
			}

			i++;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//{
	//	DWORD dwSetCount = 0;
	//	list<KSceneEditorDocLogical::NPCRefreshPointSet*>::iterator i = m_lpLogicalData->m_listNPCRefreshPointSet.begin();
	//	while (i!=m_lpLogicalData->m_listNPCRefreshPointSet.end())
	//	{
	//		KSceneEditorDocLogical::NPCRefreshPointSet* pSet = *i;


	//		list<KSceneEditorDocLogical::NPCInfo*>::iterator j = pSet->listNPCInfo.begin();
	//		while (j!=pSet->listNPCInfo.end())
	//		{
	//			KSceneEditorDocLogical::NPCInfo* pNpc = *j;

	//			pNpc->dwNpcReliveID = dwSetCount + 1;
	//			j++;
	//		}

	//		i++;
	//		dwSetCount++;
	//	}
	//}
	////////////////////////////////////////////////////////////////////////////
	//{
	//	DWORD dwSetCount = 0;
	//	list<KSceneEditorDocLogical::DoodadRefreshPointSet*>::iterator i = m_lpLogicalData->m_listDoodadRefreshPointSet.begin();
	//	while (i!=m_lpLogicalData->m_listDoodadRefreshPointSet.end())
	//	{
	//		KSceneEditorDocLogical::DoodadRefreshPointSet* pSet = *i;


	//		list<KSceneEditorDocLogical::DoodadInfo*>::iterator j = pSet->listDoodadInfo.begin();
	//		while (j!=pSet->listDoodadInfo.end())
	//		{
	//			KSceneEditorDocLogical::DoodadInfo* pDoodad = *j;

	//			pDoodad->dwDoodadReliveID = dwSetCount + 1;
	//			j++;
	//		}

	//		i++;
	//		dwSetCount++;
	//	}
	//}
	//////////////////////////////////////////////////////////////////////////
	{
		DWORD dwCount = 0;
		list<KSceneEditorDocLogical::AIGroup*>::iterator i = m_lpLogicalData->m_listAIGroup.begin();
		while (i!=m_lpLogicalData->m_listAIGroup.end())
		{
			KSceneEditorDocLogical::AIGroup* pAI = *i;

			for (size_t n = 0;n<pAI->vecNpcInfo.size();n++)
			{
				KSceneEditorDocLogical::NPCInfo* pNpcInfo = pAI->vecNpcInfo[n];
				pNpcInfo->dwNpcTeamID = dwCount + 1;
				pNpcInfo->dwNpcTeamIndex = (DWORD) n;
			}

			i++;
			dwCount++;
		}
	}
}
void ExchangeLogicalData(LPCTSTR pFileName, KScene* lpLogicalScene, KSceneEditorDocLogical* m_lpLogicalData)
{
	HRESULT hrRetCode = NULL;

	BeforeChange(m_lpLogicalData);

	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	wsprintf(path_buffer,"%s",pFileName);
	_splitpath( path_buffer, drive, dir, fname, ext );

	TCHAR Name[MAX_PATH];
	wsprintf(Name,"map\\%s\\",fname);

	char szFileName[MAX_PATH];
	g_ExtractFileName(szFileName, fname);

	//////////////////////////////////////////////////////////////////////////
	TCHAR szMapDir[MAX_PATH];
	TCHAR szDoodadReliveDir[MAX_PATH];
	TCHAR szNpcReliveDir[MAX_PATH];
	TCHAR szPatrolPathDir[MAX_PATH];
	TCHAR szNpcTeamDir[MAX_PATH];
	TCHAR szRoadDir[MAX_PATH];

	wsprintf(szMapDir,			"%s%s\\map\\%s\\",				g_EnginePath, SETTING_DIR, fname);
	wsprintf(szDoodadReliveDir,	"%s%s\\map\\%s\\DoodadRelive\\",g_EnginePath, SETTING_DIR, fname);
	wsprintf(szNpcReliveDir,	"%s%s\\map\\%s\\NpcRelive\\",	g_EnginePath, SETTING_DIR, fname);
	wsprintf(szPatrolPathDir,	"%s%s\\map\\%s\\PatrolPath\\",	g_EnginePath, SETTING_DIR, fname);
	wsprintf(szNpcTeamDir,		"%s%s\\map\\%s\\NpcTeam\\",		g_EnginePath, SETTING_DIR, fname);
	wsprintf(szRoadDir,			"%s%s\\Road\\",					g_EnginePath, SETTING_DIR);
	mkdir(szMapDir);
	mkdir(szDoodadReliveDir);
	mkdir(szNpcReliveDir);
	mkdir(szPatrolPathDir);
	mkdir(szNpcTeamDir);
	mkdir(szRoadDir);

	lpLogicalScene->DeleteAllObjects();

	ExchangeNpc(lpLogicalScene, m_lpLogicalData);

	ExchangeDoodad(lpLogicalScene, m_lpLogicalData);

	wsprintf(Name,"map\\%s\\NpcRelive\\",fname);
	ExchaneNpcRefreshPointSet(Name,fname, m_lpLogicalData);

	wsprintf(Name,"map\\%s\\DoodadRelive\\",fname);
	ExchaneDoodadRefreshPointSet(Name,fname, m_lpLogicalData);

 	wsprintf(Name,"map\\%s\\NpcTeam\\",fname);
 	ExchangeAIGroup(Name,fname, m_lpLogicalData);

	wsprintf(Name,"map\\%s\\PatrolPath\\",fname);
	ExchangeWayPointSet(Name,fname, m_lpLogicalData);

	ExchangeTrafficPointSet(szRoadDir, m_lpLogicalData);
	SetCurrentDirectory(g_EnginePath);
	lpLogicalScene->Save(szFileName, &ShowProgress);

Exit0:
	;
}

void ExchangeNpc(KScene *pLogicalScene, KSceneEditorDocLogical* m_lpLogicalData)
{
	BOOL bRetCode = FALSE;
	INpcFile* piNpcFile = NULL;
	KNPC_DATA npcData;

	KGLOG_PROCESS_ERROR(pLogicalScene);

	piNpcFile = ::CreateNpcFileInterface();
	KGLOG_PROCESS_ERROR(piNpcFile);

	{
		int nTotalCount = (int)m_lpLogicalData->m_listNPCInfo.size();
		int nCount = 0;

		list<KSceneEditorDocLogical::NPCInfo*>::iterator it = m_lpLogicalData->m_listNPCInfo.begin();
		while (it != m_lpLogicalData->m_listNPCInfo.end())
		{
			KSceneEditorDocLogical::NPCInfo* pNpcInfo = *it;

			m_lpLogicalData->UpdateNPC(pNpcInfo);

			memset(&npcData, 0, sizeof(npcData));

			npcData.dwTemplateID        = (DWORD)pNpcInfo->nTempleteID;
			npcData.nX                  = pNpcInfo->nX;
			npcData.nY                  = pNpcInfo->nY;
			npcData.nZ                  = pNpcInfo->nZ;
			npcData.byFaceDirection     = (BYTE)pNpcInfo->nFaceDirection;
			npcData.byKind              = (BYTE)pNpcInfo->nKind;
			npcData.dwScriptID          = pNpcInfo->dwScriptID;
			npcData.dwReliveID          = pNpcInfo->dwNpcReliveID;
			npcData.dwRandomID          = pNpcInfo->dwNpcRandomID;
			npcData.dwThreatLinkID      = pNpcInfo->dwNpcTeamID;
			npcData.nOrderIndex         = (int)pNpcInfo->dwNpcTeamIndex;
			npcData.nPatrolPathID       = pNpcInfo->nPathIndex;

			strncpy(npcData.szNickName, pNpcInfo->szNickName.c_str(), sizeof(npcData.szNickName));
			npcData.szNickName[sizeof(npcData.szNickName) - 1] = '\0';

			piNpcFile->Push(npcData);

			nCount++;

			if (nCount % 16 == 0)
			{
				ShowProgress("Exchange NPC",nCount*1.0F/nTotalCount);
			}

			++it;
		}
	}

	piNpcFile->Save(pLogicalScene->m_szName);

	KG_COM_RELEASE(piNpcFile);
	return ;

Exit0:
	KG_COM_RELEASE(piNpcFile);
	return ;
}

void ExchangeDoodad(KScene *pLogicalScene, KSceneEditorDocLogical* m_lpLogicalData)
{
	BOOL bRetCode = FALSE;
	IDoodadFile* piDoodadFile = NULL;
	KDOODAD_DATA DoodadData;

	ASSERT(pLogicalScene);

	piDoodadFile = ::CreateDoodadFileInterface();
	KGLOG_PROCESS_ERROR(piDoodadFile);

	{
		int nTotalCount = (int)m_lpLogicalData->m_listDoodadInfo.size();
		int nCount = 0;

		list<KSceneEditorDocLogical::DoodadInfo*>::iterator it = m_lpLogicalData->m_listDoodadInfo.begin();
		while (it != m_lpLogicalData->m_listDoodadInfo.end())
		{
			KSceneEditorDocLogical::DoodadInfo* pDoodadInfo = *it;

			m_lpLogicalData->UpdateDoodad(pDoodadInfo);

			memset(&DoodadData, 0, sizeof(DoodadData));

			DoodadData.dwTemplateID     = (DWORD)pDoodadInfo->nTempleteID;
			DoodadData.nX               = pDoodadInfo->nX;
			DoodadData.nY               = pDoodadInfo->nY;
			DoodadData.nZ               = pDoodadInfo->nZ;
			DoodadData.byFaceDirection  = (BYTE)pDoodadInfo->nFaceDirection;
			DoodadData.byLevel          = 0;
			DoodadData.dwDoodadReliveID = pDoodadInfo->dwDoodadReliveID;
			DoodadData.wObstacleGroup   = 0;
			DoodadData.dwScriptID       = pDoodadInfo->dwScriptID;

			strncpy(DoodadData.szNickName, pDoodadInfo->szNickName.c_str(), sizeof(DoodadData.szNickName));
			DoodadData.szNickName[sizeof(DoodadData.szNickName) - 1] = '\0';

			piDoodadFile->Push(DoodadData);

			nCount++;
			if (nCount % 8 == 0)
			{
				ShowProgress("Exchange Doodad",nCount*1.0F/nTotalCount);
			}

			++it;
		}
	}

	piDoodadFile->Save(pLogicalScene->m_szName);

	KG_COM_RELEASE(piDoodadFile);
	return ;

Exit0:
	KG_COM_RELEASE(piDoodadFile);
	return ;
}

void ExchaneNpcRefreshPointSet(LPCTSTR pFileName,LPCTSTR pMapName, KSceneEditorDocLogical* m_lpLogicalData)
{
	/*//NPC刷新点的功能已经没有了// Last modify by Chen TianHong:2008-5-5 13:45:50
	DWORD dwSetCount = 0;

	int nTotalCount = (int)m_lpLogicalData->m_listNPCRefreshPointSet.size();
	int nCount = 0;

	list<KSceneEditorDocLogical::NPCRefreshPointSet*>::iterator i = m_lpLogicalData->m_listNPCRefreshPointSet.begin();
	while (i!=m_lpLogicalData->m_listNPCRefreshPointSet.end())
	{
	KSceneEditorDocLogical::NPCRefreshPointSet* pSet = *i;

	nCount++;
	ShowProgress("Exchange NpcRefreshPointSet",nCount*1.0F/nTotalCount);

	TCHAR Name[MAX_PATH];
	wsprintf(Name,"%sNPCRelive%d.Tab",pFileName,dwSetCount);

	KNpcRelive* pNewRelive = g_pSO3World->m_Settings.m_NpcReliveList.AddNpcRelive((DWORD)m_lpLogicalData->m_nSceneID,dwSetCount+1,Name);
	KGLOG_PROCESS_ERROR(pNewRelive);

	list<KSceneEditorDocLogical::RefreshPoint*>::iterator j = pSet->listRefreshPoint.listRefreshPoint.begin();
	while (j!=pSet->listRefreshPoint.listRefreshPoint.end())
	{
	KSceneEditorDocLogical::RefreshPoint* pPoint = *j;

	pNewRelive->AddRelivePoint(pPoint->nX,pPoint->nY,pPoint->nZ,pPoint->nDirection);
	j++;
	}

	pNewRelive->Save();

	i++;
	dwSetCount++;
	}
	g_pSO3World->m_Settings.m_NpcReliveList.Save((DWORD)m_lpLogicalData->m_nSceneID,pMapName);

	;
	Exit0:
	;*/
}

void ExchaneDoodadRefreshPointSet(LPCTSTR pFileName,LPCTSTR pMapName, KSceneEditorDocLogical* m_lpLogicalData)
{
	/*刷新点的功能已经没有了// Last modify by Chen TianHong:2008-5-5 13:46:08
	int nTotalCount = (int)m_lpLogicalData->m_listDoodadRefreshPointSet.size();
	int nCount = 0;

	DWORD dwSetCount = 0;
	list<KSceneEditorDocLogical::DoodadRefreshPointSet*>::iterator i = m_lpLogicalData->m_listDoodadRefreshPointSet.begin();
	while (i!=m_lpLogicalData->m_listDoodadRefreshPointSet.end())
	{
	KSceneEditorDocLogical::DoodadRefreshPointSet* pSet = *i;

	nCount++;
	ShowProgress("Exchange DoodadRefreshPointSet",nCount*1.0F/nTotalCount);

	TCHAR Name[MAX_PATH];
	wsprintf(Name,"%sDoodadRelive%d.Tab",pFileName,dwSetCount);

	KDoodadRelive* pNewRelive = g_pSO3World->m_Settings.m_DoodadReliveList.AddDoodadRelive((DWORD)m_lpLogicalData->m_nSceneID,dwSetCount+1,Name);
	KGLOG_PROCESS_ERROR(pNewRelive);

	list<KSceneEditorDocLogical::RefreshPoint*>::iterator j = pSet->listRefreshPoint.listRefreshPoint.begin();
	while (j!=pSet->listRefreshPoint.listRefreshPoint.end())
	{
	KSceneEditorDocLogical::RefreshPoint* pPoint = *j;

	pNewRelive->AddRelivePoint(pPoint->nX,pPoint->nY,pPoint->nZ,pPoint->nDirection);
	j++;
	}

	pNewRelive->Save();

	i++;
	dwSetCount++;
	}
	g_pSO3World->m_Settings.m_DoodadReliveList.Save((DWORD)m_lpLogicalData->m_nSceneID,pMapName);
	;
	Exit0:
	;*/
}

void ExchangeAIGroup(LPCTSTR pFileName,LPCTSTR pMapName, KSceneEditorDocLogical* m_lpLogicalData)
{
	int nTotalCount = (int)m_lpLogicalData->m_listAIGroup.size();
	int nCount = 0;

	DWORD dwCount = 0;
	list<KSceneEditorDocLogical::AIGroup*>::iterator i = m_lpLogicalData->m_listAIGroup.begin();
	while (i!=m_lpLogicalData->m_listAIGroup.end())
	{
		KSceneEditorDocLogical::AIGroup* pAI = *i;

		nCount++;
		ShowProgress("Exchange AIGroup",nCount*1.0F/nTotalCount);

		TCHAR Name[MAX_PATH];
		wsprintf(Name,"%sNPCTeam%d.Tab",pFileName,dwCount);

		KNpcTeam* pTeam = NULL;
		g_pSO3World->m_Settings.m_NpcTeamList.AddNpcTeam((DWORD)m_lpLogicalData->m_nSceneID,dwCount+1,pAI->nArrayIndex);
		//pTeam = g_WorldSettings.m_NpcTeamList.GetNpcTeam((DWORD)m_lpLogicalData->m_nSceneID,dwCount+1);

		/*for (size_t n = 0;n<pAI->vecNpcInfo.size();n++)
		{
		KSceneEditorDocLogical::NPCInfo* pNpcInfo = pAI->vecNpcInfo[n];
		if(pNpcInfo->pLogicalNpc)
		{
		pTeam->AddMember(pNpcInfo->pLogicalNpc,(int)n);
		}
		}

		pTeam->SetLeader(0);*/
		//pTeam->SetKing((DWORD)pAI->nKing);

		i++;
		dwCount++;
	}
	g_pSO3World->m_Settings.m_NpcTeamList.Save((DWORD)m_lpLogicalData->m_nSceneID,pMapName);
}

void ExchangeWayPointSet(LPCTSTR pFileName,LPCTSTR pMapName, KSceneEditorDocLogical* m_lpLogicalData)
{
	int nTotalCount = (int)m_lpLogicalData->m_listWayPointSet.size();
	int nCount = 0;
	DWORD dwCount = 0;

	g_pSO3World->m_Settings.m_PatrolPathList.UnInit();
	list<KSceneEditorDocLogical::WayPointSet*>::iterator i = m_lpLogicalData->m_listWayPointSet.begin();
	while (i!=m_lpLogicalData->m_listWayPointSet.end())
	{
		KSceneEditorDocLogical::WayPointSet* pSet = *i;

		nCount++;
		ShowProgress("Exchange WayPointSet",nCount*1.0F/nTotalCount);

		TCHAR Name[MAX_PATH];
		wsprintf(Name,"%sPatrolPathList%d.Tab",pFileName,dwCount);

		KPatrolPath* pPath = g_pSO3World->m_Settings.m_PatrolPathList.AddPatrolPath((DWORD)m_lpLogicalData->m_nSceneID, dwCount+1, Name);
		pPath->m_nOrderID = pSet->nOrderID;
		for (size_t s=0;s<pSet->vectorWayPoint.size();s++)
		{
			KSceneEditorDocLogical::WayPoint* pPoint = pSet->vectorWayPoint[s];

			KPatrolNode Node;
			Node.nPatrolPointX = pPoint->nX;
			Node.nPatrolPointY = pPoint->nY;
			Node.nRestFrame = pPoint->nStayFrame;
			Node.nFaceTo = pPoint->nStayDirection;
			Node.nAnimationID = pPoint->nStayAnimation;
			Node.nAniLoop = pPoint->bStayAniLoop;
			Node.bIsRun = pPoint->bIsRun;
			wsprintf(Node.szScriptName,"%s",pPoint->szScriptName.c_str());

			pPath->AddPoint(Node);
		}
		pPath->Save();

		i++;
		dwCount++;

	}
	g_pSO3World->m_Settings.m_PatrolPathList.Save((DWORD)m_lpLogicalData->m_nSceneID,pMapName);

}

void ExchangeTrafficPointSet(LPCTSTR pFileName, KSceneEditorDocLogical* m_lpLogicalData)
{
	int nTotalCount = (int)m_lpLogicalData->m_mapTrafficPoint.size();
	int nCount = 0;
	DWORD dwCount = 0;
	map<DWORD,KSceneEditorDocLogical::TrafficPoint>::iterator iter = m_lpLogicalData->m_mapTrafficPoint.begin();
	while (iter != m_lpLogicalData->m_mapTrafficPoint.end())
	{
		nCount++;
		ShowProgress("Exchange TrafficPoint",nCount*1.0F/nTotalCount);
		KROUTE_NODE* pNode = g_pSO3World->m_RoadManager.GetNodeInfo(iter->first);
		if (pNode == NULL)
		{
			dwCount++;
			iter++;
			continue;
		}
		pNode->dwMapID = (iter->second).m_dwMapID;
		pNode->dwCityID = (iter->second).m_dwCityID;
		pNode->dwTongID = (iter->second).m_dwTongID;
		pNode->bIsNeedOpen = (iter->second).m_bNeedOpen;
		m_lpLogicalData->SceneToLogical(pNode->nX,pNode->nY,pNode->nZ,(iter->second).vPosition,0);
		wsprintf(pNode->szDescription,"%s",(iter->second).m_strDescription.c_str());
		wsprintf(pNode->szScriptFile,"%s",(iter->second).m_strScriptFile.c_str());
		dwCount++;
		iter++;
	}

	nTotalCount = (int)m_lpLogicalData->m_listTrafficPointSet.size();
	nCount = 0;
	dwCount = 0;
	list<KSceneEditorDocLogical::TrafficPointSet*>::iterator i = m_lpLogicalData->m_listTrafficPointSet.begin();
	while (i != m_lpLogicalData->m_listTrafficPointSet.end())
	{
		KSceneEditorDocLogical::TrafficPointSet* pSet = *i;
		nCount++;
		ShowProgress("Exchange TrafficPointSet",nCount*1.0F/nTotalCount);  
		int nTrackID = 0;
		KTRACK* pKTrack = NULL;

		pKTrack = g_pSO3World->m_RoadManager.GetTrackInfo(pSet->m_dwFromID,pSet->m_dwToID);
		if (pKTrack == NULL)
		{
			dwCount++;
			i++;
			continue;
		}
		pKTrack->nVelocity = pSet->m_dwVelocity;
		pKTrack->nCostMoney = pSet->m_dwCostMoney;

		pKTrack->vPoint.clear();
		KTRACK_POINT TrackPoint;
		DWORD dwFromID = pSet->m_dwFromID;
		for (size_t s=0;s<pSet->vecTrafficPoint.size();s++)
		{	
			m_lpLogicalData->SceneToLogical(TrackPoint.nX,TrackPoint.nY,TrackPoint.nZ,(pSet->vecTrafficPoint[s])->vPosition,0);
			pKTrack->vPoint.push_back(TrackPoint);
		}	
		dwCount++;
		i++;
	}

	nTotalCount = (int)m_lpLogicalData->m_listOverMapPointSet.size();
	nCount = 0;
	dwCount = 0;
	i = m_lpLogicalData->m_listOverMapPointSet.begin();
	while (i != m_lpLogicalData->m_listOverMapPointSet.end())
	{
		KSceneEditorDocLogical::TrafficPointSet* pSet = *i;
		nCount++;
		ShowProgress("Exchange OverMapTrafficPointSet",nCount*1.0F/nTotalCount);  
		int nTrackID = 0;
		KTRACK* pKTrack = NULL;

		pKTrack = g_pSO3World->m_RoadManager.GetTrackInfo(pSet->m_dwID);
		if (pKTrack == NULL)
		{
			dwCount++;
			i++;
			continue;
		}
		pKTrack->nVelocity = pSet->m_dwVelocity;
		pKTrack->nCostMoney = pSet->m_dwCostMoney;
		pKTrack->vPoint.clear();;
		dwCount++;
		i++;
	}
	g_pSO3World->m_RoadManager.ExportTable();
}

