// Test3DEngine.cpp : Implementation of CTest3DEngine

#include "stdafx.h"
#include "Test3DEngine.h"
#include "KG3DInterface.h"
#include "./3DEngine/SceneEditorCommon.h"
#include "IEKG3DScene.h"
#include "IEKG3DSceneOutputWnd.h"
#include "IEKG3DTerrain.h"
#include "IEEditor.h"
#include "d3d9.h"
#include "d3dx9.h"
#include "File.h"
#include "IEKG3DModelTable.h"
//#include "comutil.h"
#include "IEKG3DTextureTable.h"
#include "IEKG3DSFX.h"
#include "KG3DAnimationStruct.h"
#include "KG3DTextureTable.h"
#include "./3DEngine/KSceneEditorDocLogical.h"
#include "KSO3World.h"
#include "Functions.h"
#include "ProgressForm.h"
#include "KG3DSFX.h"

#include "KG3DRepresentNPC.h"
//#include "FileType.h"
#include "KG3DModelTable.h"
#include "KG3DClip.h"
#include "KG3DGraphicsTool.h"
#include "KG3DScene.h"

// 选择editor
//#define USE_SCENE_EDITOR
#ifndef USE_SCENE_EDITOR
#define USE_MODEL_EDITOR
#endif

//TCHAR* CTest3DEngine::m_strSockets[NUM_SOCKET] = { "s_hat", "s_spine2", "s_spine", "s_rp", "s_lp", "s_rc", "s_lc", "s_rh", "s_lh" };
extern TCHAR g_EnginePath[MAX_PATH];
extern BOOL Init3DEngine(unsigned uMode, unsigned int uFlag, HWND hBaseWindow, HWND hRenderWindow, BSTR strEnginePath, BSTR strStartUpPath);
extern void UnInit3DEngine();
extern int ShowProgress(LPCTSTR pName, float fPencent);
void BeforeChange(KSceneEditorDocLogical* m_lpLogicalData);
void ExchangeLogicalData(LPCTSTR pFileName, KScene* lpLogicalScene, KSceneEditorDocLogical* m_lpLogicalData);
void ExchangeNpc(KScene *pLogicalScene);
CProgressForm g_ProgressDlg;
CStatic g_ProgressName;
CProgressCtrl g_ProgressProgress;
BOOL g_bProgressInited = FALSE;
BOOL g_bEngineEnable = TRUE;

STDMETHODIMP CTest3DEngine::ShowProgressForm(BOOL bShow)
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

STDMETHODIMP CTest3DEngine::SaveLogicScene(BSTR iniFile, LONG ptr3DScene) // iniFile 是 *.Map.Logical 之类的文件
{
	IEKG3DScene* pScene = (IEKG3DScene*)ptr3DScene;

	HRESULT hrRetCode = NULL;
	STRING strMap(BSTR_TO_STRING(iniFile));
	strMap = strMap.substr(0, strMap.length() - 8);

	// init
	if (!g_bSO3LogicInited)
	{
		IRecorderFactory* piRecorderFactory = NULL;
		piRecorderFactory = CreateRecorderFactoryInterface(ermNormal);

		g_pSO3World = new KSO3World;
		KGLOG_PROCESS_ERROR(g_pSO3World);

		BOOL bRetCode = g_pSO3World->Init(piRecorderFactory);
		KGLOG_PROCESS_ERROR(bRetCode);

		if (InitRepresent() && InitUI())
		{
			HWND hWnd = NULL;
			g_SetGameWorldUIHandler(&g_pUI->GetGameWorldUIHandler());
			g_SetGameWorldRepresentHandler(g_pRepresent->GetGameWorldRepresentHandler());
			g_pRepresentHandler = g_pRepresent->GetRepresentHandelr();

			g_pRepresent->GetRepresentHandelr()->AttachGameWorld(g_pSO3World, QuerySO3WorldClientInterface());
			g_pRepresent->GetRepresentHandelr()->Attach3DEngine(g_GetEngineIEInterface(), g_pEngineManager->Get3DModelTable(), g_pEngineManager->Get3DUI());
			g_pRepresent->GetRepresentHandelr()->AttachUI(g_pUI, &g_pUI->GetGameWorldUIHandler());

			g_pEngineManager->GetBaseWnd(&hWnd);
			g_pRepresent->GetRepresentHandelr()->InitHWND(hWnd);
			if (!g_pRepresent->Init())
			{
				::MessageBox(NULL, "表现逻辑初始化失败！", NULL, 0);
			}
			else
			{
				g_bSO3LogicInited = TRUE;
			}
		}
		else
		{
			::MessageBox(NULL, "表现逻辑初始化失败！", NULL, 0);
		}
	}

	// load
	KScene* lpLogicalScene = g_pSO3World->LoadEditorScene((char*)strMap.c_str(), &ShowProgress);
	if (lpLogicalScene != NULL)
	{
		KSceneEditorDocLogical* m_lpLogicalData = new KSceneEditorDocLogical;
		IEKG3DSceneSceneEditor* pEditor = NULL;
		hrRetCode = pScene->GetSceneSceneEditor(&pEditor);		
		m_lpLogicalData->SetSceneEditor(pEditor);
		m_lpLogicalData->LoadFromIniFile(BSTR_TO_STRING(iniFile), &ShowProgress, FALSE);
		
		// save
		ExchangeLogicalData(strMap.c_str(), lpLogicalScene, m_lpLogicalData);
	}

Exit0:
	return S_OK;
}

