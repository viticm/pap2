// AtlBase.cpp : Implementation of CAtlBase

#include "stdafx.h"
#include "AtlBase.h"
#include "KSO3World.h"
#include "SO3World.h"
#include "SO3RepresentInterface.h"
#include "SO3UI/ISO3UI.h"
#include "IEOther.h"
#include "ProgressForm.h"
#include "TopViewForm.h"
#include "Stolen/KG3DSceneLogicalEditorBase.h"
#include "Stolen/KSceneEditorDocLogical.h"
#include "KG3DRTTIType.h"
#include "IEKG3DSceneOutputWnd.h"
#include "LoadLogicalObj.h"
#include "Stolen/KSceneHelperFuncs.h"
#include "KNpcTemplateList.h"
#include "KG3DRepresentObject.h"
#include "IEKG3DModelTable.h"
#include "Stolen/KLogicalSceneConnection.h"


// Engine interface
IEKG3DEngineManager* g_pEngineManager = NULL;
HMODULE g_EngineDll = NULL;

// Logics interface
//just use g_pSO3World.

// Scene interface
LONG g_pFocusScene = NULL;

// Loading progress show
CProgressForm g_ProgressDlg;
CStatic g_ProgressName;
CProgressCtrl g_ProgressProgress;
BOOL g_bProgressInited = FALSE;

// topview
CTopViewForm g_TopViewForm;

// global var
IEKG3DGraphicsTool* g_pGraphicsTool = NULL;
BYTE* g_SceneEditorKeyState = NULL;
TCHAR g_szDefWorkDirectory[MAX_PATH];
TCHAR g_szStartUpDirectory[MAX_PATH];
LPDIRECT3DDEVICE9  g_pd3dDevice = NULL;
IKG3DRTTITypeManager* g_pKG3DRTTITypeManager = NULL;
extern IKSO3RepresentHandler* g_pRepresentHandler;
IEKG3DScene* g_pIEScene = NULL;
IEKG3DSceneSceneEditor* g_pSceneEditor = NULL;
IEKG3DSceneEditorBase* g_pEditorBase = NULL;
INT g_iSceneOutputWindowID = 0;
KSceneEditorDocLogical* g_pDocLogical = NULL;
IEKG3DSceneLogicalEditorBase* g_pLogicEditorBase = NULL;
KScene* g_pKScene = NULL;
KLogicalSceneConnection* g_pLogicalConnection = NULL;

// ahpho suntao
IEKG3DScene* g_pNpcScene = NULL;
IEKG3DScene* g_pDoodadScene = NULL;
INT g_pNpcSceneWindowID = 0;
INT g_pDoodadSceneWindowID = 0;
CInputProxy* g_NpcInputProxy = NULL;
CInputProxy* g_DoodadInputProxy = NULL;
IEKG3DModel* g_pLastNpcModel;
IEKG3DModel* g_pLastDoodadModel;
IEKG3DRepresentObject* g_NpcRepresentObject = NULL;
IEKG3DRepresentObject* g_DoodadRepresentObject = NULL;
// end ahpho

// syntax sugar
#define GETEDITOR() \
	IEKG3DScene* pScene = (IEKG3DScene*)g_pFocusScene;\
	IEKG3DSceneSceneEditor* pEditor = NULL;\
	pScene->GetSceneSceneEditor(&pEditor);\
	_ASSERTE(pEditor); \
	if(!pEditor) return
#define GETSCENEBASE() \
	IEKG3DSceneEditorBase* pBase = NULL;\
	pEditor->GetSceneEditorBase(&pBase); \
	_ASSERTE(pBase);\
	if (!pBase) return


STDMETHODIMP CAtlBase::SetRootPath(BSTR str3DEngineDir)
{
	g_SetRootPath(BSTR_TO_STRING(str3DEngineDir));
	SetCurrentDirectory(BSTR_TO_STRING(str3DEngineDir));
	SetDefWorkingDir(str3DEngineDir);
	return S_OK;
}

STDMETHODIMP CAtlBase::FrameMove()
{
	g_pEngineManager->FrameMove();
	return S_OK;
}
STDMETHODIMP CAtlBase::Render()
{
	g_pEngineManager->Render();
	return S_OK;
}
STDMETHODIMP CAtlBase::ProcessMessage(ULONG message, LONG wParam, LONG lParam)
{
	g_pEngineManager->ProcessMessage(message, (WPARAM)wParam, (LPARAM)lParam);
	return S_OK;
}

