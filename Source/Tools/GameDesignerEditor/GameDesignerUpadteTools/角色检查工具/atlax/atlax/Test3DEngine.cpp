// Test3DEngine.cpp : Implementation of CTest3DEngine

#include "stdafx.h"
#include "Test3DEngine.h"
#include "KG3DInterface.h"
#include "./3DEngine/SceneEditorCommon.h"
#include "IEKG3DScene.h"
#include "IEKG3DSceneOutputWnd.h"
#include "IEEditor.h"
#include "Functions.h"
#include "d3d9.h"
#include "d3dx9.h"
#include "File.h"
#include "IEKG3DModelTable.h"

#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "Engine.lib")

// CTest3DEngine

#define EDITOR_CONFIG_FILE "Config.ini"

#define GETEDITOR() \
	IEKG3DSceneSceneEditor* pEditor = NULL;\
	m_p3DScene->GetSceneSceneEditor(&pEditor);\
	_ASSERTE(pEditor); \
	if(!pEditor) return
#define GETNPC() \
	IEKG3DRepresentNPC* pNpc = NULL;\
	pEditor->GetRepresentNPC(&pNpc);\
	_ASSERTE(pNpc);\
	if (!pNpc) return

#define GETSCENEBASE() \
	IEKG3DSceneEditorBase* pBase = NULL;\
	pEditor->GetSceneEditorBase(&pBase); \
	_ASSERTE(pBase);\
	if (!pBase) return

#define GETSCENE() \
	IEKG3DScene* pScene = m_p3DScene;\
	_ASSERTE(pScene); \
	if(!pScene) return

#define GETSELECT() \
	IEKG3DSceneSelectBase* pSelector = NULL; \
	pEditor->GetSceneSelector(&pSelector); \
	_ASSERTE(pSelector); \
	if (!pSelector) \
	return

#define KG_COM_PROCESS_ERROR(Condition) \
	do  \
	{   \
	if (FAILED(Condition))  \
	goto Exit0;         \
	} while (false)

#define KG_PROCESS_ERROR(Condition) \
	do  \
	{   \
	if (!(Condition))   \
	goto Exit0;     \
	} while (false)

extern BOOL Init3DEngine(unsigned uMode, unsigned int uFlag, HWND hBaseWindow, HWND hRenderWindow);

//TCHAR* CTest3DEngine::m_strSockets[NUM_SOCKET] = { "s_hat", "s_spine2", "s_spine", "s_rp", "s_lp", "s_rc", "s_lc", "s_rh", "s_lh" };

STDMETHODIMP CTest3DEngine::Init(LONG hWnd)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//(by dengzhihui Refactor 2006年11月3日
	//开始初始化3D引擎

    //g_SetRootPath(TEXT("H:\\sWordIII\\blackhole\\Tools\\SceneEditor\\Debug\\"));
	TCHAR strRootPath[MAX_PATH];
	TCHAR strModuleFileName[MAX_PATH];
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	::GetModuleFileName(NULL, strModuleFileName, MAX_PATH);
	_splitpath_s(strModuleFileName, 
		strDriver,
		MAX_PATH,
		strPath,
		MAX_PATH,
		NULL,
		0,
		NULL,
		0);
	sprintf_s(strRootPath,
		MAX_PATH,
		"%s%s",
		strDriver,
		strPath);
	g_SetRootPath(strRootPath);

	m_hRenderWnd = (HWND)hWnd;
	if (!Init3DEngine(0, KG3DENG_NONE, m_hRenderWnd, m_hRenderWnd))
	{
		AfxMessageBox("Failed to Initalize Engine Manager\n");
		return E_FAIL;
	}

	IEKG3DScene* pScene = NULL;
	g_pEngineManager->NewOneScene(SCENETYPE_SCENEEDITOR,(IKG3DScene**)&pScene);
	if(!pScene)
	{
		MessageBox(NULL,"创建场景失败","失败",0);
		return false;
	}
	if ( FAILED( pScene->LoadFromFile("",0,0) ) )
	{
		SAFE_RELEASE( pScene );
		MessageBox(NULL,"创建场景失败","失败",0);
		return FALSE;
	}

	g_pEngineManager->SetFocusScene(pScene);//by dengzhihui Refactor 2006年11月6日
	m_p3DScene = pScene;

	m_nOutputWindowID = 0;
	m_p3DScene->AddOutputWindow("SceneMain",m_hRenderWnd,
		OUTPUTWND_SCENEEDITOR_PREVIEW,0,&m_nOutputWindowID);
	ASSERT( m_p3DScene->GetCurrentCamera() );

    m_pRepresentObject = NULL;

	IEKG3DSceneOutputWnd *piSceneCurOutputWnd = NULL;
	m_p3DScene->GetCurOutputWnd(&piSceneCurOutputWnd);
	if (piSceneCurOutputWnd)
		piSceneCurOutputWnd->InitCameraStation(EDITOR_CONFIG_FILE);

	GETEDITOR() E_FAIL;	GETSCENEBASE() E_FAIL;
	InitKeyState(pBase, m_hRenderWnd, &m_InputProxy);

	m_pSelectedObj = NULL;
	//m_ItemModel = NULL;
	return S_OK;
}