STDMETHODIMP CTest3DEngine::Init(LONG hwndBase, BSTR strEnginePath, BSTR strStartUpPath)
{
	USES_CONVERSION;
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (g_bKG3DEngineInited)
		return S_OK;
	g_bKG3DEngineInited = TRUE;
	m_pLastModel = NULL;

// 	//(by dengzhihui Refactor 2006年11月3日
// 	//开始初始化3D引擎
// 	TCHAR strRootPath[MAX_PATH];
// 	TCHAR strModuleFileName[MAX_PATH];
// 	TCHAR strDriver[MAX_PATH];
// 	TCHAR strPath[MAX_PATH];
// 	::GetModuleFileName(NULL, strModuleFileName, MAX_PATH);
// 	_splitpath_s(strModuleFileName, 
// 		strDriver,
// 		MAX_PATH,
// 		strPath,
// 		MAX_PATH,
// 		NULL,
// 		0,
// 		NULL,
// 		0);
// 	sprintf_s(strRootPath,
// 		MAX_PATH,
// 		"%s%s",
// 		strDriver,
// 		strPath);



	m_hRenderWndBase = (HWND)hwndBase;
	if (!Init3DEngine(0, KG3DENG_DESIGN, m_hRenderWndBase, m_hRenderWndBase, strEnginePath, strStartUpPath))
	{
 		AfxMessageBox("Failed to Initalize Engine Manager\n");
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CTest3DEngine::FileToImage(BSTR src3DFileName, BSTR desImageFileName) // 文件名不带后缀，如果是动作的也不带001，002之类。
{
	USES_CONVERSION;	
	HRESULT hr = E_FAIL;

	DWORD width = 256;
	DWORD height = 256;
	DWORD bgcolor = 0x00ffffff;

	// 老的meshfile
	TCHAR szOldMesh[MAX_PATH];
	TCHAR szOldMtl[MAX_PATH];
	TCHAR szOldAni[MAX_PATH];
	m_RepresentObjects[m_index]->GetModelFileName(0, szOldMesh, szOldMtl, szOldAni);

	STRING str(BSTR_TO_STRING(src3DFileName));
	std::transform(str.begin(), str.end(), str.begin(), std::tolower);

	if (str.find(TEXT(".mdl")) != string::npos || str.find(TEXT(".mesh")) != string::npos)
	{
		LoadPlayerModel(src3DFileName);
		ToggleRotateModelEditor(); // 不旋转

		IKG3DModel* pPlayerModel = NULL;
		m_RepresentObjects[m_index]->GetModel(pPlayerModel, 0);
		if (!pPlayerModel)
		{
			STRING strErr(TEXT("【From atlax:】 该文件无法加载："));
			strErr += BSTR_TO_STRING(src3DFileName);
			strErr += TEXT("。\r\n");
			OutputDebugString(strErr.c_str());
			return S_OK;
		}
		IEKG3DModel* pModel = (IEKG3DModel*)pPlayerModel;

		STRING strOutputFile(BSTR_TO_STRING(desImageFileName));
		strOutputFile += TEXT("001.jpg");
		//strOutputFile += TEXT(".jpg");
		hr = pModel->RenderToPic(strOutputFile.c_str(), width, height, bgcolor);
	} 
	else if (str.find(TEXT(".ani")) != string::npos || str.find(TEXT(".tani")) != string::npos)
	{
		str = str.substr(0, str.find_last_of("\\/") + 1);
		CFileFind file;
		BOOL bContinue = file.FindFile(((STRING)(str + "*.mdl")).c_str()); // 就在同一个目录下的mdl

		if (!bContinue) // 父目录下的“模型”文件夹
		{
			str = str.substr(0, str.length() - 1);
			str = str.substr(0, str.find_last_of("\\/") + 1) + TEXT("模型\\");
			bContinue = file.FindFile(((STRING)(str + "*.mdl")).c_str());
		}
		if (!bContinue) // 父目录下的“部件/Mdl”文件夹（player）
		{
			str = str.substr(0, str.length() - 1);
			str = str.substr(0, str.find_last_of("\\/") + 1) + TEXT("部件\\Mdl\\");
			bContinue = file.FindFile(((STRING)(str + "*.mdl")).c_str());
		}
		if (bContinue)
		{
			file.FindNextFile();
			CString filename = file.GetFilePath();
			LoadPlayerModel(STRING_TO_BSTR(filename));
			ToggleRotateModelEditor(); // 不旋转

			IKG3DModel* pPlayerModel = NULL;
			m_RepresentObjects[m_index]->GetModel(pPlayerModel, 0);
			if (!pPlayerModel) return hr;
			IEKG3DModel* pModel = (IEKG3DModel*)pPlayerModel;

			hr = pModel->RenderAnimationToPicSerial(BSTR_TO_STRING(src3DFileName), BSTR_TO_STRING(desImageFileName),
				width, height, bgcolor);
		}
	}
	
	LoadPlayerModel(STRING_TO_BSTR(szOldMesh));
// 	if (hr != S_OK)
// 	{
// 		MessageBox(NULL, BSTR_TO_STRING(src3DFileName), "以下模型 / 动作可能有问题。", 0);
// 		return S_OK;
// 	}
	return S_OK;
}

STDMETHODIMP CTest3DEngine::SaveMdlWithAniToImages(LONG ptrModel, BSTR fileName)
{
	USES_CONVERSION;
	

// 	IKG3DModel* pPlayerModel = NULL;
// 	m_RepresentObjects[m_index]->GetModel(pPlayerModel, 0);
// 
// 	if (pPlayerModel)
// 	{
// 		IEKG3DModel* pModel = (IEKG3DModel*)pPlayerModel;
// 		hr = pModel->RenderAnimationToPicSerial()
// 			RenderToPic(BSTR_TO_STRING(fileName), 300, 300, 0x00ffffff);
// 	}
// 
// 	IEKG3DModel* pModel = (IEKG3DModel*)ptrModel;
// 	IKG3DAnimationController* pController = NULL;
// 	pModel->IEGetAnimationController(&pController);
// 
// 	for (int i = 0; i < 10; i++)
// 	{
// 		pController->SeekAnimation(ENU_ANI_SEEK_SET | ENU_ANI_SEEK_PERCENTAGE, i * 0.1f);
// 		pModel->FrameMove();
// 
// 		STRING str(BSTR_TO_STRING(fileName));
// 		if (i < 9)
// 			str += "00";
// 		else
// 			str += "0";
// 
// 		TCHAR szBuf[10];
// 		_itot(i + 1, szBuf, 10);
// 
// 		str += szBuf;
// 		str += TEXT(".jpg");
// 
// 		SaveModelToImageFile(STRING_TO_BSTR(str.c_str()));		
// 	}

	return S_OK;
}

STDMETHODIMP CTest3DEngine::SaveModelToImageFile(BSTR fileName)
{
// 	USES_CONVERSION;
 	HRESULT hr = E_FAIL;
// 
// 	IKG3DModel* pPlayerModel = NULL;
// 	m_RepresentObjects[m_index]->GetModel(pPlayerModel, 0);
// 	
// 	if (pPlayerModel)
// 	{
// 		IEKG3DModel* pModel = (IEKG3DModel*)pPlayerModel;
// 		hr = pModel->RenderToPic(BSTR_TO_STRING(fileName), 300, 300, 0x00ffffff);
// 	}
// 
// Exit0:
	return hr;
}

STDMETHODIMP CTest3DEngine::ActivateScene(BSTR editor_name)
{
	std::string strEditorName(BSTR_TO_STRING(editor_name));
	std::map<std::string, INT>::iterator Iter = m_NameHash.find(strEditorName);
	if (Iter == m_NameHash.end())
		return S_OK;

	INT index = Iter->second;
	m_index = index;

	if (index >= 0 && index < (int)m_p3DScenesVec.size())
	{
		m_p3DScenesVec[index]->SetCurrentOutputWindow(m_OutputWindowIDs[index]);
	}

	return S_OK;
}

STDMETHODIMP CTest3DEngine::GetCurSceneIndex(LONG* index)
{
	*index = m_index;
	return S_OK;
}

STDMETHODIMP CTest3DEngine::ActivateSceneByIndex(LONG index)
{
	m_index = index;
	if (index >= 0 && index < (int)m_p3DScenesVec.size())
		m_p3DScenesVec[index]->SetCurrentOutputWindow(m_OutputWindowIDs[index]);
	return S_OK;
}

STDMETHODIMP CTest3DEngine::GetEditorSceneCount(BSTR editor_name, LONG* count)
{
	std::string strEditorName(BSTR_TO_STRING(editor_name));
	std::map<std::string, INT>::iterator Iter = m_NameHash.find(strEditorName);
	if (Iter == m_NameHash.end())
		*count = 0;
	else
		*count = 1;

	return S_OK;
}

STDMETHODIMP CTest3DEngine::RemoveScene(BSTR editor_name)
{
	std::string strEditorName(BSTR_TO_STRING(editor_name));
	std::map<std::string, INT>::iterator Iter = m_NameHash.find(strEditorName);
	if (Iter == m_NameHash.end())
		return S_OK;

	INT index = Iter->second;

	if (index >= 0 && index < (int)m_p3DScenesVec.size())
	{
		m_p3DScenesVec[index]->RemoveOutputWindow(m_OutputWindowIDs[index]);
		g_pEngineManager->RemoveScene(m_p3DScenesVec[index]);
		m_p3DScenesVec[index] = NULL;
		m_NameHash.erase(Iter);
	}

	return S_OK;
}

STDMETHODIMP CTest3DEngine::AdjustCameraOrthogonal(LONG iOffset)
{
	IEKG3DSceneOutputWnd *piSceneCurOutputWnd = NULL;
	m_p3DScenesVec[m_index]->GetCurOutputWnd(&piSceneCurOutputWnd);
	if (piSceneCurOutputWnd)
	{
		IEKG3DCamera* pCamera = piSceneCurOutputWnd->GetIECamera();
		FLOAT fWidth, fHeight, fzNear, fzFar;
		pCamera->GetOrthogonal(&fWidth, &fHeight, &fzNear, &fzFar);
		fWidth += iOffset;
		fHeight += iOffset;
		pCamera->SetOrthogonal(fWidth, fHeight, fzNear, fzFar);

		if (iOffset > 0)
		{
			D3DXVECTOR3 vPos;
			pCamera->GetPosition(&vPos);
			vPos.x -= iOffset;
			vPos.z -= iOffset;
			pCamera->SetPosition(vPos);
		}
	}
	return S_OK;
}
STDMETHODIMP CTest3DEngine::SetCameraInfo(LONG pX, LONG pY, LONG pZ, LONG lX, LONG lY, LONG lZ, LONG width, LONG height)
{
	IEKG3DSceneOutputWnd *piSceneCurOutputWnd = NULL;
	m_p3DScenesVec[m_index]->GetCurOutputWnd(&piSceneCurOutputWnd);
	if (piSceneCurOutputWnd)
	{
		IEKG3DCamera* pCamera = piSceneCurOutputWnd->GetIECamera();
		D3DXVECTOR3 vPos(pX, pY, pZ);
		pCamera->SetPosition(vPos);
		D3DXVECTOR3 vLookat(lX, lY, lZ);
		pCamera->SetLookAtPosition(vLookat);
		FLOAT fWidth, fHeight, fzNear, fzFar;
		pCamera->GetOrthogonal(&fWidth, &fHeight, &fzNear, &fzFar);
		pCamera->SetOrthogonal(width, height, fzNear, fzFar);
	}
	return S_OK;
}
STDMETHODIMP CTest3DEngine::GetCameraInfo(LONG* pX, LONG* pY, LONG* pZ, LONG* lX, LONG* lY, LONG* lZ, LONG* width, LONG* height)
{
	IEKG3DSceneOutputWnd *piSceneCurOutputWnd = NULL;
	m_p3DScenesVec[m_index]->GetCurOutputWnd(&piSceneCurOutputWnd);
	if (piSceneCurOutputWnd)
	{
		IEKG3DCamera* pCamera = piSceneCurOutputWnd->GetIECamera();
		D3DXVECTOR3 vPos;
		pCamera->GetPosition(&vPos);
		D3DXVECTOR3 vLookat;
		pCamera->GetLookAtPosition(&vLookat);
		FLOAT fWidth, fHeight, fzNear, fzFar;
		pCamera->GetOrthogonal(&fWidth, &fHeight, &fzNear, &fzFar);
		*pX = vPos.x; *pY = vPos.y; *pZ = vPos.z;
		*lX = vLookat.x; *lY = vLookat.y; *lZ = vLookat.z;
		*width = fWidth; *height = fHeight;
	}
	return S_OK;
}

STDMETHODIMP CTest3DEngine::LoadOneMap_NoOutputWindow(BSTR filename, LONG* ptrScene)
{
	IEKG3DScene* pScene = NULL;
	g_pEngineManager->NewOneScene(SCENETYPE_SCENEEDITOR, (IKG3DScene**)&pScene);
	if (!pScene)
	{
		MessageBox(NULL, "创建场景失败", "失败", 0);
		return E_FAIL;
	}
	if ( FAILED( pScene->LoadFromFile(BSTR_TO_STRING(filename), 0, 0) ) )
	{
		SAFE_RELEASE(pScene);
		MessageBox(NULL, "创建场景失败", "失败", 0);
		return E_FAIL;
	}

	*ptrScene = (LONG)pScene;

	return S_OK;
}

STDMETHODIMP CTest3DEngine::GetHeight(LONG ptrScene, FLOAT x, FLOAT z, FLOAT* y)
{
	IEKG3DScene* pScene = (IEKG3DScene*)ptrScene;
	D3DXVECTOR3 vec(x, 0.0f, z);
	pScene->GetHeight(&vec, &vec);
	*y = vec.y;

	return S_OK;
}

STDMETHODIMP CTest3DEngine::ReadMapTerrainInfo(LONG ptrScene, LONG* width, LONG* height)
{
	IEKG3DScene* pScene = (IEKG3DScene*)ptrScene;

	INT xstart, ystart;
	pScene->GetLogicalSceneSize(&xstart, &ystart, (int*)width, (int*)height);

// 	IEKG3DSceneSceneEditor* pEditor = NULL;
// 	pScene->GetSceneSceneEditor(&pEditor);
// 
// 	IEKG3DTerrain* pTerrain = NULL;
// 	pEditor->GetTerrain(&pTerrain);
// 
// 	*width = (LONG)(pTerrain->GetBBoxLength() / 1600);
// 	*height = (LONG)(pTerrain->GetBBoxWidth() / 1600);

	return S_OK;
}

STDMETHODIMP CTest3DEngine::GetEditorSceneSize(LONG ptrScene, LONG* width, LONG* height)
{
	IEKG3DScene* pScene = (IEKG3DScene*)ptrScene;
	IEKG3DSceneSceneEditor* pSceneEditor = dynamic_cast<IEKG3DSceneSceneEditor*>(pScene);
	if (pSceneEditor)
		pSceneEditor->GetSceneRect(width, height);
	return S_OK;
}
													  /*例如 "npc编辑器[1]" 代表npc编辑器第一个分页的预览*/
STDMETHODIMP CTest3DEngine::AddScene(LONG hwndOutput, BSTR editor_name, LONG iOption)
{
	INT iSceneIndex = (INT)(m_p3DScenesVec.size());
	std::string strEditorName(BSTR_TO_STRING(editor_name));
	std::map<std::string, INT>::iterator Iter = m_NameHash.find(strEditorName);
	if (Iter == m_NameHash.end())
	{
		m_NameHash.insert(std::map<std::string, INT>::value_type(strEditorName, iSceneIndex));
	}

	IEKG3DScene* pScene = NULL;
	g_pEngineManager->NewOneScene(
#ifdef USE_SCENE_EDITOR
		SCENETYPE_SCENEEDITOR
#else
		SCENETYPE_MODELEDITOR
#endif
		, (IKG3DScene**)&pScene);
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

	m_p3DScenesVec.push_back(pScene);
	g_pEngineManager->SetFocusScene(pScene); // by dengzhihui Refactor 2006年11月6日

	INT windowid = 0;
	ULONG dwOutputWndType;
#ifdef USE_SCENE_EDITOR
	dwOutputWndType = OUTPUTWND_SCENEEDITOR_PREVIEW;
#else
	if (iOption == 0)
		dwOutputWndType = OUTPUTWND_MODELEDITOR_MAIN; // OUTPUTWND_MODELEDITOR_MAIN
	else
		dwOutputWndType = OUTPUTWND_MODELEDITOR_PREVIEW_NPC_PORTRAIT;
#endif
	pScene->AddOutputWindow("SceneMain", (HWND)hwndOutput, dwOutputWndType, 0, &windowid);
		
	m_OutputWindowIDs.push_back(windowid);
	pScene->SetCurrentOutputWindow(windowid);
	ASSERT( pScene->GetCurrentCamera() );

#ifdef USE_MODEL_EDITOR
	IKG3DCamera* pCamera = pScene->GetCurrentCamera();
	D3DXVECTOR3 vCameraPos(0.f, 250.f, -500.f);
	pCamera->SetPosition(vCameraPos);
#endif

	m_RepresentObjects.push_back(NULL);
	m_RepresentObjectSelectors.push_back(NULL);

	IEKG3DSceneOutputWnd *piSceneCurOutputWnd = NULL;
	pScene->GetCurOutputWnd(&piSceneCurOutputWnd);
	if (piSceneCurOutputWnd)
		piSceneCurOutputWnd->InitCameraStation(EDITOR_CONFIG_FILE);

	ActivateScene(editor_name);

#ifdef USE_SCENE_EDITOR
 	GETEDITOR() E_FAIL;	GETSCENEBASE() E_FAIL;
	KG3DTERRAIN_LOAD_PARAM param;
	param.nStart_X = param.nStart_Y = 0;
	param.nHeight = param.nWidth = 1;
	pEditor->LoadDefaultTerrain(&param);
#else
	GETMODELEDITOR() E_FAIL; GETSCENEBASE_MODEL() E_FAIL;
#endif

	CInputProxy* inputProxy = new CInputProxy();
	m_InputProxies.push_back(inputProxy);
 	InitKeyState(pBase, (HWND)hwndOutput, m_InputProxies[m_InputProxies.size() - 1]);

	m_pSelectedObj = NULL;

	return S_OK;
}

STDMETHODIMP CTest3DEngine::ToggleRotateModelEditor(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#ifdef USE_SCENE_EDITOR
	GETEDITOR() E_FAIL; GETSCENEBASE() E_FAIL; 
	pBase->SetEditing(!pBase->GetEditing());
#else
	GETMODELEDITOR() E_FAIL; GETSCENEBASE_MODEL() E_FAIL; 
	pBase->SetEditing(!pBase->GetEditing());
#endif

	return S_OK;
}

STDMETHODIMP CTest3DEngine::UnInit(void)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
// 	if (!m_p3DScenesVec[m_index])
// 		return S_OK;
	//SAFE_RELEASE(m_pRepresentObject);
	SAFE_RELEASE(m_pSelectedObj);
//	SAFE_RELEASE(m_ItemModel);

// 	m_pActive3DScene->RemoveOutputWindow(m_nOutputWindowID);
// 	g_pEngineManager->RemoveScene(m_pActive3DScene);
// 	m_nOutputWindowID = -1;
// 	for (size_t i = 0; i < m_p3DScenesVec.size(); i++)
// 	{
// 		m_p3DScenesVec[i]->RemoveOutputWindow(m_OutputWindowIDs[i]);
// 		g_pEngineManager->RemoveScene(m_p3DScenesVec[i]);
// 	}

 	g_pEngineManager->UnInit();
 	//m_pActive3DScene = NULL;
 	//m_p3DScenesVec.clear();

 	UnInit3DEngine();
	return S_OK;
}


STDMETHODIMP CTest3DEngine::Render(void)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (g_pEngineManager && g_bEngineEnable)
	    g_pEngineManager->Render();
	return S_OK;
}
STDMETHODIMP CTest3DEngine::FrameMove(void)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (g_pEngineManager && g_bEngineEnable)
	    g_pEngineManager->FrameMove();
	return S_OK;
}
STDMETHODIMP CTest3DEngine::ToggleEngine(void)
{
	g_bEngineEnable = !g_bEngineEnable;
	return S_OK;
}