STDMETHODIMP CAtlBase::Init3DEngineManager(BSTR strStartUpPath, LONG ihWnd)
{
//	TCHAR dllpath[MAX_PATH];
//	//StrCpyN(dllpath, TEXT("KG3DEngineSD.dll"), MAX_PATH);
//	sprintf(g_szStartUpDirectory, BSTR_TO_STRING(strStartUpPath));
//	sprintf(dllpath, BSTR_TO_STRING(strStartUpPath));
//	strcat(dllpath, "/3DProxy/");
//	g_SetRootPath(dllpath);
//#ifdef _DEBUG
//	strcat(dllpath, "KG3DEngineS.dll");
//#else
//	strcat(dllpath, "KG3DEngineS.dll");
//#endif

	SetRootPath(STRING_TO_BSTR(g_szDefWorkDirectory));

	//MessageBox(NULL, "pre load kg3dEngine.dll", "info", 0);

#ifdef _DEBUG
	g_EngineDll = LoadLibrary("KG3DEngineD.dll");
#else
	g_EngineDll = LoadLibrary("KG3DEngine.dll");
#endif

	if (!g_EngineDll)
		MessageBox(NULL, "加载KG3DEngine失败", "失败", 0);
	
	typedef HRESULT (*pfnGet3DEngineInterface)(void**);
	pfnGet3DEngineInterface Get3DEngineInterface = (pfnGet3DEngineInterface)GetProcAddress(g_EngineDll, "Get3DEngineInterface");

	Get3DEngineInterface((void**)&g_pEngineManager);

	//MessageBox(NULL, "pre g_pEngineManager->Init", "info", 0);
	g_pEngineManager->Init(0, KG3DENG_DESIGN, (HWND)ihWnd, (HWND)ihWnd);
	g_pEngineManager->SetAutoReloadTexture(TRUE);
	BOOL* pbEnable = NULL;
	g_pEngineManager->GetEnableAddEntity(&pbEnable);
	*pbEnable = FALSE;

	return S_OK;
}

STDMETHODIMP CAtlBase::SetDefWorkingDir(BSTR strDir)
{
	lstrcpy(g_szDefWorkDirectory, BSTR_TO_STRING(strDir));
	return S_OK;
}

STDMETHODIMP CAtlBase::GetSO3World(ISO3World** ptr)
{
	LPVOID* pVoid = (LPVOID*)ptr;
	*pVoid = (IUnknown*)(g_pSO3World);
	
	return S_OK;
}

void InitGVar()
{
	g_pEngineManager->GetGraphicsTool(&g_pGraphicsTool);
	g_pEngineManager->GetDefWorkDirectory(g_szDefWorkDirectory, sizeof(g_szDefWorkDirectory));
	g_pEngineManager->GetD3DDevice(&g_pd3dDevice);
	g_pEngineManager->GetKeyStateBuffer(&g_SceneEditorKeyState);
	g_pEngineManager->GetKG3DRTTITypeManager(&g_pKG3DRTTITypeManager);///<RTTI_INTERFACE>
	_ASSERTE(g_pKG3DRTTITypeManager);
}

STDMETHODIMP CAtlBase::InitGlobalVars()
{
	InitGVar();
	return S_OK;
}

int ShowProgress(LPCTSTR pName, float fPencent)
{
	g_ProgressName.SetWindowText(pName);
	g_ProgressProgress.SetPos((int)(fPencent * 100));
	return 0;
}
STDMETHODIMP CAtlBase::ShowProgressForm(LONG bShow)
{
	if (bShow)
	{
		if (!g_bProgressInited)
		{
			g_bProgressInited = TRUE;
			g_ProgressDlg.Create(NULL);
			g_ProgressDlg.CenterWindow();			

			HWND hName = g_ProgressDlg.GetDlgItem(IDC_STATIC_NAME);
			g_ProgressName.Attach(hName);
			HWND hProgress = g_ProgressDlg.GetDlgItem(IDC_PROGRESS1);
			g_ProgressProgress.Attach(hProgress);
		}
		g_ProgressDlg.ShowWindow(SW_SHOW);
	}
	else
	{
		g_ProgressDlg.ShowWindow(SW_HIDE);
		g_ProgressName.SetWindowText("");
		g_ProgressProgress.SetPos(0);
	}

	return S_OK;
}