STDMETHODIMP CTest3DEngine::UnInit(void)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!m_p3DScene)
		return S_OK;
	//SAFE_RELEASE(m_pRepresentObject);
	SAFE_RELEASE(m_pSelectedObj);
//	SAFE_RELEASE(m_ItemModel);
	m_p3DScene->RemoveOutputWindow(m_nOutputWindowID);
	g_pEngineManager->RemoveScene(m_p3DScene);
	g_pEngineManager->UnInit();
	m_p3DScene = NULL;
	m_nOutputWindowID = -1;
	extern void UnInit3DEngine();
	UnInit3DEngine();


	return S_OK;
}


STDMETHODIMP CTest3DEngine::Render(void)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (g_pEngineManager)
		g_pEngineManager->Render();
	return S_OK;
}

STDMETHODIMP CTest3DEngine::FrameMove(void)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (g_pEngineManager)
		g_pEngineManager->FrameMove();
	return S_OK;
}

STDMETHODIMP CTest3DEngine::OnResizeWindow(void)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_p3DScene)
		m_p3DScene->OnResizeWindow(NULL);

	return S_OK;
}

STDMETHODIMP CTest3DEngine::get_PlayMode(VARIANT_BOOL* pVal)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());

	GETEDITOR() E_FAIL; GETSCENEBASE() E_FAIL;
	*pVal = !pBase->GetEditing();

	return S_OK;
}

STDMETHODIMP CTest3DEngine::put_PlayMode(VARIANT_BOOL newVal)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());

	GETEDITOR() E_FAIL; GETSCENEBASE() E_FAIL;
	pBase->SetEditing(!newVal);

	return S_OK;
}

STDMETHODIMP CTest3DEngine::OnWndMsg(PMSG WndMsg, INT* pnRet)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	extern BYTE* g_SceneEditorKeyState;

	PMSG pMsg = (PMSG)WndMsg;

	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
		g_SceneEditorKeyState[pMsg->wParam] = !!(GetAsyncKeyState(INT(pMsg->wParam)) & 0x8000);
	if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST && pMsg->message != WM_MOUSEMOVE || pMsg->message == WM_COMMAND)
	{	
		g_SceneEditorKeyState[VK_LBUTTON] = !!(GetAsyncKeyState(VK_LBUTTON) & 0x8000);
		g_SceneEditorKeyState[VK_MBUTTON] = !!(GetAsyncKeyState(VK_MBUTTON) & 0x8000);
		g_SceneEditorKeyState[VK_RBUTTON] = !!(GetAsyncKeyState(VK_RBUTTON) & 0x8000);
	}

	*pnRet = m_InputProxy.CheckInputEvent(pMsg);

	return S_OK;
}

STDMETHODIMP CTest3DEngine::LoadFile(BSTR strFileName)
{
	USES_CONVERSION;
	LoadPlayerModel(A2W("E:\\work\\Sword3\\blackhole\\tools\\sceneeditor\\debug\\Data\\public\\PlayerCheckTool\\Checker.mdl"));
	LoadAni(A2W("E:\\work\\Sword3\\blackhole\\tools\\sceneeditor\\debug\\Data\\public\\PlayerCheckTool\\Checker_skin.ani"));
	BindToPlayer(strFileName, 0, A2W("Bip01 Head"));
	/*IKG3DModel* pModel = NULL;
	USES_CONVERSION;

    GETEDITOR() E_FAIL;

    if (FAILED(g_pEngineManager->NewKG3DRepresentObjectInTable(&m_pRepresentObject)))
        return E_FAIL;

    m_pRepresentObject->SetModel(0, W2A(strFileName), NULL, NULL);
    pEditor->SetObjectInHandle(m_pRepresentObject);*/
    
/*
	pPC->Unbind(BoneName);
	m_pRepresentObject->GetModel(&pModel, 0);
	pModel->BindToBone(pPC, BoneName);
*/
	return S_OK;
}