STDMETHODIMP CTest3DEngine::OnResizeWindow(void)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_p3DScenesVec[m_index])
		m_p3DScenesVec[m_index]->OnResizeWindow(NULL);

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

STDMETHODIMP CTest3DEngine::OnWndMsg(BSTR editor_name, PMSG WndMsg, INT* pnRet)
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

	//*pnRet = m_InputProxy.CheckInputEvent(pMsg);
	std::string strEditorName(BSTR_TO_STRING(editor_name));
	std::map<std::string, INT>::iterator Iter = m_NameHash.find(strEditorName);
	if (Iter == m_NameHash.end())
		return S_OK;

	INT index = Iter->second;
	*pnRet = m_InputProxies[index]->CheckInputEvent(pMsg);

	return S_OK;
}

STDMETHODIMP CTest3DEngine::LoadFile(BSTR strFileName)
{
	USES_CONVERSION;
	LoadPlayerModel(A2W("E:\\work\\Sword3\\blackhole\\tools\\sceneeditor\\debug\\Data\\public\\PlayerCheckTool\\Checker.mdl"));
	LoadAni(A2W("E:\\work\\Sword3\\blackhole\\tools\\sceneeditor\\debug\\Data\\public\\PlayerCheckTool\\Checker_skin.ani"), TRUE);
	BindToPlayer(strFileName, 0, A2W("Bip01 Head"));
	/*IKG3DModel* pModel = NULL;
	USES_CONVERSION;

    GETEDITOR() E_FAIL;

    if (FAILED(g_pEngineManager->NewKG3DRepresentObjectInTable(&m_pRepresentObject)))
        return E_FAIL;

    m_pRepresentObject->SetModel(0, BSTR_TO_STRING(strFileName), NULL, NULL);
    pEditor->SetObjectInHandle(m_pRepresentObject);*/
    
/*
	pPC->Unbind(BoneName);
	m_pRepresentObject->GetModel(&pModel, 0);
	pModel->BindToBone(pPC, BoneName);
*/
	return S_OK;
}