STDMETHODIMP CAtlBase::ShowTopViewForm(LONG bShow)
{
	GETEDITOR() E_FAIL;
	g_TopViewForm.Create(NULL);
	g_TopViewForm.CenterWindow();
	g_TopViewForm.ShowWindow(SW_SHOW);
	g_TopViewForm.InitTopView(pEditor);
	return S_OK;
}

STDMETHODIMP CAtlBase::UnInitEngine(void)
{
	g_pEngineManager->UnInit();
	FreeLibrary(g_EngineDll);
	return S_OK;
}

void InitLogicalEditor()
{
	g_pLogicEditorBase = new KG3DSceneLogicalEditorBase();
	g_pLogicEditorBase->Init();
}
STDMETHODIMP CAtlBase::InitLogicalEditorBase()
{
	InitLogicalEditor();
	return S_OK;
}

STDMETHODIMP CAtlBase::InitKeyStateBuf()
{
	g_pEngineManager->GetKeyStateBuffer(&g_SceneEditorKeyState);
	return S_OK;
}

STDMETHODIMP CAtlBase::UpdateKeyState()
{
	g_SceneEditorKeyState[VK_LBUTTON] = !!(GetAsyncKeyState(VK_LBUTTON) & 0x8000);
	g_SceneEditorKeyState[VK_MBUTTON] = !!(GetAsyncKeyState(VK_MBUTTON) & 0x8000);
	g_SceneEditorKeyState[VK_RBUTTON] = !!(GetAsyncKeyState(VK_RBUTTON) & 0x8000);
	g_SceneEditorKeyState[VK_MENU]    = !!(GetAsyncKeyState(VK_MENU)    & 0x8000);
	g_SceneEditorKeyState[VK_CONTROL] = !!(GetAsyncKeyState(VK_CONTROL) & 0x8000);
	g_SceneEditorKeyState[VK_SHIFT]   = !!(GetAsyncKeyState(VK_SHIFT)   & 0x8000);
	return S_OK;
}

STDMETHODIMP CAtlBase::SetFocusScene(LONG pScene)
{
	g_pFocusScene = pScene;
	g_pEngineManager->SetFocusScene((IKG3DScene*)g_pFocusScene);
	return S_OK;
}

STDMETHODIMP CAtlBase::MsgOnMouseWheel(SHORT nDelta)
{
// 	GETEDITOR() E_FAIL;
// 	GETSCENEBASE() E_FAIL;
// 	POINT pt;
// 	pt.x = pt.y = 0;
// 	pBase->ZoomView(nDelta, pt);
	return S_OK;
}

STDMETHODIMP CAtlBase::GetSceneEditorCameraTopScale(FLOAT* fScale)
{
	*fScale = g_pSceneEditor->GetCameraTopScale();
	return S_OK;
}

STDMETHODIMP CAtlBase::SetCurDir(void)
{
	::SetCurrentDirectory(g_szDefWorkDirectory);
	return S_OK;
}

STDMETHODIMP CAtlBase::QuaternionRotationByNDir(FLOAT* x, FLOAT* y, FLOAT* z, FLOAT* w, LONG nDir)
{
	D3DXQUATERNION rotation(*x, *y, *z, *w);
	FLOAT fAngle = (nDir - 192) * 2 * D3DX_PI / 256;
	D3DXVECTOR3 axis(0.0f, 1.0f, 0.0f);
	D3DXQuaternionRotationAxis(&rotation, &axis, fAngle);
	*x = rotation.x;
	*y = rotation.y;
	*z = rotation.z;
	*w = rotation.w;
	return S_OK;
}

STDMETHODIMP CAtlBase::GetRepresentObjTranslation(LONG representObjPtr, AtlVector3* vec)
{
	KG3DRepresentObject* representObj = (KG3DRepresentObject*)representObjPtr;
	DWORD t = representObj->GetType();

	if (t > REPRESENTOBJECT_DEFAULT && t < REPRESENTOBJECT_NONE) // 不是多边形
	{
		D3DXVECTOR3 vPos = representObj->GetTranslation();
		vec->x = vPos.x;
		vec->y = vPos.y;
		vec->z = vPos.z;
	}
	else
	{
		IEKG3DRepresentPoly* pIEPoly = (IEKG3DRepresentPoly*)representObjPtr;
		D3DXVECTOR3 d3dvec = pIEPoly->GetVertex(0);
		vec->x = d3dvec.x;
		vec->y = d3dvec.y;
		vec->z = d3dvec.z;
	}
	return S_OK;
}


