// SO3World.cpp : Implementation of CSO3World

#include "stdafx.h"
#include "SO3World.h"
#include "KSO3World.h"
#include "IndeSource/SO3Represent/SO3Represent.h"
#include "SO3UI/ISO3UI.h"
#include "IEOther.h"
#include "IEEditor.h"
#include "AtlBase.h"
#include "ProgressForm.h"
#include "TopViewForm.h"
#include "Stolen/KG3DSceneLogicalEditorBase.h"
#include "Stolen/KSceneEditorDocLogical.h"
#include "KG3DRTTIType.h"
#include "LoadLogicalObj.h"
#include "Stolen/KLogicObstacleGenerator.h"
#include "SaveLogicalObj.h"
#include "Stolen/KLogicalSceneConnection.h"

extern IEKG3DSceneLogicalEditorBase* g_pLogicEditorBase;
extern KScene* g_pKScene;
extern KLogicalSceneConnection* g_pLogicalConnection;
extern KSceneEditorDocLogical* g_pDocLogical;
extern IEKG3DScene* g_pIEScene;
extern IKGSO3WorldClient* QuerySO3WorldClientInterface();

class CDllHolder
{
public:
	CDllHolder()
	{
		m_hMod = NULL;
	}
	void Init(LPCTSTR lpstrDllName)
	{
		m_hMod = LoadLibrary(lpstrDllName);
		_ASSERTE(m_hMod);
	}
	void UnInit()
	{
		if (m_hMod)
		{
			FreeLibrary(m_hMod);
			m_hMod = NULL;
		}
	}

	~CDllHolder()
	{
		UnInit();
	}
	HMODULE GetDllHandle()
	{
		return m_hMod;
	}
private:
	HMODULE m_hMod;
};

CDllHolder g_ModuleRepresent;
CDllHolder g_ModuleUI;
IKSO3RepresentInterface* g_pSO3Represent;
IKSO3RepresentHandler* g_pRepresentHandler = NULL;
ISO3UI* g_pUI;

extern IEKG3DEngineManager* g_pEngineManager;
extern IEKG3DGraphicsTool* g_pGraphicsTool;
//extern LPDIRECT3DDEVICE9 g_pd3dDevice;
extern IKG3DRTTITypeManager* g_pKG3DRTTITypeManager; ///<RTTI_INTERFACE>
extern IEKG3DSceneSceneEditor* g_pSceneEditor;
extern IEKG3DSceneEditorBase* g_pEditorBase;
extern TCHAR g_szStartUpDirectory[MAX_PATH];


/*
// Func
void UnInitRepresentDLL()
{
	SAFE_RELEASE(g_pRepresent);
	g_ModuleRepresent.UnInit();
}
BOOL InitRepresentDLL()
{
	BOOL bRetCode = FALSE;
	fnCreateSO3Represent pfnCreateSO3Represent = NULL;

	UnInitRepresentDLL();

#ifdef _DEBUG
	g_ModuleRepresent.Init("JX3RepresentD.dll");
	//TCHAR dllpath[MAX_PATH];
	//sprintf(dllpath, "%s/3DProxy/SO3RepresentD.dll", g_szStartUpDirectory);
	//g_ModuleRepresent.Init(dllpath);
#else
	g_ModuleRepresent.Init("JX3Represent.dll");
#endif

	pfnCreateSO3Represent = (fnCreateSO3Represent)GetProcAddress(g_ModuleRepresent.GetDllHandle(), FN_CREATE_SO3REPRESENT);
	KGLOG_PROCESS_ERROR(pfnCreateSO3Represent);

	g_pRepresent = pfnCreateSO3Represent();
	KGLOG_PROCESS_ERROR(g_pRepresent);

	return TRUE;
Exit0:
	return FALSE;
}
*/