STDMETHODIMP CTest3DEngine::LoadAni(BSTR strFileName, BOOL bLoop)
{
    USES_CONVERSION;
    
	if(!g_IsFileExist(BSTR_TO_STRING(strFileName)))
	{
		STRING strErr = TEXT("文件不存在：") + STRING(BSTR_TO_STRING(strFileName));
		OutputDebugString(strErr.c_str());
		return S_OK;
	}

    if (m_RepresentObjects[m_index])
    {
        IKG3DModel* pModel = NULL;
        if (SUCCEEDED(m_RepresentObjects[m_index]->GetModel(pModel, 0)))
        {
			DWORD dwAniType = bLoop ? ENU_ANIMATIONPLAY_CIRCLE : ENU_ANIMATIONPLAY_ONCE;
			HRESULT hr = pModel->PlayAnimation(dwAniType,  BSTR_TO_STRING(strFileName), 1.0f, 0.0f, NULL, NULL, ANICTL_PRIMARY);
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
	//AfxMessageBox(BSTR_TO_STRING(strFileName));

	return S_OK;
}

STDMETHODIMP CTest3DEngine::UnloadPlayerModel(void)
{
	// todo:
	return S_OK;
}

STDMETHODIMP CTest3DEngine::SetAllPluginModelScale(FLOAT fScale)
{
	for (std::vector<IKG3DModel*>::iterator iter = m_ItemModels.begin(); iter != m_ItemModels.end(); iter++)
	{
		IKG3DModel* pPlugin = *iter;
		D3DXVECTOR3 vScale(fScale, fScale, fScale);
		pPlugin->SetScaling(&vScale);
	}
	
	return S_OK;	
}

STDMETHODIMP CTest3DEngine::LoadPlayerModel(BSTR strFileName)
{
	//LONG count = -1;
	//GetAniFrameCount(STRING_TO_BSTR("D:\\Kingsoft\\Game\\sword3-products\\trunk\\client\\data\\source\\player\\M2\\动作\\M2b04ty攻击01.ani"), &count);
	if(!g_IsFileExist(BSTR_TO_STRING(strFileName)))
	{
		STRING strErr = TEXT("文件不存在：") + STRING(BSTR_TO_STRING(strFileName));
		OutputDebugString(strErr.c_str());
		return S_OK;
	}

	if (FAILED(g_pEngineManager->NewKG3DRepresentObject(REPRESENTOBJECT_DEFAULT, 0, 0, &m_RepresentObjects[m_index])))
		return E_FAIL;

	if (m_pLastModel)
	{
		m_pLastModel->Release();
	}

	ItemsClear();//把所有plugin model卸载

	m_RepresentObjects[m_index]->SetModel(0, BSTR_TO_STRING(strFileName), NULL, NULL, TRUE);
	IKG3DModel *pModel = NULL;
	m_RepresentObjects[m_index]->GetModel(pModel, 0);
	m_pLastModel = dynamic_cast<IEKG3DModel*>(pModel);


	FLOAT x, y;
	//GetBBoxXY(STRING_TO_BSTR("D:/Kingsoft/Game/sword3-products/trunk/client/data/source/NPC_source/P001/模型/p001027c.mdl"), &x, &y);
	//GetBBoxXY(STRING_TO_BSTR("D:/Kingsoft/Game/sword3-products/trunk/client/data/source/NPC_source/E27010/模型/E27010.mdl"), &x, &y);
	//GetBBoxXY(STRING_TO_BSTR("D:/Kingsoft/Game/sword3-products/trunk/client/data/source/NPC_source/E27011/模型/E27011.mdl"), &x, &y);
	//GetBBoxXY(STRING_TO_BSTR("D:/Kingsoft/Game/sword3-products/trunk/client/data/source/NPC_source/A002/模型/A002.mdl"), &x, &y);	
	//GetBBoxXY(STRING_TO_BSTR("D:/Kingsoft/Game/sword3-products/trunk/client/data/source/NPC_source/E14009/模型/E14009.mdl"), &x, &y);


#if (defined USE_SCENE_EDITOR /*&& 0*/)
	GETEDITOR() E_FAIL;
	pEditor->ClearRepresentObjects_Design();
	pEditor->AddRepresentObject_Design(m_RepresentObjects[m_index]);
#else
 	GETMODELEDITOR() E_FAIL;
	pModelEditor->OpenModel(m_pLastModel, FALSE);
#endif

	//SetCaptionText(STRING_TO_BSTR(""), STRING_TO_BSTR(""));

	return S_OK;
}

STDMETHODIMP CTest3DEngine::SetModelScale(FLOAT fScale)
{
 	IKG3DModel *pModel = NULL;
	m_RepresentObjects[m_index]->GetModel(pModel, 0);
	D3DXVECTOR3 vScale(fScale, fScale, fScale);
	pModel->SetScaling(&vScale);
	return S_OK;
}

STDMETHODIMP CTest3DEngine::LoadNPCMarkSfxFile(BSTR sfx_name, FLOAT fHeight)
{
	IKG3DModel* pLastModel = NULL;
	m_RepresentObjects[m_index]->GetModel(pLastModel, 5);
	if (pLastModel)
		pLastModel->Release();

	if (FAILED(g_pEngineManager->NewKG3DRepresentObject(REPRESENTOBJECT_DEFAULT, 0, 0, &m_RepresentObjectSelectors[m_index])))
		return E_FAIL;
	m_RepresentObjects[m_index]->SetModel(5, BSTR_TO_STRING(sfx_name), NULL, NULL, FALSE);
	IKG3DModel *pModel = NULL;
	m_RepresentObjects[m_index]->GetModel(pModel, 5);
	D3DXVECTOR3 vec(0, fHeight, 0);
	pModel->SetTranslation(&vec);
	
#ifdef USE_SCENE_EDITOR
 	GETEDITOR() E_FAIL;
 	pEditor->AddRepresentObject(m_RepresentObjects[m_index], FALSE, FALSE, FALSE, FALSE);
#else
	GETMODELEDITOR() E_FAIL;
	pModelEditor->OpenModel((IEKG3DModel*)pModel, TRUE);
#endif

	return S_OK;
}

STDMETHODIMP CTest3DEngine::LoadSelectorSfxFile(BSTR sfx_name)
{
	if (FAILED(g_pEngineManager->NewKG3DRepresentObject(REPRESENTOBJECT_DEFAULT, 0, 0, &m_RepresentObjectSelectors[m_index])))
		return E_FAIL;
	m_RepresentObjects[m_index]->SetModel(1, BSTR_TO_STRING(sfx_name), NULL, NULL, FALSE);

	IKG3DModel *pModel = NULL;
	m_RepresentObjects[m_index]->GetModel(pModel, 1);
	D3DXVECTOR3 vec(0,1,0);
	pModel->SetTranslation(&vec);

	KG3DSFX* psfx = dynamic_cast<KG3DSFX*>(pModel);
	IEKG3DSFXSelectMark* pSelectedMark = NULL;
	psfx->AddSelectMark(&pSelectedMark);
	TCHAR szTga[MAX_PATH];
	sprintf(szTga, "%s\\data\\source\\other\\特效\\系统\\贴图\\选择\\选择特效A001.tga", g_EnginePath);
	pSelectedMark->LoadTexture(szTga);

	// ModelEditor的话，根本不用 m_RepresentObjects
#ifdef USE_SCENE_EDITOR
 	GETEDITOR() E_FAIL;
 	pEditor->AddRepresentObject(m_RepresentObjects[m_index], FALSE, FALSE, FALSE, FALSE);
#else
 	GETMODELEDITOR() E_FAIL;
	pModelEditor->OpenModel((IEKG3DModel*)pModel, TRUE/*是否增加*/);
// 	IEKG3DModel* p = NULL;
// 	pModelEditor->OpenModel(BSTR_TO_STRING(sfx_name), "", "", TRUE/*是否增加*/, &p);
#endif

	return S_OK;
}

STDMETHODIMP CTest3DEngine::ScaleSelectorSfx(FLOAT x, FLOAT y, FLOAT z)
{
	IKG3DModel* pModel = NULL;
	m_RepresentObjects[m_index]->GetModel(pModel, 1);
	if (pModel)
	{
		D3DXVECTOR3 vec(x, y, z);
		pModel->SetScaling(&vec);
		
		IEKG3DSFX* pSfx = dynamic_cast<IEKG3DSFX*>(pModel);
		//pSfx->SetScaleSize(x);
		IEKG3DSFXSelectMark* pSelectedMark = pSfx->GetSelectMark();

		static FLOAT oldWidth = 0;
		static FLOAT oldHeight = 0;

		if (oldWidth == 0 && oldHeight == 0)
		{
			oldWidth = pSelectedMark->GetTextureWidth();
			oldHeight = pSelectedMark->GetTextureHeight();
		}

		pSelectedMark->SetTextureWidth(oldWidth * x);
		pSelectedMark->SetTextureHeight(oldHeight * y);
	}

	return S_OK;
}
																						 /* 0 Left 1 Right */
STDMETHODIMP CTest3DEngine::LoadSkillSfx(BSTR strFileName, FLOAT scale, BSTR socketName, LONG iIndex)
{
	if (iIndex == 1 && !StrCmp(BSTR_TO_STRING(socketName), _T(""))) // 如果右手没说绑到哪，右手就不load该sfx
		return S_OK;

	if (FAILED(g_pEngineManager->NewKG3DRepresentObject(REPRESENTOBJECT_DEFAULT, 0, 0, &m_RepresentObjectSelectors[m_index])))
		return E_FAIL;
	m_RepresentObjects[m_index]->SetModel(3 + iIndex, BSTR_TO_STRING(strFileName), NULL, NULL, FALSE);

#ifdef USE_MODEL_EDITOR
	IKG3DModel *pModel = NULL;
	m_RepresentObjects[m_index]->GetModel(pModel, 3 + iIndex);
	GETMODELEDITOR() E_FAIL;
	pModelEditor->OpenModel((IEKG3DModel*)pModel, TRUE);
	D3DXVECTOR3 vScale(scale, scale, scale);
	pModel->SetScaling(&vScale);

	if (StrCmp(BSTR_TO_STRING(socketName), _T("")))
	{
		IKG3DModel* pModelTo = NULL;
		m_RepresentObjects[m_index]->GetModel(pModelTo, 0);
		if (pModelTo)
		{
			if ((BSTR_TO_STRING(socketName))[0] == _T('S') && (BSTR_TO_STRING(socketName))[1] == _T('_'))
			{
				return pModel->BindToSocket(pModelTo, BSTR_TO_STRING(socketName));
			}
			else
			{
				return pModel->BindToBone(pModelTo, BSTR_TO_STRING(socketName));
			}
		}
	}
#endif

	return S_OK;
}


STDMETHODIMP CTest3DEngine::LoadPlaneForSelector(BSTR strFileName)
{
	if (FAILED(g_pEngineManager->NewKG3DRepresentObject(REPRESENTOBJECT_DEFAULT, 0, 0, &m_RepresentObjectSelectors[m_index])))
		return E_FAIL;
	m_RepresentObjects[m_index]->SetModel(4, BSTR_TO_STRING(strFileName), NULL, NULL, FALSE);

#ifdef USE_SCENE_EDITOR
 	GETEDITOR() E_FAIL;
 	pEditor->AddRepresentObject(m_RepresentObjects[m_index], FALSE, FALSE, FALSE, FALSE);
#else
	IKG3DModel *pModel = NULL;
	m_RepresentObjects[m_index]->GetModel(pModel, 4);
	GETMODELEDITOR() E_FAIL;
	pModelEditor->OpenModel((IEKG3DModel*)pModel, TRUE);
#endif

	return S_OK;
}

STDMETHODIMP CTest3DEngine::LoadBillboardSfx(BSTR strFileName)
{
	if (FAILED(g_pEngineManager->NewKG3DRepresentObject(REPRESENTOBJECT_DEFAULT, 0, 0, &m_RepresentObjectSelectors[m_index])))
		return E_FAIL;
	m_RepresentObjects[m_index]->SetModel(2, BSTR_TO_STRING(strFileName), NULL, NULL, FALSE);

#ifdef USE_SCENE_EDITOR
 	GETEDITOR() E_FAIL;
 	pEditor->AddRepresentObject(m_RepresentObjects[m_index], FALSE, FALSE, FALSE, FALSE);
#else
	IKG3DModel *pModel = NULL;
	m_RepresentObjects[m_index]->GetModel(pModel, 2);
	GETMODELEDITOR() E_FAIL;
	pModelEditor->OpenModel((IEKG3DModel*)pModel, TRUE);
#endif

	return S_OK;
}

STDMETHODIMP CTest3DEngine::SetBillboardHeight(INT npc_height, INT head_top_adjust)
{
	IKG3DModel* pModel = NULL;
	m_RepresentObjects[m_index]->GetModel(pModel, 2);
	if (pModel)
	{
		D3DXVECTOR3 vec(0, ((FLOAT)npc_height) * ALTITUDE_UNIT / POINT_PER_ALTITUDE + 40 + head_top_adjust, 0);
		pModel->SetTranslation(&vec);
	}

	return S_OK;
}

STDMETHODIMP CTest3DEngine::SetCaptionHeight(FLOAT fHeight)
{
	IKG3DModel* pModel = NULL;
	m_RepresentObjects[m_index]->GetModel(pModel, 0);
	if (pModel)
	{
		pModel->SetCaptionFloorHeight(fHeight);
	}
	return S_OK;
}
STDMETHODIMP CTest3DEngine::SetCaptionText(BSTR strName, BSTR strTitle)
{
	IKG3DModel* pModel = NULL;
	m_RepresentObjects[m_index]->GetModel(pModel, 0);
	if (pModel)
	{
		pModel->SetCaption(0, BSTR_TO_STRING(strTitle));
		pModel->SetCaptionColor(0, D3DXCOLOR(.0f, 1.f, .0f, 1.f));
		pModel->SetCaption(1, BSTR_TO_STRING(strName));
		pModel->SetCaptionColor(1, D3DXCOLOR(.0f, 1.f, .0f, 1.f));
		pModel->SetPercentageStyle(D3DXCOLOR(1.0f, .0f, .0f, 1.0f));
	}
	return S_OK;
}

STDMETHODIMP CTest3DEngine::BindToPlayer(BSTR strFileName, DWORD dwType, BSTR strBindToObj)
{
	TCHAR strExt[MAX_PATH];
	USES_CONVERSION;
	_splitpath_s(BSTR_TO_STRING(strFileName), 
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
		pModelTable->IELoadResourceFromFile(BSTR_TO_STRING(strFileName),
			0,
			0,
			(IKG3DResourceBase**)&m_pSelectedObj);
		m_RepresentObjects[m_index]->GetModel(pPlayerModel, 0);
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
		pModelTable->IELoadResourceFromFile(BSTR_TO_STRING(strFileName),
			0,
			0,
			(IKG3DResourceBase**)&m_pSelectedObj);
		switch(dwType)
		{
		case 0://Bind to bone
			m_RepresentObjects[m_index]->GetModel(pPlayerModel, 0);
			ASSERT(pPlayerModel);
			((IEKG3DModel*)pPlayerModel)->UnbindChild(BSTR_TO_STRING(strBindToObj));
			m_pSelectedObj->BindToBone(pPlayerModel, BSTR_TO_STRING(strBindToObj));
			break;
		case 1://Bind to socket
			m_RepresentObjects[m_index]->GetModel(pPlayerModel, 0);
			ASSERT(pPlayerModel);
			((IEKG3DModel*)pPlayerModel)->UnbindChild(BSTR_TO_STRING(strBindToObj));
			m_pSelectedObj->BindToSocket(pPlayerModel, BSTR_TO_STRING(strBindToObj));
			break;
		}
	}
	
	return S_OK;
}

STDMETHODIMP CTest3DEngine::ZoomView(LONG lDelta)
{
#ifdef USE_SCENE_EDITOR
	GETEDITOR() E_FAIL; GETSCENEBASE() E_FAIL;
#else
	GETMODELEDITOR() E_FAIL; GETSCENEBASE_MODEL() E_FAIL;
#endif
	pBase->ExecAction(EXEACTION_MOUSE_WHEEL, 0, lDelta, 0);

	return S_OK;
}

// STDMETHODIMP CTest3DEngine::RotateModel(INT xoff, INT yoff)
// {
// 	IKG3DModel* pModel = NULL;
// 	m_RepresentObjects[m_index]->GetModel(pModel, 0);
// 	D3DXQUATERNION qtrn;
// 	pModel->GetRotation(&qtrn);
// 	m_ObjectRotation[m_index].yaw += xoff * 0.05f;
// 	m_ObjectRotation[m_index].pitch += yoff * 0.05f;
// 	D3DXQuaternionRotationYawPitchRoll(&qtrn, m_ObjectRotation[m_index].yaw, m_ObjectRotation[m_index].pitch, 0.0f);
// 	pModel->SetRotation(&qtrn);
// 	return S_OK;
// }

STDMETHODIMP CTest3DEngine::ChangeModelPart(BSTR filename, INT PartID)
{
	HRESULT hr = E_FAIL;
	IKG3DModel* pModel = NULL;
	IKG3DModel* pPlayerModel = NULL;
	IEKG3DModelTable* piModelTable = NULL;
	USES_CONVERSION;
	KG_PROCESS_ERROR(PartID != CB_ERR);

	hr = g_pEngineManager->GetIEKG3DModelTable(&piModelTable);
	
	hr = piModelTable->IELoadResourceFromFile(BSTR_TO_STRING(filename), 
		0,
		0,
		(IKG3DResourceBase**)&pModel);
	KG_COM_PROCESS_ERROR(hr);
	
	m_RepresentObjects[m_index]->GetModel(pPlayerModel, 0);
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
	
	hr = piModelTable->IELoadResourceFromFile(BSTR_TO_STRING(strfilename),
		0,
		0,
		(IKG3DResourceBase**)&m_pSelectedObj);
	KG_COM_PROCESS_ERROR(hr);
	m_RepresentObjects[m_index]->GetModel(pPlayerModel, 0);
	
	//ASSERT(pPlayerModel);	
	if (pPlayerModel)
	{
		m_pSelectedObj->BindToSocket(pPlayerModel, "s_face");
	}

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
	m_ItemModels.clear();
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

	m_RepresentObjects[m_index]->GetModel(pPlayerModel, 0);
	ASSERT(pPlayerModel);

	hr = pPlayerModel->SaveToFile(BSTR_TO_STRING(strFileName), 0);	
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
STDMETHODIMP CTest3DEngine::LoadMaterial(BSTR strFileName)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    HRESULT hr = E_FAIL;
    IKG3DModel *pModel = NULL;
    IEKG3DModelTable *pModelTable = NULL;
    USES_CONVERSION;
    hr = g_pEngineManager->GetIEKG3DModelTable(&pModelTable);
    if (FAILED(hr))
        return S_OK;//hr;
    if (!m_RepresentObjects[m_index])
        return S_OK;//E_FAIL;
    
    m_RepresentObjects[m_index]->GetModel(pModel, 0);
    ASSERT(pModel);

    hr = pModel->LoadMaterialFromFile(BSTR_TO_STRING(strFileName), 0);
    if (FAILED(hr))
    {
		TCHAR s_strOutputInfo[MAX_PATH << 1];
        sprintf_s(s_strOutputInfo, 
            MAX_PATH << 1,
            "读取材质文件%s失败.",
            strFileName);
        MessageBoxA(NULL, s_strOutputInfo, "错误", MB_OK);
    }
    return S_OK;
}

STDMETHODIMP CTest3DEngine::LoadAnimation(BSTR strFileName, BOOL bLoopPlay)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    USES_CONVERSION;
    DWORD dwPlayType = ENU_ANIMATIONPLAY_CIRCLE;
    if (!bLoopPlay)
    {
        dwPlayType = ENU_ANIMATIONPLAY_ONCE;
    }

    if (m_RepresentObjects[m_index])
    {
        IKG3DModel* pModel = NULL;
        if (SUCCEEDED(m_RepresentObjects[m_index]->GetModel(pModel, 0)))
        {            
            HRESULT hr = pModel->PlayAnimation(dwPlayType, BSTR_TO_STRING(strFileName), 1.0f, 0.0f, NULL, NULL, ANICTL_PRIMARY);
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

STDMETHODIMP CTest3DEngine::LoadPluginModel(BSTR strFileName, BSTR strBindDest)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    HRESULT hr = E_FAIL;
    IKG3DModel* pPlayerModel = NULL;
    IKG3DModel* pItemModel = NULL;
    IEKG3DModelTable* piModelTable = NULL;
    TCHAR strBindDestConverted[MAX_PATH];
    USES_CONVERSION;
    //SAFE_RELEASE(m_ItemModel);
    hr = g_pEngineManager->GetIEKG3DModelTable(&piModelTable);
    //KGLOG_COM_PROCESS_ERROR(hr);

    hr = piModelTable->IELoadResourceFromFile(BSTR_TO_STRING(strFileName),
        0,
        0,
        (IKG3DResourceBase**)&pItemModel);
    KG_COM_PROCESS_ERROR(hr);
    m_RepresentObjects[m_index]->GetModel(pPlayerModel, 0);
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
    m_ItemModels.push_back(pItemModel);
Exit1:
    return S_OK;
Exit0:
    return E_FAIL;
}

STDMETHODIMP CTest3DEngine::LoadPluginMaterial(BSTR strSocketName, BSTR strMtlFile)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IKG3DModel *pModel = NULL;
	IEKG3DModel *peModel = NULL;
	std::vector<IEKG3DModel*> vecChildModels;

	KG_PROCESS_ERROR(m_RepresentObjects[m_index]);

	HRESULT hr = m_RepresentObjects[m_index]->GetModel(pModel, 0);
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

STDMETHODIMP CTest3DEngine::ClearPluginModel(BSTR strBindDest)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    IKG3DModel *pModel = NULL;
    IEKG3DModel *peModel = NULL;
    HRESULT hr = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    USES_CONVERSION;
    KG_PROCESS_ERROR(m_RepresentObjects[m_index]);

    hr = m_RepresentObjects[m_index]->GetModel(pModel, 0);
    KG_COM_PROCESS_ERROR(hr);

    peModel = dynamic_cast<IEKG3DModel*>(pModel);
    KG_PROCESS_ERROR(peModel);

    hr = peModel->UnbindChild(BSTR_TO_STRING(strBindDest));
    KG_COM_PROCESS_ERROR(hr);

    hRetCode = S_OK;
Exit0:
    return hRetCode;
}

STDMETHODIMP CTest3DEngine::GetFileNameHash(BSTR fileName, DWORD* hashid)
{
	*hashid = g_FileNameHash(BSTR_TO_STRING(fileName));
	return S_OK;
}

STDMETHODIMP CTest3DEngine::QuaternionRotationByNDir(FLOAT* x, FLOAT* y, FLOAT* z, FLOAT* w, LONG nDir)
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

INT _getAniFrameCount(TCHAR* filename)
{
	IFile* pFile = NULL;
	pFile = g_OpenFile(filename);
	TCHAR szBuf[1024];
	pFile->Read(szBuf, 12);
	INT dwType = -1;
	pFile->Read(&dwType, sizeof(DWORD));
	INT count = -1;
	if (dwType == ANIMATION_BONE || dwType == ANIMATION_BONE_RELATIVE || dwType == ANIMATION_BONE_RTS)
	{
		pFile->Read(&szBuf, 34);
		pFile->Read(&count, sizeof(DWORD));
		float frameLen = 0.0f;
		pFile->Read(&frameLen, sizeof(float));
		count = (INT)((count - 1) * frameLen * 16.0f / 1000.0f) + 1;
	}
	else if (dwType == ANIMATION_VERTICES_RELATIVE || dwType == ANIMATION_VERTICES)
	{
		pFile->Read(&szBuf, 38);
		pFile->Read(&count, sizeof(DWORD));
		float frameLen = 0.0f;
		pFile->Read(&frameLen, sizeof(float));
		count = (INT)((count - 1) * frameLen * 16.0f / 1000.0f) + 1;
	}
	else
	{
		MessageBox(NULL, TEXT("_getAniFrameCount文件读取错误（非法的dwType）。"), NULL, 0);
	}

	return count;	
}

STDMETHODIMP CTest3DEngine::GetAniFrameCount(BSTR strAniFileName, LONG* count)
{
	STRING str(BSTR_TO_STRING(strAniFileName));
	std::transform(str.begin(), str.end(), str.begin(), std::tolower);

	if (str.find(TEXT(".tani")) != string::npos)
	{
		IFile* pFile = NULL;
		pFile = g_OpenFile(str.c_str());
		DWORD dwDummi = 0;
		pFile->Read(&dwDummi, sizeof(DWORD));
		pFile->Read(&dwDummi, sizeof(DWORD));
		TCHAR szBuf[MAX_PATH];
		for (int i = 0; ; i += sizeof(TCHAR))
		{
			pFile->Read(szBuf + i, sizeof(TCHAR));
			if (szBuf[i] == '\0')
				break;
		}
		*count = _getAniFrameCount(szBuf);
	}
	else if (str.find(TEXT(".ani")) != string::npos)
	{
		*count = _getAniFrameCount((TCHAR*)str.c_str());
	}
	else
	{
		MessageBox(NULL, TEXT("CTest3DEngine::GetAniFrameCount参数错误，文件后缀名不符。"), NULL, 0);
	}

	return S_OK;
}

STDMETHODIMP CTest3DEngine::GetBBoxXY(BSTR mdl, FLOAT* x, FLOAT* y)
{
	if(!g_IsFileExist(BSTR_TO_STRING(mdl)))
	{
		STRING strErr = TEXT("文件不存在：") + STRING(BSTR_TO_STRING(mdl));
		OutputDebugString(strErr.c_str());
		return S_OK;
	}

	IEKG3DRepresentObject* representObj = NULL;

	if (FAILED(g_pEngineManager->NewKG3DRepresentObject(REPRESENTOBJECT_DEFAULT, 0, 0, &representObj)))
		return E_FAIL;

	representObj->SetModel(0, BSTR_TO_STRING(mdl), NULL, NULL, TRUE);
	IKG3DModel *p = NULL;
	representObj->GetModel(p, 0);
	IEKG3DModel* pModel = dynamic_cast<IEKG3DModel*>(p);
	D3DXVECTOR3 A(0,0,0), B(0,0,0);
	pModel->GetBBox(A, B);

	*x = fabs(A.x - B.x);
	*y = fabs(A.y - B.y);
	FLOAT z = fabs(A.z - B.z);

	pModel->Release();

	return S_OK;
}

STDMETHODIMP CTest3DEngine::GetMdlBBoxMax2DLength(BSTR mdl, FLOAT* len)
{
	*len = 0.0f;

	if(!g_IsFileExist(BSTR_TO_STRING(mdl)))
	{
		STRING strErr = TEXT("文件不存在：") + STRING(BSTR_TO_STRING(mdl));
		OutputDebugString(strErr.c_str());
		return S_OK;
	}

	IEKG3DRepresentObject* representObj = NULL;

	if (FAILED(g_pEngineManager->NewKG3DRepresentObject(REPRESENTOBJECT_DEFAULT, 0, 0, &representObj)))
		return E_FAIL;

	representObj->SetModel(0, BSTR_TO_STRING(mdl), NULL, NULL, TRUE);
	IKG3DModel *p = NULL;
	representObj->GetModel(p, 0);
	IEKG3DModel* pModel = dynamic_cast<IEKG3DModel*>(p);
	if (!pModel)
		return S_OK;

	D3DXVECTOR3 A(0,0,0), B(0,0,0);
	pModel->GetBBox(A, B);

	FLOAT x = fabs(A.x - B.x);
	FLOAT y = fabs(A.z - B.z);
	*len = x > y ? x : y;

	pModel->Release();

	return S_OK;
}