// ahoho suntao
STDMETHODIMP CAtlBase::LoadNpcScene(LONG hwndOutput)
{
	g_pEngineManager->NewOneScene(SCENETYPE_MODELEDITOR, (IKG3DScene**)&g_pNpcScene);

	if(!g_pNpcScene)
	{
		MessageBox(NULL, "创建场景失败", "失败", 0);
		return FALSE;
	}

	if (FAILED(g_pNpcScene->LoadFromFile("", 0, 0)))
	{
		SAFE_RELEASE(g_pNpcScene);
		MessageBox(NULL, "创建场景失败", "失败", 0);
		return FALSE;
	}

	g_pEngineManager->SetFocusScene(g_pNpcScene);

	g_pNpcScene->AddOutputWindow("SceneMain", (HWND)hwndOutput, OUTPUTWND_MODELEDITOR_MAIN, 0, &g_pNpcSceneWindowID);
	ASSERT(g_pNpcScene->GetCurrentCamera());
		
	IKG3DCamera* pCamera = g_pNpcScene->GetCurrentCamera();
	D3DXVECTOR3 vCameraPos(0.f, 250.f, -500.f);
	pCamera->SetPosition(vCameraPos);

	IEKG3DSceneOutputWnd *piSceneCurOutputWnd = NULL;
	g_pNpcScene->GetCurOutputWnd(&piSceneCurOutputWnd);
	if (piSceneCurOutputWnd)
	{
		piSceneCurOutputWnd->InitCameraStation("Config.ini");
	}

	g_pNpcScene->SetCurrentOutputWindow(g_pNpcSceneWindowID);
	
	IEKG3DSceneModelEditor* pModelEditor = NULL;
	g_pNpcScene->GetSceneModelEditor(&pModelEditor);
	_ASSERTE(pModelEditor);
	if (!pModelEditor) 
	{
		return E_FAIL;
	}

	IEKG3DSceneEditorBase* pBase = NULL;
	g_pNpcScene->GetSceneEditorBase(&pBase);
	_ASSERTE(pBase);
	if (!pBase)
	{
		return E_FAIL;
	}

	g_NpcInputProxy = new CInputProxy();		
	_ASSERTE(g_NpcInputProxy);
	static PROXYKEYMAP KEYMAP[] =
	{	//           C  S  A  L  M  R  ?  Trigger		  ID     Prev Ig   Exclusive
		{VK_RBUTTON, 0, 0, 0, 0, 0, 1, 0, WM_MOUSEMOVE,   1,     0,   0,   0},	// 鼠标右键, 旋转摄像机镜头
		{VK_LBUTTON, 0, 0, 0, 1, 0, 0, 0, WM_MOUSEMOVE,   2,     0,   0,   0},	// 鼠标左键, 快速缩放视角
		{VK_MBUTTON, 0, 0, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   3,     0,   0,   0},	// 鼠标中键, PAN 视图
		{0		   , 0, 0, 0, 0, 0, 0, 0, 0             , 0,     0,   0},	    // 结束
	};

	g_NpcInputProxy->Init(pBase, CWnd::FromHandle((HWND)hwndOutput), KEYMAP);
	g_pLastNpcModel = NULL;

	return S_OK;
}
STDMETHODIMP CAtlBase::FocusNpcScene()
{
	g_pEngineManager->SetFocusScene(g_pNpcScene);
	g_pNpcScene->SetCurrentOutputWindow(g_pNpcSceneWindowID);
	return S_OK;
}
STDMETHODIMP CAtlBase::FocusDoodadScene()
{
	g_pEngineManager->SetFocusScene(g_pDoodadScene);
	g_pDoodadScene->SetCurrentOutputWindow(g_pDoodadSceneWindowID);
	return S_OK;
}
STDMETHODIMP CAtlBase::LoadDoodadScene(LONG hwndOutput)
{
	g_pEngineManager->NewOneScene(SCENETYPE_MODELEDITOR, (IKG3DScene**)&g_pDoodadScene);

	if(!g_pDoodadScene)
	{
		MessageBox(NULL, "创建场景失败", "失败", 0);
		return FALSE;
	}

	if (FAILED(g_pDoodadScene->LoadFromFile("", 0, 0)))
	{
		SAFE_RELEASE(g_pDoodadScene);
		MessageBox(NULL, "创建场景失败", "失败", 0);
		return FALSE;
	}

	g_pEngineManager->SetFocusScene(g_pDoodadScene);

	g_pDoodadScene->AddOutputWindow("SceneMain", (HWND)hwndOutput, OUTPUTWND_MODELEDITOR_MAIN, 0, &g_pDoodadSceneWindowID);	
	ASSERT(g_pDoodadScene->GetCurrentCamera());

	IKG3DCamera* pCamera = g_pDoodadScene->GetCurrentCamera();
	D3DXVECTOR3 vCameraPos(0.f, 250.f, -500.f);
	pCamera->SetPosition(vCameraPos);

	IEKG3DSceneOutputWnd *piSceneCurOutputWnd = NULL;
	g_pDoodadScene->GetCurOutputWnd(&piSceneCurOutputWnd);
	if (piSceneCurOutputWnd)
	{
		piSceneCurOutputWnd->InitCameraStation("Config.ini");
	}

	g_pDoodadScene->SetCurrentOutputWindow(g_pDoodadSceneWindowID);

	IEKG3DSceneModelEditor* pModelEditor = NULL;
	g_pDoodadScene->GetSceneModelEditor(&pModelEditor);
	_ASSERTE(pModelEditor);
	if (!pModelEditor) 
	{
		return E_FAIL;
	}

	IEKG3DSceneEditorBase* pBase = NULL;
	g_pDoodadScene->GetSceneEditorBase(&pBase);
	_ASSERTE(pBase);
	if (!pBase)
	{
		return E_FAIL;
	}

	g_DoodadInputProxy = new CInputProxy();
	_ASSERTE(g_DoodadInputProxy);
	static PROXYKEYMAP KEYMAP[] =
	{	//           C  S  A  L  M  R  ?  Trigger		  ID     Prev Ig   Exclusive
		{VK_RBUTTON, 0, 0, 0, 0, 0, 1, 0, WM_MOUSEMOVE,   1,     0,   0,   0},	// 鼠标右键, 旋转摄像机镜头
		{VK_LBUTTON, 0, 0, 0, 1, 0, 0, 0, WM_MOUSEMOVE,   2,     0,   0,   0},	// 鼠标左键, 快速缩放视角
		{VK_MBUTTON, 0, 0, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   3,     0,   0,   0},	// 鼠标中键, PAN 视图
		{0		   , 0, 0, 0, 0, 0, 0, 0, 0             , 0,     0,   0},	    // 结束
	};

	g_DoodadInputProxy->Init(pBase, CWnd::FromHandle((HWND)hwndOutput), KEYMAP);

	g_pLastDoodadModel = NULL;

	return S_OK;
}