void UnInitUI()
{
	if (g_pUI)
	{
		g_pUI->Exit();
		g_pUI = NULL;
	}
	g_ModuleUI.UnInit();
}
BOOL InitUI()
{
	BOOL bRetCode = FALSE;
	fnCreateSO3UI pfnCreateSO3UI = NULL;

	KGLOG_PROCESS_ERROR(g_pEngineManager && "init 3d engine first!");
	//KGLOG_PROCESS_ERROR(g_pRepresent && "init represent first!");

	UnInitUI();

#ifdef _DEBUG
	g_ModuleUI.Init("JX3UID.dll");
#else
	g_ModuleUI.Init("JX3UI.dll");
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

// CSO3World

STDMETHODIMP CSO3World::Init(LONG nWorldIndex)
{
	IRecorderFactory* piRecorderFactory = CreateRecorderFactoryInterface(ermNormal);

	g_pSO3World = new KSO3World();
	KGLOG_PROCESS_ERROR(g_pSO3World);

	if (!g_pSO3World->Init(piRecorderFactory))
		::MessageBox(NULL, "逻辑初始化失败！", "错误", 0);

	//CreateSO3Represent();
	g_pRL = new SO3Represent();
	g_pSO3Represent = g_pRL;
	KGLOG_PROCESS_ERROR(g_pSO3Represent);

	if (!InitUI())
		::MessageBox(NULL, "UI初始化失败！", "错误", 0);

	if (true/*InitRepresentDLL() && InitUI()*/)
	{
		HWND hWnd = NULL;
		g_SetGameWorldUIHandler(&g_pUI->GetGameWorldUIHandler());
		g_SetGameWorldRepresentHandler(g_pSO3Represent->GetGameWorldRepresentHandler());
		g_pRepresentHandler = g_pSO3Represent->GetRepresentHandelr();

		g_pRepresentHandler->AttachGameWorld(g_pSO3World, QuerySO3WorldClientInterface());
		g_pRepresentHandler->Attach3DEngine(g_pEngineManager, g_pEngineManager->Get3DModelTable(), g_pEngineManager->Get3DUI());
		g_pRepresentHandler->AttachUI(g_pUI, &g_pUI->GetGameWorldUIHandler());

		g_pEngineManager->GetBaseWnd(&hWnd);
		g_pRepresentHandler->InitHWND(hWnd);
		
		if (!g_pSO3Represent->Init())
			::MessageBox(NULL, "表现逻辑初始化失败！", "错误", 0);
	}

Exit0:
	return S_OK;
	//return g_GetRepresent().Attach3DEngine(g_pEngineManager, g_pEngineManager->Get3DModelTable(), g_pEngineManager->Get3DUI());
}

STDMETHODIMP CSO3World::UnInit(void)
{
	g_pSO3World->UnInit();
	return S_OK;
}

STDMETHODIMP CSO3World::Activate(void)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSO3World::NewEditorScene(LONG nRegionWidth, LONG nRegionHeight, LONG* pKScene)
{
	// TODO: Add your implementation code here

	return S_OK;
}

extern int ShowProgress(LPCTSTR pName, float fPencent);
STDMETHODIMP CSO3World::LoadEditorScene(BSTR cszFileName)
{
	// load逻辑场景
	g_pKScene = g_pSO3World->LoadEditorScene(BSTR_TO_STRING(cszFileName), &ShowProgress);
	if (!g_pKScene)
	{
		::MessageBox(NULL, "LoadEditorScene失败！", NULL, 0);
	}
	return S_OK;
}

STDMETHODIMP CSO3World::NewClientScene(ULONG dwMapID, LONGLONG nMapCopyIndex, LONG* pKScene)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSO3World::DeleteScene(LONG* pScene)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSO3World::GetScene(LONG dwMapID, LONGLONG nMapCopyIndex, LONG* pScene)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSO3World::NewNpc(LONG dwNpcID, LONG* pNpc)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSO3World::DeleteNpc(LONG* pNpc)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSO3World::AddNpc(LONG* pNpc, LONG* pScene, LONG nX, LONG nY, LONG nZ)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSO3World::RemoveNpc(LONG* pNpc, LONG bKilled)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSO3World::NewDoodad(LONG dwDoodadID, LONG* pDoodad)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSO3World::DelDoodad(BYTE* pDoodad)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSO3World::AddDoodad(LONG* pDoodad, LONG* pScene, LONG nX, LONG nY, LONG nZ)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSO3World::RemoveDoodad(LONG* pDoodad)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSO3World::NewPlayer(LONG dwPlayerID, LONG* pPlayer)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSO3World::DelPlayer(LONG* pPlayer)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSO3World::AddPlayer(LONG* pPlayer, LONG* pScene, LONG nX, LONG nY, LONG nZ)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CSO3World::RemovePlayer(LONG* pPlayer)
{
	// TODO: Add your implementation code here

	return S_OK;
}


STDMETHODIMP CSO3World::AttachLogicalSceneTo3DScene()
{
	HRESULT hr = E_FAIL;
	IKG3DScene* pScene = NULL;
	IEKG3DTerrain *pTerrain = NULL;
	ASSERT(g_pKScene);

	hr = g_pRepresentHandler->GetScene(g_pKScene->m_dwID, &pScene);
	KGLOG_COM_PROCESS_ERROR(hr);
	g_pEngineManager->SetFocusScene(pScene);

	g_pIEScene = dynamic_cast<IEKG3DScene*>(pScene);
	g_pIEScene->GetSceneSceneEditor(&g_pSceneEditor);
	ASSERT(g_pSceneEditor);
	g_pSceneEditor->GetSceneEditorBase(&g_pEditorBase);

	//SetScene(piScene); // 返回IEscene, 外面做
	g_pLogicEditorBase->SetLogicScene(g_pKScene);
	g_pSceneEditor->IESetLogicalScene(g_pLogicEditorBase);
	//m_lpLogicalData->SetSceneEditor(pEditor); // 需要处理

	return S_OK;
Exit0:
	return hr;
}

STDMETHODIMP CSO3World::NewLogicSceneConnectionAndBindLogicalSceneAndSceneEditor()
{
	g_pLogicalConnection = new KLogicalSceneConnection();
	g_pLogicalConnection->BindLogicalScene(g_pKScene);
	g_pSceneEditor->SetLogicalSceneConnection(g_pLogicalConnection);
	return S_OK;
}

STDMETHODIMP CSO3World::NewLogicalDataAndAdviseWithSceneEditor()
{
	g_pDocLogical = new KSceneEditorDocLogical();
	g_pSceneEditor->Advise(g_pDocLogical);
	return S_OK;
}

STDMETHODIMP CSO3World::LoadLogicalData(BSTR fileName)
{
	g_pDocLogical->SetSceneEditor(g_pSceneEditor);
	g_pDocLogical->LoadFromIniFile(BSTR_TO_STRING(fileName), &ShowProgress, TRUE);
	LoadRepresentNPC();
 	LoadRepresentDoodad();
 	LoadRepresentRefreshPoint();
 	LoadRepresentWayPoint();
 	LoadRepresentTrafficPoint();
	return S_OK;
}

STDMETHODIMP CSO3World::SaveEditorIni(BSTR relaPath)
{
	g_pDocLogical->SaveToIniFile(BSTR_TO_STRING(relaPath));
	return S_OK;
}

HRESULT TransformAllHeightToCellHeight(IEKG3DSceneSceneEditor* pEditorA,KScene* pLogicalScene,LONG bOnlyAppointCell,LONG bClearCell,IELogicalSceneConnection* pLogicalConnection)
{
	HRESULT                 hResult                 = E_FAIL;
	int                     nRetCode                = false;
	BOOL                    bUseSO3Logic            = false;
	IEKG3DScene*            pi3DScene               = NULL; 
	KLogicObstacleGenerator LogicObstacleGenerator;

	nRetCode = pEditorA->GetScene(&pi3DScene);
	KGLOG_COM_PROCESS_ERROR(nRetCode);
	ASSERT(pi3DScene);

	pLogicalConnection->BindLogicalScene(pLogicalScene);
	pEditorA->SetLogicalSceneConnection(pLogicalConnection);

	nRetCode = LogicObstacleGenerator.GenerateSceneLogicObstacleData(
		pi3DScene, pLogicalScene,bOnlyAppointCell,bClearCell
		);
	KGLOG_COM_PROCESS_ERROR(nRetCode);

	hResult = S_OK;
Exit0:
	return hResult;
}
HRESULT TransformatCell(KScene* lpLogicalScene, IEKG3DSceneSceneEditor* pEditor, LONG bOnlyAppointCell, LONG bClearCell, IELogicalSceneConnection* pLogicalConnection)
{
	HRESULT bRetCode = NULL;
	if (!bOnlyAppointCell)
	{
		bRetCode = lpLogicalScene->DeleteAllObstacle();
		KGLOG_CHECK_ERROR(bRetCode);
	}
	TransformAllHeightToCellHeight(pEditor,lpLogicalScene,bOnlyAppointCell,bClearCell,pLogicalConnection);
	return S_OK;
}

HRESULT SaveAllCellRest(LONG bAllRest)
{
	int iAllRest = bAllRest ? 1 : 0;

	HRESULT hrRetCode = NULL;
	int nRetCode = false;

	int nX_Start,nZ_Start,nX_Width,nZ_Width;
	int nX_LogicRegionStart = 0, nY_LogicRegionStart = 0, nX_LogicRegionEnd = 0, nY_LogicRegionEnd = 0;
	int nCellCountX = 0, nCellCountY = 0, nRegionCountX = 0, nRegionCountY = 0;

	g_pIEScene->GetLogicalSceneSize(&nX_Start,&nZ_Start,&nX_Width,&nZ_Width);
	KGLOG_PROCESS_ERROR(nX_Width > 0);
	KGLOG_PROCESS_ERROR(nZ_Width > 0);

	nCellCountX = nX_Width * 100  / LOGICAL_CELL_CM_LENGTH;
	nCellCountY = nZ_Width * 100  / LOGICAL_CELL_CM_LENGTH;

	KGLOG_PROCESS_ERROR(nCellCountX > 0);
	KGLOG_PROCESS_ERROR(nCellCountY > 0);

	nRegionCountX = (nCellCountX - 1) / REGION_GRID_WIDTH + 1;
	nRegionCountY = (nCellCountY - 1) / REGION_GRID_HEIGHT + 1;

	nRetCode = g_pKScene->Resize(nRegionCountX, nRegionCountY, eResizeSceneAlignLeftBottom);
	KGLOG_PROCESS_ERROR(nRetCode);

	nX_LogicRegionEnd = nX_LogicRegionStart + nRegionCountX;
	nY_LogicRegionEnd = nY_LogicRegionStart + nRegionCountY;

	int nTotalRegion = nRegionCountX * nRegionCountY;

	char szMessage[128];
	int nCompleteRegion = 0;

	// 遍历每个Region
	for (int nY = nY_LogicRegionStart; nY < nY_LogicRegionEnd; nY++)
	{
		for (int nX = nX_LogicRegionStart; nX < nX_LogicRegionEnd; nX++)
		{
			KRegion* pLogicRegion = g_pKScene->GetRegion(nX, nY);
			KGLOG_PROCESS_ERROR(pLogicRegion);

			// 每个cell
			for (int nYCell = 0; nYCell < REGION_GRID_HEIGHT; nYCell++)
			{
				for (int nXCell = 0; nXCell < REGION_GRID_WIDTH; nXCell++)
				{
					KCell* pLowestCell  = pLogicRegion->GetLowestObstacle(nXCell, nYCell);
					pLowestCell->m_BaseInfo.dwRest = iAllRest;
				}
			}

			nCompleteRegion++;
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

STDMETHODIMP CSO3World::SaveCalcCollision(LONG bOnlyAppointCell, LONG bClearCell)
{
	HRESULT hr = NULL;
	if (bOnlyAppointCell)
	{
		int nXStart,nZStart,nCountX,nCountZ;
		g_pIEScene->GetAppointLogicalRegion(nXStart,nZStart,nCountX,nCountZ);
		D3DXVECTOR2 A(nXStart* 3200.0F,nZStart * 3200.0F);
		D3DXVECTOR2 C = A + D3DXVECTOR2(nCountX*3200.0F,nCountZ* 3200.0F);
		hr = g_pSceneEditor->ForceLoadEntity(A,C);
	}
	else
	{
		hr = g_pSceneEditor->ForceLoadAllEntity();
	}
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = TransformatCell(g_pKScene, g_pSceneEditor, bOnlyAppointCell, bClearCell, g_pLogicalConnection);
	KGLOG_COM_PROCESS_ERROR(hr);
	
	return S_OK;
Exit0:
	return hr;
}

STDMETHODIMP CSO3World::UpdateAllRest(LONG bRest)
{
	SaveAllCellRest(bRest);
	return S_OK;
}

STDMETHODIMP CSO3World::SaveLogicalScene(BSTR fileName)
{
	ExchangeLogicalData(BSTR_TO_STRING(fileName), g_pKScene, g_pDocLogical);
	return S_OK;
}

STDMETHODIMP CSO3World::GetLogicalDataPtr(IAtlIEKSceneEditorDocLogical** ppLogicalData)
{
	*ppLogicalData = (IAtlIEKSceneEditorDocLogical*)((LONG)g_pDocLogical);
	return S_OK;
}