STDMETHODIMP CTest3DEngine::LoadAni(BSTR strFileName)
{
    USES_CONVERSION;
    
    if (m_pRepresentObject)
    {
        IKG3DModel* pModel = NULL;
        if (SUCCEEDED(m_pRepresentObject->GetModel(pModel, 0)))
        {            
			HRESULT hr = pModel->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE,  W2A(strFileName), 1.0f, 0.0f, NULL);
			if (hr == E_FAIL)
			{
				CString str;
				str.Format("当前所选的动作文件不可用");
				MessageBoxA(NULL,str,"提示",MB_ICONINFORMATION);
			}
			
        }
		
    }

    return S_OK;
}

STDMETHODIMP CTest3DEngine::UnloadFile(BSTR strFileName)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//USES_CONVERSION;
	//AfxMessageBox(W2A(strFileName));

	return S_OK;
}

STDMETHODIMP CTest3DEngine::LoadPlayerModel(BSTR strFileName)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	GETEDITOR() E_FAIL;

	if (FAILED(g_pEngineManager->NewKG3DRepresentObjectInTable(&m_pRepresentObject)))
		return E_FAIL;

	m_pRepresentObject->SetModel(0, W2A(strFileName), NULL, NULL);
	pEditor->SetObjectInHandle(m_pRepresentObject,TRUE);
	return S_OK;
}

STDMETHODIMP CTest3DEngine::BindToPlayer(BSTR strFileName, DWORD dwType, BSTR strBindToObj)
{
	TCHAR strExt[MAX_PATH];
	USES_CONVERSION;
	_splitpath_s(W2A(strFileName), 
		NULL,
		0,
		NULL,
		0,
		NULL,
		0,
		strExt,
		MAX_PATH);
	_strlwr_s(strExt, MAX_PATH);
	if (!strcmp(strExt, ".bind"))
	{
		IEKG3DModelTable* pModelTable = NULL;
		IKG3DModel* pPlayerModel = NULL;
		HRESULT hr = g_pEngineManager->GetIEKG3DModelTable(&pModelTable);
		ASSERT(pModelTable);
		SAFE_RELEASE(m_pSelectedObj);
		pModelTable->IELoadResourceFromFile(W2A(strFileName),
			0,
			0,
			(IKG3DResourceBase**)&m_pSelectedObj);
		m_pRepresentObject->GetModel(pPlayerModel, 0);
		ASSERT(pPlayerModel);
		m_pSelectedObj->BindToBone(pPlayerModel, "");
	}
	else if (!strcmp(strExt, ".sfx") ||
		!strcmp(strExt, ".mdl") ||
		!strcmp(strExt, ".mesh"))
	{
		IEKG3DModelTable* pModelTable = NULL;
		IKG3DModel* pPlayerModel = NULL;
		HRESULT hr = g_pEngineManager->GetIEKG3DModelTable(&pModelTable);
		ASSERT(pModelTable);
		SAFE_RELEASE(m_pSelectedObj);
		pModelTable->IELoadResourceFromFile(W2A(strFileName),
			0,
			0,
			(IKG3DResourceBase**)&m_pSelectedObj);
		switch(dwType)
		{
		case 0://Bind to bone
			m_pRepresentObject->GetModel(pPlayerModel, 0);
			ASSERT(pPlayerModel);
			((IEKG3DModel*)pPlayerModel)->UnbindChild(W2A(strBindToObj));
			m_pSelectedObj->BindToBone(pPlayerModel, W2A(strBindToObj));
			break;
		case 1://Bind to socket
			m_pRepresentObject->GetModel(pPlayerModel, 0);
			ASSERT(pPlayerModel);
			((IEKG3DModel*)pPlayerModel)->UnbindChild(W2A(strBindToObj));
			m_pSelectedObj->BindToSocket(pPlayerModel, W2A(strBindToObj));
			break;
		}
	}
	
	return S_OK;
}

STDMETHODIMP CTest3DEngine::ZoomView(LONG lDelta)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());

	GETEDITOR() E_FAIL; GETSCENEBASE() E_FAIL;
	POINT pt;
	pt.x = pt.y = 0;

	pBase->ZoomView(lDelta, pt);
	return S_OK;
}