STDMETHODIMP CAtlBase::LoadNpcModel(BSTR fileName)
{
	if (FAILED(g_pEngineManager->NewKG3DRepresentObject(REPRESENTOBJECT_DEFAULT, 0, 0, &g_NpcRepresentObject)))
	{
		return E_FAIL;
	}

	if (g_pLastNpcModel)
	{
		g_pLastNpcModel->Release();
	}
	
	// 自动恢复镜头，不然误操作的话就没办法了
	IKG3DCamera* pCamera = g_pNpcScene->GetCurrentCamera();
	D3DXVECTOR3 vCameraPos(0.f, 250.f, -500.f);
	pCamera->SetPosition(vCameraPos);

	g_NpcRepresentObject->SetModel(0, BSTR_TO_STRING(fileName), NULL, NULL, TRUE);
	IKG3DModel *pModel = NULL;
	g_NpcRepresentObject->GetModel(pModel, 0);
	g_pLastNpcModel = dynamic_cast<IEKG3DModel*>(pModel);

	IEKG3DSceneModelEditor* pModelEditor = NULL;
	g_pNpcScene->GetSceneModelEditor(&pModelEditor);
	_ASSERTE(pModelEditor);
	pModelEditor->OpenModel(g_pLastNpcModel, FALSE);	

	return S_OK;
}

STDMETHODIMP CAtlBase::LoadFace(BSTR meshFileName)
{
	HRESULT hr = E_FAIL;
	SAFE_RELEASE(g_pLastNpcModel);
	IEKG3DModelTable* piModelTable = NULL;
	IKG3DModel* pPlayerModel = NULL;
	USES_CONVERSION;

	hr = g_pEngineManager->GetIEKG3DModelTable(&piModelTable);
	hr = piModelTable->IELoadResourceFromFile(BSTR_TO_STRING(meshFileName), 0, 0, (IKG3DResourceBase**)&g_pLastNpcModel);
	KG_COM_PROCESS_ERROR(hr);
	g_NpcRepresentObject->GetModel(pPlayerModel, 0);

	//ASSERT(pPlayerModel);	
	if (pPlayerModel)
	{
		g_pLastNpcModel->BindToSocket(pPlayerModel, "s_face");
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

STDMETHODIMP CAtlBase::LoadPluginModel(BSTR strFileName, BSTR strBindDest)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	HRESULT hr = E_FAIL;
	IKG3DModel* pPlayerModel = NULL;
	IKG3DModel* pItemModel = NULL;
	IEKG3DModelTable* piModelTable = NULL;
	TCHAR strBindDestConverted[MAX_PATH];
	USES_CONVERSION;
	
	hr = g_pEngineManager->GetIEKG3DModelTable(&piModelTable);	
	hr = piModelTable->IELoadResourceFromFile(BSTR_TO_STRING(strFileName),0,0,(IKG3DResourceBase**)&pItemModel);
	KG_COM_PROCESS_ERROR(hr);
	g_NpcRepresentObject->GetModel(pPlayerModel, 0);

	if (!pPlayerModel)
	{
		goto Exit1;
	}

	strcpy_s(strBindDestConverted, 
		MAX_PATH,
		BSTR_TO_STRING(strBindDest));
	_strlwr_s(strBindDestConverted);

	if (strstr(strBindDestConverted, "s_") == strBindDestConverted)
	{
		pItemModel->BindToSocket(pPlayerModel, strBindDestConverted);
	}
	else
	{
		pItemModel->BindToBone(pPlayerModel, strBindDestConverted);
	}

Exit1:
	return S_OK;
Exit0:
	return E_FAIL;
}

STDMETHODIMP CAtlBase::LoadPluginMaterial(BSTR strSocketName, BSTR strMtlFile)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IKG3DModel *pModel = NULL;
	IEKG3DModel *peModel = NULL;
	std::vector<IEKG3DModel*> vecChildModels;

	KG_PROCESS_ERROR(g_NpcRepresentObject);

	HRESULT hr = g_NpcRepresentObject->GetModel(pModel, 0);
	KG_COM_PROCESS_ERROR(hr);

	peModel = dynamic_cast<IEKG3DModel*>(pModel);
	KG_PROCESS_ERROR(peModel);

	INT iCount = peModel->GetBindToSocketModel(BSTR_TO_STRING(strSocketName), vecChildModels);

	if (iCount == 1)
	{
		IEKG3DModel* pluginMdl = vecChildModels[0];
		hr = pluginMdl->LoadMaterialFromFile(BSTR_TO_STRING(strMtlFile), 0);
	}

Exit0:
	return hr;
}

STDMETHODIMP CAtlBase::LoadDoodadModel(BSTR fileName)
{
	if (FAILED(g_pEngineManager->NewKG3DRepresentObject(REPRESENTOBJECT_DEFAULT, 0, 0, &g_DoodadRepresentObject)))
	{
		return E_FAIL;
	}

	if (g_pLastDoodadModel)
	{
		g_pLastDoodadModel->Release();
	}

	// 自动恢复镜头，不然误操作的话就没办法了
	IKG3DCamera* pCamera = g_pDoodadScene->GetCurrentCamera();
	D3DXVECTOR3 vCameraPos(0.f, 250.f, -500.f);
	pCamera->SetPosition(vCameraPos);

	g_DoodadRepresentObject->SetModel(0, BSTR_TO_STRING(fileName), NULL, NULL, TRUE);
	IKG3DModel *pModel = NULL;
	g_DoodadRepresentObject->GetModel(pModel, 0);
	g_pLastNpcModel = dynamic_cast<IEKG3DModel*>(pModel);

	IEKG3DSceneModelEditor* pModelEditor = NULL;
	g_pDoodadScene->GetSceneModelEditor(&pModelEditor);
	_ASSERTE(pModelEditor);
	pModelEditor->OpenModel((IEKG3DModel*)pModel, FALSE);	

	return S_OK;
}