STDMETHODIMP CTest3DEngine::ChangeModelPart(BSTR filename, INT PartID)
{
	HRESULT hr = E_FAIL;
	IKG3DModel* pModel = NULL;
	IKG3DModel* pPlayerModel = NULL;
	IEKG3DModelTable* piModelTable = NULL;
	USES_CONVERSION;
	KG_PROCESS_ERROR(PartID != CB_ERR);

	hr = g_pEngineManager->GetIEKG3DModelTable(&piModelTable);
	
	hr = piModelTable->IELoadResourceFromFile(W2A(filename), 
		0,
		0,
		(IKG3DResourceBase**)&pModel);
	KG_COM_PROCESS_ERROR(hr);
	
	m_pRepresentObject->GetModel(pPlayerModel, 0);
	ASSERT(pPlayerModel);
	hr = pPlayerModel->ChangeModel(PartID, pModel);
	KG_COM_PROCESS_ERROR(hr);
	return S_OK;
Exit0:
	return E_FAIL;
	
}

STDMETHODIMP CTest3DEngine::LoadFace(BSTR strfilename)
{
	HRESULT hr = E_FAIL;
	SAFE_RELEASE(m_pSelectedObj);
	IEKG3DModelTable* piModelTable = NULL;
	IKG3DModel* pPlayerModel = NULL;
	USES_CONVERSION;
	hr = g_pEngineManager->GetIEKG3DModelTable(&piModelTable);
	
	hr = piModelTable->IELoadResourceFromFile(W2A(strfilename),
		0,
		0,
		(IKG3DResourceBase**)&m_pSelectedObj);
	KG_COM_PROCESS_ERROR(hr);
	m_pRepresentObject->GetModel(pPlayerModel, 0);
	ASSERT(pPlayerModel);
	m_pSelectedObj->BindToSocket(pPlayerModel, "s_face");
	return S_OK;
Exit0:
	return E_FAIL;

}

STDMETHODIMP CTest3DEngine::LoadSocket(BSTR strBindToObj, BSTR strFileName)
{
	HRESULT hr = E_FAIL;
	IKG3DModel* pPlayerModel = NULL;
	IKG3DModel* pItemModel = NULL;
	IEKG3DModelTable* piModelTable = NULL;
	USES_CONVERSION;
	//SAFE_RELEASE(m_ItemModel);
	hr = g_pEngineManager->GetIEKG3DModelTable(&piModelTable);
	//KGLOG_COM_PROCESS_ERROR(hr);

	hr = piModelTable->IELoadResourceFromFile(W2A(strFileName),
		0,
		0,
		(IKG3DResourceBase**)&pItemModel);
	KG_COM_PROCESS_ERROR(hr);
	m_pRepresentObject->GetModel(pPlayerModel, 0);
	ASSERT(pPlayerModel);

	pItemModel->BindToSocket(pPlayerModel, W2A(strBindToObj));
	m_ItemModels.push_back(pItemModel);
	return S_OK;
Exit0:
	return E_FAIL;
}

STDMETHODIMP CTest3DEngine::ItemsClear()
{
	for (size_t i = 0; i < m_ItemModels.size(); i++)
	{
		SAFE_RELEASE(m_ItemModels[i]);
	}
	return S_OK;
}

STDMETHODIMP CTest3DEngine::SaveMDLToFile( BSTR strFileName)
{
	HRESULT hr = E_FAIL;
	TCHAR strOutputInfo[MAX_PATH << 1];
	IKG3DModel* pPlayerModel = NULL;
	IEKG3DModelTable* piModelTable = NULL;
	USES_CONVERSION;
	hr = g_pEngineManager->GetIEKG3DModelTable(&piModelTable);

	m_pRepresentObject->GetModel(pPlayerModel, 0);
	ASSERT(pPlayerModel);

	hr = pPlayerModel->SaveToFile(W2A(strFileName), 0);	
	if (SUCCEEDED(hr))
	{
		sprintf_s(strOutputInfo, MAX_PATH << 1, "保存%s成功.请不要移动MDL文件的位置.", strFileName);
		MessageBoxA(NULL,strOutputInfo,"提示", MB_ICONINFORMATION);
	}
	else
	{
		sprintf_s(strOutputInfo, MAX_PATH << 1, "保存%s失败.", strFileName);
		MessageBoxA(NULL,strOutputInfo,"提示", MB_ICONINFORMATION);
	}
	return hr;
}