STDMETHODIMP CAtlBase::OnWndMsg(PMSG WndMsg, INT* pnRet, BSTR preViewType)
{
	extern BYTE* g_SceneEditorKeyState;

	PMSG pMsg = (PMSG)WndMsg;

	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		g_SceneEditorKeyState[pMsg->wParam] = !!(GetAsyncKeyState(INT(pMsg->wParam)) & 0x8000);
	}

	if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST && pMsg->message != WM_MOUSEMOVE || pMsg->message == WM_COMMAND)
	{	
		g_SceneEditorKeyState[VK_LBUTTON] = !!(GetAsyncKeyState(VK_LBUTTON) & 0x8000);
		g_SceneEditorKeyState[VK_MBUTTON] = !!(GetAsyncKeyState(VK_MBUTTON) & 0x8000);
		g_SceneEditorKeyState[VK_RBUTTON] = !!(GetAsyncKeyState(VK_RBUTTON) & 0x8000);
	}
	
	if (_strcmpi(BSTR_TO_STRING(preViewType), "NPC") == 0 && g_NpcInputProxy)
	{
		g_NpcInputProxy->CheckInputEvent(pMsg);
	}
	else if (_strcmpi(BSTR_TO_STRING(preViewType), "Doodad") == 0 && g_DoodadInputProxy)
	{
		g_DoodadInputProxy->CheckInputEvent(pMsg);
	}	

	return S_OK;
}

STDMETHODIMP CAtlBase::OnResizeNpcWindow(void)
{
	if (g_pNpcScene)
	{
		g_pNpcScene->OnResizeWindow(NULL);
	}

	return S_OK;
}

STDMETHODIMP CAtlBase::OnResizeDoodadWindow(void)
{
	if (g_pDoodadScene)
	{
		g_pDoodadScene->OnResizeWindow(NULL);
	}

	return S_OK;
}

STDMETHODIMP CAtlBase::ModelPlayAnimation(BSTR fileName)
{	
	if (g_pLastNpcModel)
	{
		HRESULT hr = g_pLastNpcModel->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, BSTR_TO_STRING(fileName), 1.0f, 0.0f, NULL, NULL, ANICTL_PRIMARY);
		if (hr == E_FAIL)
		{
			CString str;
			str.Format("当前所选的动作文件不可用");
			MessageBoxA(NULL,str,"提示",MB_ICONINFORMATION);
		}
	}
	
	return S_OK;
}

STDMETHODIMP CAtlBase::SwitchDoodadBrush(LONG nTemplateID, BSTR szDoodadName, LONG dwDefaultReliveID)
{
	TCHAR * m_szDoodadName = (TCHAR *)_com_util::ConvertBSTRToString(szDoodadName);

	//以下为旧代码
	HRESULT hr = E_FAIL;

	KGLOG_PROCESS_ERROR(g_pSO3World);

	const KDoodadTemplate* pcDoodadTemplate = g_pSO3World->m_Settings.m_DoodadTemplateList.GetTemplate(nTemplateID);
	if (pcDoodadTemplate && pcDoodadTemplate->m_dwTemplateID != 0)
	{
		KRepresentDoodadResource DoodadRes;

		g_pRepresentHandler->GetDoodadResource(pcDoodadTemplate->m_dwRepresentID,&DoodadRes);

		LPCTSTR pMeshName = DoodadRes.aAnimation[0].szMeshFilePath;

		LPCTSTR pMtlName = DoodadRes.aAnimation[0].szMtlFilePath;
		LPCTSTR pAniName = DoodadRes.aAnimation[0].szAniFilePath;
		if(!pMeshName)
		{
			pMeshName = "data\\public\\scenerunner\\scenerunner.Mesh";
			pAniName  = "data\\public\\scenerunner\\scenerunner.Ani";
		}
		TCHAR szMeshName[MAX_PATH];
		TCHAR szMtlName[MAX_PATH];
		TCHAR szAniName[MAX_PATH];
		wsprintf(szMeshName,"%s",pMeshName);
		wsprintf(szMtlName,"%s" ,pMtlName);
		wsprintf(szAniName,"%s" ,pAniName);

		{
			IEKG3DRepresentObject* pObj = BuildRepresentDoodad(g_pEngineManager
				, nTemplateID, m_szDoodadName, szMeshName, szMtlName, pAniName,dwDefaultReliveID
				, true, pcDoodadTemplate->m_bDynamicObstacle);

			if(pObj)
			{
				hr = g_pSceneEditor->SetObjectInHandle(pObj, TRUE);//生命周期管理移交了
				if (FAILED(hr))
				{
					SAFE_RELEASE(pObj);
				}
			}
		}

	}

	return S_OK;
Exit0:
	return E_FAIL;
}


STDMETHODIMP CAtlBase::SwitchNpcBrush(LONG nTemplateID, BSTR szNpcName, LONG dwDefaultReliveID, LONG dwDefaultRandomID, LONG dwDefaultAISet)
{
	TCHAR * m_szNpcName = (TCHAR *)_com_util::ConvertBSTRToString(szNpcName);
	DWORD m_dwDefaultReliveID = dwDefaultReliveID;
	DWORD m_dwDefaultRandomID = dwDefaultRandomID;
	DWORD m_dwDefaultAISet = dwDefaultAISet;

	//以下为旧代码
	HRESULT hr = E_FAIL;
	KGLOG_PROCESS_ERROR(g_pSO3World);
	const KNpcTemplate* pcNpcTempate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(nTemplateID);
	if (pcNpcTempate == NULL)
		return E_FAIL;

	//const KAIParamTemplate* pcAITempate = g_pSO3World->m_Settings.m_AIParamTemplateList.GetTemplate(pcNpcTempate->nAIParamTemplateID);
	if (pcNpcTempate && pcNpcTempate->dwTemplateID != 0)
	{

		DWORD dwRepresentID = pcNpcTempate->dwRepresentID[0];

		KRepresentNPCEquipResource NPCRes;
		KRepresentNPCAnimationResource AniRes;

		
		g_pRepresentHandler->GetNPCEquipResource(dwRepresentID,&NPCRes);
		g_pRepresentHandler->GetNPCAnimationResource(dwRepresentID,30,&AniRes);

		LPCTSTR szMDLName = NPCRes.szMDLFilePath;
		LPCTSTR szAniName = AniRes.szAniFilePath;

		LPCTSTR szFaceMesh = NPCRes.Parts[RL_NPC_SOCKET_FACE].szMeshFilePath;
		LPCTSTR szFaceMtl = NPCRes.Parts[RL_NPC_SOCKET_FACE].szMtlFilePath;

// 		float fAlertRange  = (float)(pcAITempate->nAlertRange) * LOGICAL_CELL_CM_LENGTH / CELL_LENGTH;
// 		float fAttackRange = (float)(pcAITempate->nAttackRange) * LOGICAL_CELL_CM_LENGTH / CELL_LENGTH;
		float fAlertRange = 0.0f;
		float fAttackRange = 0.0f;

		TCHAR szMDLName_S[MAX_PATH];
		TCHAR szAniName_S[MAX_PATH];
		wsprintf(szMDLName_S,"%s",szMDLName);
		wsprintf(szAniName_S,"%s",szAniName);

		IEKG3DRepresentObject* pObj = BuildRepresentNPC(g_pEngineManager, nTemplateID, m_szNpcName, szMDLName_S
			, szAniName_S, szFaceMesh, szFaceMtl, pcNpcTempate->nLevel, pcNpcTempate->nKind/*, pcNpcTempate->nAIType*/
			, NPCRes.fModelScale, pcNpcTempate->dwRepresentID[0], fAlertRange, fAttackRange,m_dwDefaultReliveID,m_dwDefaultRandomID,m_dwDefaultAISet);
		if (pObj)
		{
			hr = g_pSceneEditor->SetObjectInHandle(pObj, TRUE);
			if (FAILED(hr))
			{
				SAFE_RELEASE(pObj);
			}
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

STDMETHODIMP CAtlBase::QuaternionNDirByRotation(FLOAT x, FLOAT y, FLOAT z, FLOAT w, LONG* nDir)
{	
	LONG angle = acos(w) * 2;
	*nDir = angle;

	return S_OK;
}
// end ahpho

STDMETHODIMP CAtlBase::SetShowProfile(BOOL bShow)
{	
	if (g_pEngineManager)
		g_pEngineManager->SetRenderOption(RENDER_INFO, bShow);
	return S_OK;
}
