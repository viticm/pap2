// SceneEditorDoc.cpp : implementation of the KSceneEditorDoc class
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "SceneEditorDoc.h"
#include "CntrItem.h"
//#include "MainFrm.h" //(by dengzhihui 2006年7月6日 14:51:57
//#include "SceneEditorCommon.h"//by dengzhihui Refactor 2006年11月6日

#include "SO3Represent/SO3RepresentInterface.h"
#include "KSceneEditorDocLogical.h"
#include "IEKG3DTerrain.h"
#include "KNpcTemplateList.h"
#include "KSceneEditorDocLogical.h"
#include "SO3Represent/SO3RepresentInterface.h"
#include "KSO3World.h"
#include "KPatrolPath.h"
#include "KScene.h"
#include "IEKG3DScene.h"
#include "KG3DSceneLogicalEditorBase.h"
#include "KLogicObstacleGenerator.h"
#include "IKGSO3WorldClient.h"
#include "IEEditor.h"
#include "IEKG3DSceneObjectPlacementTool.h"
#include "KSceneHelperFuncs.h"
#include "INpcFile.h"
#include "IDoodadFile.h"
#include "KSceneHelperFuncs.h"	//SceneFileSaveConfirmer
#include "IKG3DMessenger.h"
#include "KSceneEditorDialogCheckRelive.h"

extern IKSO3RepresentHandler* g_pRepresentHandler;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//(by dengzhihui Refactor 2006年11月6日
#define GETSCENE() \
	IEKG3DScene* pScene = GetScene();\
	_ASSERTE(pScene); \
	if(!pScene) return
#define GETEDITOR() \
	IEKG3DSceneSceneEditor* pEditor = NULL;\
	pScene->GetSceneSceneEditor(&pEditor);\
	_ASSERTE(pEditor); \
	if(!pEditor) return

#define GETSCENEBASE() \
	IEKG3DSceneEditorBase* pBase = NULL;\
	pEditor->GetSceneEditorBase(&pBase); \
	_ASSERTE(pBase);\
	if (!pBase) return

#define GETLOGIC() \
	IEKG3DSceneLogicalEditorBase * pLogic = NULL; \
	pEditor->GetIELogicScene(&pLogic);\
	_ASSERTE(pLogic);\
	if(!pLogic) return
//)

/////////////////////////////////////////////////////////////////////////////
// KSceneEditorDoc

IMPLEMENT_DYNCREATE(KSceneEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(KSceneEditorDoc, CDocument)
	//{{AFX_MSG_MAP(KSceneEditorDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
	//ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, CDocument::OnUpdatePasteMenu)
	//ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK, CDocument::OnUpdatePasteLinkMenu)
	//ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT, CDocument::OnUpdateObjectVerbMenu)
	//ON_COMMAND(ID_OLE_EDIT_CONVERT, CDocument::OnEditConvert)
	//ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, CDocument::OnUpdateEditLinksMenu)
	//ON_COMMAND(ID_OLE_EDIT_LINKS, CDocument::OnEditLinks)
	//ON_UPDATE_COMMAND_UI_RANGE(ID_OLE_VERB_FIRST, ID_OLE_VERB_LAST, CDocument::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_PHYSISCSETTING, &KSceneEditorDoc::OnPhysiscsetting)
	ON_COMMAND(ID_CLEARALLTRAP, &KSceneEditorDoc::OnClearalltrap)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneEditorDoc construction/destruction
KDlgLogicSaveProgress*  g_pProgressBar = NULL;
int g_nProgressBarRef = 0;

KSceneEditorDoc::KSceneEditorDoc() : m_lpLogicEditor(NULL)
{
	//EnableCompoundFile();
    BOOL *pbEnableAddEntity = NULL;
    m_lpLogicalData = new KSceneEditorDocLogical;
    
    g_pEngineManager->GetEnableAddEntity(&pbEnableAddEntity);
	*pbEnableAddEntity = FALSE;

	m_lpPhysiscSetting = NULL;
	m_lpLogicalform = NULL;

	if(g_nProgressBarRef==0)
	{
		g_pProgressBar = new KDlgLogicSaveProgress(AfxGetMainWnd());
		//g_pProgressBar->Create(KDlgLogicSaveProgress::IDD,AfxGetMainWnd());
		g_pProgressBar->m_progressModel.SetRange32(0, 100);
		g_pProgressBar->ShowWindow(SW_HIDE);
		g_pProgressBar->m_progressModel.SetPos(0);
	}
	g_nProgressBarRef++;
}

KSceneEditorDoc::~KSceneEditorDoc()
{
	//OutputDebugString("中华人民共和国");
	SAFE_DELETE(KSceneEditorDialogCheckRelive::This);
	SAFE_DELETE(m_lpLogicalData);
	SAFE_DELETE(m_lpLogicEditor);

	SAFE_DELETE(m_lpPhysiscSetting);

	g_nProgressBarRef--;
	if(g_nProgressBarRef==0) 
	{
		g_pProgressBar->DestroyWindow();
		SAFE_DELETE(g_pProgressBar);
	}
}

HRESULT KSceneEditorDoc::AttachLogicalSceneTo3DScene(KScene* pLogicalScene)
{
	HRESULT hr = E_FAIL;
	IKG3DScene* pScene = NULL;
    IEKG3DScene* piScene = NULL;
	IEKG3DSceneSceneEditor* pEditor = NULL;
    IEKG3DTerrain *pTerrain = NULL;
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	ASSERT(pLogicalScene);
	_ASSERTE(m_lpLogicEditor);

	hr = g_pRepresentHandler->GetScene(pLogicalScene->m_dwID, &pScene);
	KGLOG_COM_PROCESS_ERROR(hr);

    piScene = dynamic_cast<IEKG3DScene *>(pScene);
    piScene->GetSceneSceneEditor(&pEditor);

	ASSERT(pEditor);

	pManager->SetFocusScene(piScene);
	SetScene(piScene);

	m_lpLogicEditor->SetLogicScene(pLogicalScene);
	pEditor->IESetLogicalScene(m_lpLogicEditor);

	m_lpLogicalData->SetSceneEditor(pEditor);

	return S_OK;
Exit0:
	return hr;
}


HRESULT KSceneEditorDoc::NewLogicalScene()
{
	HRESULT hr = E_FAIL;
	static DWORD const NEW_TERRAIN_WIDTH = 128;

	DWORD RegionW = NEW_TERRAIN_WIDTH / REGION_GRID_WIDTH;
	DWORD RegionH = NEW_TERRAIN_WIDTH / REGION_GRID_WIDTH;

	if (NEW_TERRAIN_WIDTH % REGION_GRID_WIDTH != 0)
	{
		++RegionW;
		++RegionH;
	}

	KScene* lpLogicalScene = g_pSO3World->NewEditorScene(RegionW * 2, RegionH * 2);
	KGLOG_PROCESS_ERROR(lpLogicalScene);

	hr = AttachLogicalSceneTo3DScene(lpLogicalScene);
	KGLOG_COM_PROCESS_ERROR(hr);

	m_LogicalConnection.BindLogicalScene(lpLogicalScene);

	return S_OK;
Exit0:
	return hr;
}

inline void InitLogicEditor(IEKG3DSceneLogicalEditorBase** ppLogicEditor)
{
	*ppLogicEditor = new KG3DSceneLogicalEditorBase();
	(*ppLogicEditor)->Init();
}

BOOL KSceneEditorDoc::OnNewDocument()
{
    HRESULT hrRetCode = NULL;
    IEKG3DScene* pScene = NULL;
    IEKG3DSceneSceneEditor* pEditor = NULL;

	if (!CDocument::OnNewDocument())
		return FALSE;

	//(by dengzhihui Refactor 2006年11月6日
	BOOL bUseSO3Logic = FALSE;
	HRESULT hr = g_pEngineManager->GetUseSO3Logical(&bUseSO3Logic);
	KGLOG_COM_PROCESS_ERROR(hr);
	if(bUseSO3Logic)
	//)
	{
		InitLogicEditor(&m_lpLogicEditor);
		
		IEKG3DScene* pScene = NULL;
		hr = NewLogicalScene();
		KGLOG_COM_PROCESS_ERROR(hr);

	}
	else
	{
		IEKG3DScene* pScene = NULL;
		g_pEngineManager->NewOneScene(SCENETYPE_SCENEEDITOR,(IKG3DScene**)&pScene);//by dengzhihui Refactor 2006年11月6日
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
		SetScene(pScene);
	}

    //建立连接

    pScene = GetScene();
    KGLOG_PROCESS_ERROR(pScene);
    
    hrRetCode = pScene->GetSceneSceneEditor(&pEditor);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	pEditor->SetLogicalSceneConnection(&m_LogicalConnection);

    pEditor->Advise(m_lpLogicalData);
	//(by dengzhihui 2006年8月4日 11:34:00
	SetModifiedFlag();
	//)
	return TRUE;

Exit0:

	return FALSE;
}

BOOL KSceneEditorDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
{
	////////////////////////////////
	/*判断要不要运行重生检查*/
	////////////////////////////////
	if (KSceneEditorDialogCheckRelive::This && KSceneEditorDialogCheckRelive::This->GetCheckReliveEnable())
	{
		KSceneEditorDialogCheckRelive::This->ShowWindow(SW_NORMAL);
		if (KSceneEditorDialogCheckRelive::This->IsLoseRelive())
		{
			return FALSE;
		}
		else
		{
			KSceneEditorDialogCheckRelive::This->ShowWindow(SW_HIDE);
		}
	}
	if (m_dlgSaveOption.DoModal() == IDCANCEL)
		return FALSE;

	return CDocument::DoSave(lpszPathName, bReplace);
}

BOOL KSceneEditorDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	BOOL bRetCode = FALSE;

	g_pProgressBar->CenterWindow();
	g_pProgressBar->ShowWindow(SW_NORMAL);
	
	::SetCurrentDirectory(g_szDefWorkDirectory);
	TCHAR strPathRet[MAX_PATH];
	g_pEngineManager->GetRelativePath(g_szDefWorkDirectory,lpszPathName,strPathRet);

	//(by dengzhihui Refactor 2006年11月6日
	BOOL bUseSO3Logic = FALSE;
	HRESULT hr = g_pEngineManager->GetUseSO3Logical(&bUseSO3Logic);
	KGLOG_COM_PROCESS_ERROR(hr);

	//IEKG3DSceneSceneEditor* pEditor = NULL;
	//static_cast<KG3DSceneSceneEditor*> (GetScene());
	IEKG3DScene* pScene = GetScene();
	if(pScene)
	{
		if(m_dlgSaveOption.m_bTerrainMap)
		{
			GETEDITOR() FALSE;
			pEditor->SetSnapToolScale(m_dlgSaveOption.m_fScale);
			
			pScene->RegisterListener(KM_LOADING_PERCENT, g_pProgressBar);

			TCHAR strDrive[MAX_PATH];
			TCHAR strDir[MAX_PATH];
			TCHAR strFileName[MAX_PATH];
			TCHAR strExt[MAX_PATH];
			_splitpath(lpszPathName, strDrive, strDir, strFileName, strExt);

			TCHAR TerrainMapName[256];
			wsprintf(TerrainMapName,"%s%s%s_TerrainMap.tga",strDrive, strDir,strFileName);
			pEditor->ExprotTerrainTexture(TerrainMapName);
			pScene->UnRegisterListener(KM_LOADING_PERCENT, g_pProgressBar);
		}
		if(m_dlgSaveOption.m_b3DScene)
		{
			if(m_dlgSaveOption.m_bComputeBBox)
			{
				GETEDITOR() FALSE;
				pEditor->ComputeAllEntityBBox();
			}

			INT nSave = TRUE;
			std::pair<DWORD, std::tstring> prTemp = m_dlgSaveOption.GetSceneSaveOption();

#if 1
#else
			//<DISABLE_SAVE_OPTIONS>
			prTemp.first = EM_SAVE_OP_ALL;//测试阶段，不能使用这个功能
			prTemp.second = _T("");
#endif

			if (0 == prTemp.first)
			{
				MessageBox(NULL, _T("没有选择保存任何3D场景中的元素,3D场景不会保存"), _T("确认保存选项"), MB_OK);
				nSave = FALSE;
			}
			else if (EM_SAVE_OP_ALL != prTemp.first)
			{
				std::tstring strTemp = _T("你对3D场景中的元素的保存选择如下, 如果不对话，请按取消，3D场景将不会保存:\r\n");
				strTemp += prTemp.second;
				INT nOK = MessageBox(NULL, strTemp.c_str(), _T("确认保存选项"), MB_OKCANCEL);
				if (nOK != IDOK)
				{
					nSave = FALSE;
				}
			}

			if(nSave)
			{
				DWORD dwSaveOp = prTemp.first;
				
				KSF::SceneFileSaveConfirmer fileSaveConfirmer;

				fileSaveConfirmer.RecordSaveFiles(dwSaveOp, lpszPathName);	//全路径

				pScene->SaveToFile(strPathRet, dwSaveOp);//<SAVE_OPTION>
				
				fileSaveConfirmer.ConfirmFileWrite();
			}
		}

		if(m_dlgSaveOption.m_bLogicalExchange && bUseSO3Logic)
		{
			TCHAR path_buffer[256];
			TCHAR drive[256];
			TCHAR dir[256];
			TCHAR fname[256];
			TCHAR ext[256];

			wsprintf(path_buffer,"%s",strPathRet);
			_splitpath( path_buffer, drive, dir, fname, ext );

			TCHAR str[MAX_PATH];
			wsprintf(str,"%s%s%s.Map.Logical",drive,dir,fname);
			m_lpLogicalData->SaveToIniFile(str);
		}
	}
	//)	
	if(bUseSO3Logic)
	{
		char szFileName[MAX_PATH];
		g_ExtractFileName(szFileName, lpszPathName);

		int nIndex = (int)strlen(szFileName) - 1;
		while (szFileName[nIndex] != '.' && nIndex >= 0)
		{
			nIndex--;
		}

		if (nIndex >= 0)
		{
			szFileName[nIndex] = 0;
		}

		// 算doodad可能的动态障碍group id
		CalcDoodadObstacleGroupIdIntoRepresentObject();

		// 存cell，算障碍
		SetCurrentDirectory(g_szDefWorkDirectory);
		KScene *lpLogicalScene = NULL;//pEditor->GetLogicalScene();
		GETSCENE() FALSE; GETEDITOR() FALSE; GETLOGIC() FALSE;
		pLogic->GetLogicScene((LPVOID*)&lpLogicalScene);
		KGLOG_PROCESS_ERROR(lpLogicalScene);
		pLogic->SetLogicScene(lpLogicalScene);

		pEditor->PauseMultiThreadLoad(TRUE);

		if(m_dlgSaveOption.m_bComputeCells)
		{
			if(m_dlgSaveOption.m_bOnlyAppointCell)
			{
				int nXStart,nZStart,nCountX,nCountZ;
				pScene->GetAppointLogicalRegion(nXStart,nZStart,nCountX,nCountZ);
				D3DXVECTOR2 A(nXStart* 3200.0F,nZStart * 3200.0F);
				D3DXVECTOR2 C = A + D3DXVECTOR2(nCountX*3200.0F,nCountZ* 3200.0F);
				hr = pEditor->ForceLoadEntity(A,C);
			}
			else
				hr = pEditor->ForceLoadAllEntity();
			KGLOG_COM_PROCESS_ERROR(hr);

		    hr = TransformatCell();
		    KGLOG_COM_PROCESS_ERROR(hr);

		}

		// 场景所有cell更新dwRest信息
		if (m_dlgSaveOption.m_bUpdateAllRest)
		{
			SaveAllCellRest();
		}

		// 存logicScene
		if(m_dlgSaveOption.m_bLogicalExchange)
		{
			ExchangeLogicalData(lpszPathName);
		}
		else
		{
			bRetCode = lpLogicalScene->Save(szFileName,&ShowProgress);
			KGLOG_PROCESS_ERROR(bRetCode);
		}

		pEditor->PauseMultiThreadLoad(FALSE);
	}

	this->SetModifiedFlag( FALSE );

	g_pProgressBar->ShowWindow(SW_HIDE);
	KSceneEditorDialogCheckRelive::This->SetCheckReliveEnableToFile();
Exit0:

	return TRUE;
}

HRESULT KSceneEditorDoc::SaveAllCellRest()
{
	IEKG3DScene* pScene = GetScene();
	if (!pScene) return E_FAIL;
	GETEDITOR() FALSE;

	int iAllRest = m_dlgSaveOption.m_bAllRest ? 1 : 0;

	HRESULT hrRetCode = NULL;
	int nRetCode = false;

	hrRetCode = pScene->GetSceneSceneEditor(&pEditor);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	IEKG3DSceneLogicalEditorBase* pLogicEditor = NULL;
	hrRetCode = pEditor->GetIELogicScene(&pLogicEditor);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	KScene* pLogicScene = NULL;
	hrRetCode = pLogicEditor->GetLogicScene((void **)&pLogicScene);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);
	KGLOG_PROCESS_ERROR(pLogicScene);

	int nX_Start,nZ_Start,nX_Width,nZ_Width;
	int nX_LogicRegionStart = 0, nY_LogicRegionStart = 0, nX_LogicRegionEnd = 0, nY_LogicRegionEnd = 0;
	int nCellCountX = 0, nCellCountY = 0, nRegionCountX = 0, nRegionCountY = 0;

	pScene->GetLogicalSceneSize(&nX_Start,&nZ_Start,&nX_Width,&nZ_Width);
	KGLOG_PROCESS_ERROR(nX_Width > 0);
	KGLOG_PROCESS_ERROR(nZ_Width > 0);

	nCellCountX = nX_Width * 100  / LOGICAL_CELL_CM_LENGTH;
	nCellCountY = nZ_Width * 100  / LOGICAL_CELL_CM_LENGTH;

	KGLOG_PROCESS_ERROR(nCellCountX > 0);
	KGLOG_PROCESS_ERROR(nCellCountY > 0);

	nRegionCountX = (nCellCountX - 1) / REGION_GRID_WIDTH + 1;
	nRegionCountY = (nCellCountY - 1) / REGION_GRID_HEIGHT + 1;

	nRetCode = pLogicScene->Resize(nRegionCountX, nRegionCountY, eResizeSceneAlignLeftBottom);
	KGLOG_PROCESS_ERROR(nRetCode);

	nX_LogicRegionEnd = nX_LogicRegionStart + nRegionCountX;
	nY_LogicRegionEnd = nY_LogicRegionStart + nRegionCountY;

	int nTotalRegion = nRegionCountX * nRegionCountY;

	g_pProgressBar->m_progressModel.SetRange32(0, 100);
	g_pProgressBar->ShowWindow(SW_NORMAL);

	char szMessage[128];
	int nCompleteRegion = 0;

	// 遍历每个Region
	for (int nY = nY_LogicRegionStart; nY < nY_LogicRegionEnd; nY++)
	{
		for (int nX = nX_LogicRegionStart; nX < nX_LogicRegionEnd; nX++)
		{
			KRegion* pLogicRegion = pLogicScene->GetRegion(nX, nY);
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

			snprintf(szMessage, sizeof(szMessage), "Region: %d / %d", nCompleteRegion, nTotalRegion);
			szMessage[sizeof(szMessage) - 1] = '\0';
			g_pProgressBar->m_staticModel.SetWindowText(szMessage);
			g_pProgressBar->m_progressModel.SetPos(nCompleteRegion * 100 / nTotalRegion);
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KSceneEditorDoc::TransformatCell()
{
    HRESULT hrRetCode = NULL;
    IEKG3DScene* pScene = NULL;
    IEKG3DSceneSceneEditor* pEditor = NULL;
    IEKG3DSceneLogicalEditorBase * pLogicEditor = NULL;
    //IEKG3DTerrain *piTerrain = NULL;
    KScene *lpLogicalScene = NULL;


	BOOL bRetCode = FALSE;

    pScene = GetScene();
    KGLOG_PROCESS_ERROR(pScene);

    hrRetCode = pScene->GetSceneSceneEditor(&pEditor);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = pEditor->GetIELogicScene(&pLogicEditor);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = pLogicEditor->GetLogicScene((void **)&lpLogicalScene);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
	KGLOG_PROCESS_ERROR(lpLogicalScene);

	if(!m_dlgSaveOption.m_bOnlyAppointCell)
	{
		bRetCode = lpLogicalScene->DeleteAllObstacle();
		KGLOG_CHECK_ERROR(bRetCode);
	}

	TransformAllHeightToCellHeight(pEditor,lpLogicalScene);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KSceneEditorDoc::TransformAllHeightToCellHeight(IEKG3DSceneSceneEditor* pEditorA,KScene* pLogicalScene)
{

    HRESULT                 hResult                 = E_FAIL;
    int                     nRetCode                = false;
	BOOL                    bUseSO3Logic            = false;
    IEKG3DScene*            pi3DScene               = NULL; 
    KLogicObstacleGenerator LogicObstacleGenerator;

	nRetCode = g_pEngineManager->GetUseSO3Logical(&bUseSO3Logic);
	KGLOG_COM_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(bUseSO3Logic);

    nRetCode = pEditorA->GetScene(&pi3DScene);
    KGLOG_COM_PROCESS_ERROR(nRetCode);
    ASSERT(pi3DScene);

    m_LogicalConnection.BindLogicalScene(pLogicalScene);
    pEditorA->SetLogicalSceneConnection(&m_LogicalConnection);
	
	nRetCode = LogicObstacleGenerator.GenerateSceneLogicObstacleData(pi3DScene, pLogicalScene,m_dlgSaveOption.m_bOnlyAppointCell,m_dlgSaveOption.m_bClearCell);
	
    KGLOG_COM_PROCESS_ERROR(nRetCode);
	
    hResult = S_OK;
Exit0:
    return hResult;
}


BOOL KSceneEditorDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	KSceneEditorDialogCheckRelive::This->SetCurrentDir();
	::SetCurrentDirectory(g_szDefWorkDirectory);
	TCHAR RetPath[MAX_PATH];
	g_pEngineManager->GetRelativePath(g_szDefWorkDirectory,lpszPathName,RetPath);

	g_pProgressBar->CenterWindow();
	g_pProgressBar->ShowWindow(SW_NORMAL);

    HRESULT hrRetCode = NULL;
    IEKG3DScene* pScene = NULL;
    IEKG3DSceneSceneEditor* pEditor = NULL;
    BOOL bUseSO3Logical = FALSE;

    g_pEngineManager->GetUseSO3Logical(&bUseSO3Logical);
	if (bUseSO3Logical)
	{
		InitLogicEditor(&m_lpLogicEditor);

		TCHAR str[256];
		wsprintf(str,"%s",lpszPathName);
		
		{
			KScene* lpLogicalScene = g_pSO3World->LoadEditorScene((char*)RetPath,&ShowProgress);

			if (lpLogicalScene == NULL)
			{
				hrRetCode = NewLogicalScene();
				KGLOG_COM_PROCESS_ERROR(hrRetCode);
			}
			else
			{
				hrRetCode = AttachLogicalSceneTo3DScene(lpLogicalScene);
				KGLOG_COM_PROCESS_ERROR(hrRetCode);

                m_LogicalConnection.BindLogicalScene(lpLogicalScene);
			}
		}
	}
	else
	{
		IKG3DScene* piScene = NULL;
		g_pEngineManager->NewOneScene(SCENETYPE_SCENEEDITOR, &piScene);
        pScene = static_cast<IEKG3DScene *>(piScene);
		SetScene(pScene);
		g_pEngineManager->SetFocusScene(pScene);

		if(pScene)
		{
			KG3DSCENE_INITSTRUCT TempStruct;
			
			TempStruct.m_vecGameGPSInitPos = D3DXVECTOR3(0,0,0);
			HRESULT hr = pScene->LoadFromFileEx(RetPath, 0, 0, reinterpret_cast<DWORD_PTR>(&TempStruct), 0);
			//</CTH>
			if (FAILED(hr))
			{
				MessageBox(NULL, "场景文件读取失败，即将退出","警告",MB_OK | MB_ICONWARNING);
				goto Exit0;
			}
		}
	}

	//(by dengzhihui 2006年8月4日 11:34:00
	SetModifiedFlag();

	if (bUseSO3Logical)
	{
		//建立连接
		BOOL bUseLogicObject = TRUE;
		g_pEngineManager->GetUseLogicObject(&bUseLogicObject);
		
		pScene = GetScene();
		KGLOG_PROCESS_ERROR(pScene);

		hrRetCode = pScene->GetSceneSceneEditor(&pEditor);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
		_ASSERTE(pEditor);
		pEditor->SetLogicalSceneConnection(&m_LogicalConnection);

		pEditor->Advise(m_lpLogicalData);

		TCHAR str[256];
		wsprintf(str,"%s.Logical",lpszPathName);
		m_lpLogicalData->SetSceneEditor(pEditor);
		m_lpLogicalData->LoadFromIniFile(str,&ShowProgress,bUseSO3Logical,this);
		if(bUseLogicObject)
		{
			LoadRepresentNPC();
			LoadRepresentDoodad();
			LoadRepresentRefreshPoint();
			LoadRepresentWayPoint();
			LoadRepresentTrafficPoint();
		}
	}
	//)
	g_pProgressBar->ShowWindow(SW_HIDE);
	//////////////////////////
	/*加入打开判断是否运行*/
	//////////////////////////

	if (KSceneEditorDialogCheckRelive::This == NULL)
	{
		KSceneEditorDialogCheckRelive* pDlg = new KSceneEditorDialogCheckRelive;
	}
	IEKG3DSceneSceneEditor*	pSceneSceneEditor = NULL;
	GetScene()->GetSceneSceneEditor(&pSceneSceneEditor);
	KSceneEditorDialogCheckRelive::This->SetSceneEditor(pSceneSceneEditor);
//	KSceneEditorDialogCheckRelive::This->SetCurrentDir();
	KSceneEditorDialogCheckRelive::This->SetDocLogical(this->m_lpLogicalData);/////////
	KSceneEditorDialogCheckRelive::This->Create(KSceneEditorDialogCheckRelive::IDD);
	if (KSceneEditorDialogCheckRelive::GetCheckReliveEnableFromFile())
	{
		KSceneEditorDialogCheckRelive::This->ShowWindow(SW_NORMAL);
		if (!KSceneEditorDialogCheckRelive::This->IsLoseRelive())
		{
			KSceneEditorDialogCheckRelive::This->ShowWindow(SW_HIDE);
		}
	}

	return TRUE;
Exit0:
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// KSceneEditorDoc serialization

void KSceneEditorDoc::Serialize(CArchive& ar)
{
	CString Name = GetPathName();
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}

	// Calling the base class CDocument enables serialization
	//  of the container document's COleClientItem objects.
	//CDocument::Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// KSceneEditorDoc diagnostics

#ifdef _DEBUG
void KSceneEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void KSceneEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// KSceneEditorDoc commands
//
//HRESULT KSceneEditorDoc::SetScene(KSceneSceneEdit *pScene)
//{
//	m_lpEditScene = pScene;
//	return S_OK;
//}


void KSceneEditorDoc::OnCloseDocument() 
{
	// TODO: Add your specialized code here and/or call the base class
	//m_lpEditScene->
	//if (m_lpEditScene->m_pScene != NULL)
	//{
	//	//删除原来的地图
	//	g_pSO3World->DeleteScene(m_lpEditScene->m_pScene, TRUE);
	//	m_lpEditScene->m_pScene = NULL;
	//}
	if(KSceneEditorDialogCheckRelive::This)
		KSceneEditorDialogCheckRelive::This->DestroyWindow();
    HRESULT hrRetCode = NULL;
    IEKG3DScene* pScene = NULL;
    IEKG3DSceneSceneEditor* pEditor = NULL;
    BOOL bUseSO3Logical = FALSE;

    BOOL bAutoDelete = m_bAutoDelete;
	m_bAutoDelete = FALSE;
	CDocument::OnCloseDocument();

    pScene = GetScene();
    KGLOG_PROCESS_ERROR(pScene);

    hrRetCode = pScene->GetSceneSceneEditor(&pEditor);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	pEditor->Unadvise(m_lpLogicalData);

    g_pEngineManager->GetUseSO3Logical(&bUseSO3Logical);
	if (bUseSO3Logical)
	{
		KScene *lpLogicalScene = NULL;
        IEKG3DSceneLogicalEditorBase * pLogicEditor = NULL;

        hrRetCode = pEditor->GetIELogicScene(&pLogicEditor);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);

        hrRetCode = pLogicEditor->GetLogicScene((void **)&lpLogicalScene);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
        KGLOG_PROCESS_ERROR(lpLogicalScene);
        
		BOOL bRetCode = g_pSO3World->DeleteScene(lpLogicalScene);
		KGLOG_PROCESS_ERROR(bRetCode);

		SetScene(NULL);
	}
	else
	{
		g_pEngineManager->RemoveScene(pScene);
		SetScene(NULL);
	}

Exit0:
	if (bAutoDelete)
		delete this;
	return;
}


void KSceneEditorDoc::LoadRepresentNPC()
{
    INT nRetCode = false;
    HRESULT hrRetCode = NULL;
	DWORD dwErrorCount = 0;
    IEKG3DScene* pScene = NULL;
    IEKG3DSceneSceneEditor* pEditor = NULL;
    list<KSceneEditorDocLogical::NPCInfo*>::iterator i;
	BOOL bUseSO3Logical = FALSE;
	g_pEngineManager->GetUseSO3Logical(&bUseSO3Logical);

    if (!bUseSO3Logical)
    {
        return;
    }

    pScene = GetScene();
    KGLOG_PROCESS_ERROR(pScene);

    hrRetCode = pScene->GetSceneSceneEditor(&pEditor);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	int nTotalCount = (int)m_lpLogicalData->m_listNPCInfo.size();
	int nCount = 0;
	i = m_lpLogicalData->m_listNPCInfo.begin();
	while (i!=m_lpLogicalData->m_listNPCInfo.end())
	{
		KSceneEditorDocLogical::NPCInfo* pNpc = *i;

		nCount++;
		ShowProgress("Input NPC",nCount * 1.0F / nTotalCount);

		if(!pNpc->pPointer)
		{
            const KNpcTemplate* pcNpcTempate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(pNpc->nTempleteID);
				
			if (pcNpcTempate && pcNpcTempate->dwTemplateID != 0)
			{
				//const KAIParamTemplate* pcAITempate = g_pSO3World->m_Settings.m_AIParamTemplateList.GetTemplate(pcNpcTempate->nAIParamTemplateID);

				DWORD dwRepresentID = pNpc->dwRepresentID;//NpcTempate.dwRepresentID[0];


                // 因为一个物件对应多个表现ID是后来加的新功能
                // 所以在旧的编辑器保存逻辑信息的ini里没有表现
                // id这个项,这个时候它的值是0,使用第一个表现id
                if (dwRepresentID == 0)
                {
                    dwRepresentID = pcNpcTempate->dwRepresentID[0];

                    //  修复
                    pNpc->dwRepresentID = pcNpcTempate->dwRepresentID[0];
                }

				if(bUseSO3Logical)
				{
					KRepresentNPCEquipResource NPCRes;
					KRepresentNPCAnimationResource AniRes;
					BOOL bNpcExist = TRUE;
					IEKG3DRepresentObject* pObj = NULL;
					TCHAR szMDLName[MAX_PATH];
					TCHAR szAniName[MAX_PATH];
					TCHAR szFaceName[MAX_PATH];
					TCHAR szFaceMtlName[MAX_PATH];
					float fAlertRange = 100;
					float fAttackRange = 100;

					hrRetCode = g_pRepresentHandler->GetNPCEquipResource(dwRepresentID,&NPCRes);
					if(FAILED(hrRetCode))
					{
						KGLogPrintf(KGLOG_ERR, "g_pRepresentHandler->GetNPCEquipResource Failed!\n");
						bNpcExist = FALSE;
					}
					hrRetCode = g_pRepresentHandler->GetNPCAnimationResource(dwRepresentID,24,&AniRes);
					if(FAILED(hrRetCode))
					{
						KGLogPrintf(KGLOG_ERR, "g_pRepresentHandler->GetNPCAnimationResource() Failed!\n");
						bNpcExist = FALSE;
					}

					if(bNpcExist)
					{
						LPCTSTR pMDLName = NPCRes.szMDLFilePath;
						LPCTSTR pAniName = AniRes.szAniFilePath;

						LPCTSTR pFaceMesh = NPCRes.Parts[RL_NPC_SOCKET_FACE].szMeshFilePath;
						LPCTSTR pFaceMtl = NPCRes.Parts[RL_NPC_SOCKET_FACE].szMtlFilePath;

						//aAnimation[MODEL_DOODAD_ANI_BEGIN][0].szMeshFilePath;
						//LPCTSTR pMtlName = NPCRes.aAnimation[MODEL_DOODAD_ANI_BEGIN][0].szMtlFilePath;
						//LPCTSTR pAniName = NPCRes.aAnimation[MODEL_DOODAD_ANI_BEGIN][0].szAniFilePath;
						fAlertRange = (float)(0) * LOGICAL_CELL_CM_LENGTH / CELL_LENGTH;
						fAttackRange = (float)(0) * LOGICAL_CELL_CM_LENGTH / CELL_LENGTH;

						wsprintf(szMDLName,"%s",pMDLName);
						wsprintf(szAniName,"%s",pAniName);
						wsprintf(szFaceName,"%s",pFaceMesh);
						wsprintf(szFaceMtlName,"%s",pFaceMtl);
					}
					else
					{
						wsprintf(szMDLName,"data\\public\\scenerunner\\scenerunner.Mesh");
						wsprintf(szAniName,"data\\public\\scenerunner\\scenerunner.Ani");
						wsprintf(szFaceName,"");
						wsprintf(szFaceMtlName,"");
					}

					//新的载入不需要用放置工具倒腾
					{
						pObj = BuildRepresentNPC(g_GetEngineIEInterface(), pNpc->nTempleteID
							, pcNpcTempate->szName, szMDLName, szAniName, szFaceName, szFaceMtlName
							, pcNpcTempate->nLevel, pcNpcTempate->nKind, /*pcNpcTempate->nAIType,*/ 
							NPCRes.fModelScale, pcNpcTempate->dwRepresentID[0],fAlertRange,fAttackRange,pNpc->dwNpcReliveID,pNpc->dwNpcRandomID,
							pNpc->dwNpcAISet
							);
						if (pObj)
						{
							pObj->SetTranslation(&pNpc->vPosition);
							pObj->SetRotation(&pNpc->vRotation);
							//hrRetCode = pEditor->AddRepresentObject(pObj, TRUE, FALSE,FALSE, TRUE);//生命周期现在由Scene接管了

							ULONG uAddOption = EM_SCENE_ADD_OPTION_SELECTABLE | EM_SCENE_ADD_OPTION_TO_PHYSICS;
							hrRetCode = pEditor->AddRepresentObject(pObj, uAddOption, NULL);
							if (FAILED(hrRetCode))
							{
								if(NULL != pcNpcTempate->szName)
								{
									KGLogPrintf(KGLOG_ERR, "NPC添加到场景失败:%s", pcNpcTempate->szName);
								}
								SAFE_RELEASE(pObj);
							}
							else
							{
								pNpc->pPointer = pObj;
							}

						}
					}
				}
			}	
			else//如查在NPC列表中找不到对应的NPC，则加载一个默认模型（scenerunner.Mesh），使得在辑器中能选中该NPC在场景中删掉。add by huangjinshou 2008-8-1
			{
				IEKG3DRepresentObject* pObj = NULL;
				TCHAR szMDLName[MAX_PATH];
				TCHAR szAniName[MAX_PATH];
				TCHAR szFaceName[MAX_PATH];
				TCHAR szFaceMtlName[MAX_PATH];
				float fAlertRange = 100;
				float fAttackRange = 100;
				wsprintf(szMDLName,"data\\public\\scenerunner\\scenerunner.Mesh");
				wsprintf(szAniName,"");//data\\public\\scenerunner\\scenerunner.Ani
				wsprintf(szFaceName,"");
				wsprintf(szFaceMtlName,"");
				{
					dwErrorCount++;
					pObj = BuildRepresentNPC(g_GetEngineIEInterface(), pNpc->nTempleteID
						, pNpc->szName.c_str(), szMDLName, szAniName, szFaceName, szFaceMtlName
						, pNpc->nLevel, pNpc->nKind, /*pNpc->nAIType, */
						1.0f, pNpc->dwRepresentID,fAlertRange,fAttackRange,pNpc->dwNpcReliveID,pNpc->dwNpcRandomID,pNpc->dwNpcAISet
						);
					if (pObj)
					{
						D3DXVECTOR3 vPos(0,0,0);
						//D3DXQUATERNION vRot;
						//D3DXQuaternionIdentity(&vRot);
						pObj->SetTranslation(&vPos);
						//pObj->SetRotation(&vRot);

						//hrRetCode = pEditor->AddRepresentObject(pObj, TRUE, FALSE,FALSE, TRUE);//生命周期现在由Scene接管了
						ULONG uAddOption = EM_SCENE_ADD_OPTION_SELECTABLE | EM_SCENE_ADD_OPTION_TO_PHYSICS;
						hrRetCode = pEditor->AddRepresentObject(pObj, uAddOption, NULL);
						
						if (FAILED(hrRetCode))
						{
							SAFE_RELEASE(pObj);
						}
						else
						{
							pNpc->pPointer = pObj;
						}

					}
				}
			}
		}
		i++;
	}
Exit0:
	if(dwErrorCount>0)
	{
		TCHAR strError[MAX_PATH];
		wsprintf(strError,"在NPC列表中找不到对应的NPC,有%i个!\n全部被放到地图的（0，0，0）点，请手动删除冇用的NPC",dwErrorCount);
		MessageBox(NULL,strError,"提示",MB_ICONWARNING);
	}
    return;
}

void KSceneEditorDoc::LoadRepresentDoodad()
{
	INT nRetCode = false;
    HRESULT hrRetCode = NULL;
    IEKG3DScene* pScene = NULL;
	DWORD dwErrorCount = 0;
    IEKG3DSceneSceneEditor* pEditor = NULL;
    list<KSceneEditorDocLogical::DoodadInfo*>::iterator i;
	BOOL bUseSO3Logical = FALSE;
	g_pEngineManager->GetUseSO3Logical(&bUseSO3Logical);

    if (!bUseSO3Logical)
    {
        return;
    }

    pScene = GetScene();
    KGLOG_PROCESS_ERROR(pScene);

    hrRetCode = pScene->GetSceneSceneEditor(&pEditor);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	KG_PROCESS_ERROR(bUseSO3Logical);
	int nTotalCount = (int)m_lpLogicalData->m_listDoodadInfo.size();
	int nCount =0;
	i = m_lpLogicalData->m_listDoodadInfo.begin();
	while (i!=m_lpLogicalData->m_listDoodadInfo.end())
	{
		KSceneEditorDocLogical::DoodadInfo* pDoodad = *i;

		const KDoodadTemplate* pcDoodadTemplate = g_pSO3World->m_Settings.m_DoodadTemplateList.GetTemplate(pDoodad->nTempleteID);
		if (pcDoodadTemplate && pcDoodadTemplate->m_dwTemplateID != 0)
		{
			KRepresentDoodadResource DoodadRes;

			nCount++;
			ShowProgress("Input Doodad",nCount * 1.0F / nTotalCount);

			g_pRepresentHandler->GetDoodadResource(pcDoodadTemplate->m_dwRepresentID,&DoodadRes);

			LPCTSTR pMeshName = DoodadRes.aAnimation[0].szMeshFilePath;
			LPCTSTR pMtlName = DoodadRes.aAnimation[0].szMtlFilePath;
			LPCTSTR pAniName = DoodadRes.aAnimation[0].szAniFilePath;

			TCHAR szMeshName[MAX_PATH];
			TCHAR szMtlName[MAX_PATH];
			TCHAR szAniName[MAX_PATH];
			wsprintf(szMeshName,"%s",pMeshName);
			wsprintf(szMtlName,"%s",pMtlName);
			wsprintf(szAniName,"%s",pAniName);
			//新的方式，直接Add，不需要用放置工具倒腾
			{
				IEKG3DRepresentObject* pObj = BuildRepresentDoodad(g_GetEngineIEInterface()
					, pDoodad->nTempleteID, pcDoodadTemplate->m_szName
					, szMeshName, szMtlName, szAniName,pDoodad->dwDoodadReliveID
					, pDoodad->bUseDefaultObstacle, pDoodad->bGenerateObstacle);

				if(pObj)
				{
					pObj->SetTranslation(&pDoodad->vPosition);
					pObj->SetRotation(&pDoodad->vRotation);
					//hrRetCode = pEditor->AddRepresentObject(pObj, TRUE, FALSE,FALSE, FALSE);
					hrRetCode = pEditor->AddRepresentObject(pObj, EM_SCENE_ADD_OPTION_SELECTABLE, NULL);
					if (FAILED(hrRetCode))
					{
						SAFE_RELEASE(pObj);
					}
					else
					{
						pDoodad->pPointer = pObj;
					}					
				}
				
			}
		}
		else//如果在Doodad列表中找不到对应的Doodad，则加载一个默认模型（scenerunner.Mesh），使得在辑器中能选中该Doodad在场景中删掉。add by huangjinshou 2008-8-1
		{
			TCHAR szMeshName[MAX_PATH];
			TCHAR szMtlName[MAX_PATH];
			TCHAR szAniName[MAX_PATH];
			wsprintf(szMeshName,"data\\public\\scenerunner\\scenerunner.Mesh");
			wsprintf(szMtlName,"");
			wsprintf(szAniName,"");//data\\public\\scenerunner\\scenerunner.Ani
			//新的方式，直接Add，不需要用放置工具倒腾
			{
				IEKG3DRepresentObject* pObj = BuildRepresentDoodad(g_GetEngineIEInterface()
					, pDoodad->nTempleteID, pDoodad->szName.c_str()
					, szMeshName, szMtlName, szAniName,pDoodad->dwDoodadReliveID
					, pDoodad->bUseDefaultObstacle, pDoodad->bGenerateObstacle);
				dwErrorCount++;
				if(pObj)
				{
					D3DXVECTOR3 vPos(0,0,0);
					pObj->SetTranslation(&vPos);
				//	pObj->SetRotation(&pDoodad->vRotation);
					//hrRetCode = pEditor->AddRepresentObject(pObj, TRUE, FALSE,FALSE, FALSE);
					hrRetCode = pEditor->AddRepresentObject(pObj, EM_SCENE_ADD_OPTION_SELECTABLE, NULL);
					if (FAILED(hrRetCode))
					{
						SAFE_RELEASE(pObj);
					}
					else
					{
						pDoodad->pPointer = pObj;
					}					
				}

			}
		}
		i++;
	}
Exit0:
	if(dwErrorCount>0)
	{
		TCHAR strError[MAX_PATH];
		wsprintf(strError,"在Doodad列表中找不到对应的Doodad,有%i个!\n全部被放到地图的（0，0，0）点，请手动删除冇用的Doodad",dwErrorCount);
		MessageBox(NULL,strError,"提示",MB_ICONWARNING);
	}
    return;
}

void KSceneEditorDoc::LoadRepresentWayPoint()
{
    HRESULT hrRetCode = NULL;
    IEKG3DScene* pScene = NULL;
    IEKG3DSceneSceneEditor* pEditor = NULL;
    map<DWORD,KSceneEditorDocLogical::WayPointSet*>::iterator i;
	BOOL bUseSO3Logical = FALSE;
	g_pEngineManager->GetUseSO3Logical(&bUseSO3Logical);

    pScene = GetScene();
    KGLOG_PROCESS_ERROR(pScene);

    hrRetCode = pScene->GetSceneSceneEditor(&pEditor);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	KG_PROCESS_ERROR(bUseSO3Logical);

	int nTotalCount = (int)m_lpLogicalData->m_mapWayPointSet.size();
	int nCount =0;

	i = m_lpLogicalData->m_mapWayPointSet.begin();
	while (i!=m_lpLogicalData->m_mapWayPointSet.end())
	{
		KSceneEditorDocLogical::WayPointSet* pSet = i->second;

		nCount++;
		ShowProgress("Input WayPointSet",nCount * 1.0F / nTotalCount);

		for (size_t s=0;s<pSet->vectorWayPoint.size();s++)
		{
			KSceneEditorDocLogical::WayPoint* pPoint = pSet->vectorWayPoint[s];

			DWORD dwSetID = pSet->dwSetID;

            IEKG3DRepresentObject* pObj = BuildWayPoint(g_GetEngineIEInterface(), dwSetID, _T("WayPoint"));
			if (pObj)
			{
				pObj->SetTranslation(&pPoint->vPosition);
				pObj->SetRotation(&pPoint->vRotation);

				hrRetCode = pEditor->AddRepresentObject(pObj, EM_SCENE_ADD_OPTION_SELECTABLE, NULL);
				if (FAILED(hrRetCode))
				{
					SAFE_RELEASE(pObj);
				}
				else
				{
					pPoint->pPointer = pObj;
				}
			}
		}
		i++;
	}
Exit0:
    return;
}

void KSceneEditorDoc::LoadRepresentTrafficPoint()
{
	HRESULT hrRetCode = NULL;
	IEKG3DScene* pScene = NULL;
	IEKG3DSceneSceneEditor* pEditor = NULL;
	list<KSceneEditorDocLogical::TrafficPointSet*>::iterator i;
	map<DWORD,KSceneEditorDocLogical::TrafficPoint>::iterator iPoint;
	BOOL bUseSO3Logical = FALSE;
	g_pEngineManager->GetUseSO3Logical(&bUseSO3Logical);

	pScene = GetScene();
	KGLOG_PROCESS_ERROR(pScene);

	hrRetCode = pScene->GetSceneSceneEditor(&pEditor);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);
	
	KG_PROCESS_ERROR(bUseSO3Logical);

	KGLOG_PROCESS_ERROR(m_lpLogicalData);
	int nTotalCount = (int)m_lpLogicalData->m_listTrafficPointSet.size();
	int nCount =0;


	i = m_lpLogicalData->m_listTrafficPointSet.begin();
	while (i!=m_lpLogicalData->m_listTrafficPointSet.end())
	{
		KSceneEditorDocLogical::TrafficPointSet* pSet = *i;

		nCount++;
		ShowProgress("Input TrafficPointSet",nCount * 1.0F / nTotalCount);

		for (size_t s=0;s<pSet->vecTrafficPoint.size();s++)
		{
			KSceneEditorDocLogical::TrafficLittlePoint* pPoint = pSet->vecTrafficPoint[s];
			IEKG3DRepresentObject* pObj = BuildTrafficPoint(g_GetEngineIEInterface(), pSet->m_dwID, 1, _T("TrafficPoint"));
			if (pObj)
			{
				pObj->SetTranslation(&pPoint->vPostion);
				//hrRetCode = pEditor->AddRepresentObject(pObj, TRUE, FALSE,FALSE,FALSE);
				hrRetCode = pEditor->AddRepresentObject(pObj, EM_SCENE_ADD_OPTION_SELECTABLE, NULL);
				if (FAILED(hrRetCode))
				{
					SAFE_RELEASE(pObj);
				}
				else
				{
					pPoint->pPointer = pObj;
				}
				
			}
		}
		i++;
	}
	nTotalCount = (int)m_lpLogicalData->m_mapTrafficPoint.size();
	nCount =0;

	iPoint = m_lpLogicalData->m_mapTrafficPoint.begin();
	while (iPoint != m_lpLogicalData->m_mapTrafficPoint.end())
	{
		//KSceneEditorDocLogical::TrafficPoint* pPoint = i;

		nCount++;
		ShowProgress("Input TrafficPoint",nCount * 1.0F / nTotalCount);
		
		KSceneEditorDocLogical::TrafficPoint& Point = iPoint->second;
		
		IEKG3DRepresentObject* pObj = BuildTrafficPoint(g_GetEngineIEInterface(), iPoint->first, 0, _T("TrafficPoint"));
	
		if (pObj)
		{
			pObj->SetTranslation(&Point.vPosition);
			//hrRetCode = pEditor->AddRepresentObject(pObj, TRUE, FALSE,FALSE,FALSE);
			hrRetCode = pEditor->AddRepresentObject(pObj, EM_SCENE_ADD_OPTION_SELECTABLE, NULL);
			if (FAILED(hrRetCode))
			{
				SAFE_RELEASE(pObj);
			}
			else
			{
				Point.pPointer = pObj;
			}
		}

		iPoint++;
	}

Exit0:
	return;
}

void KSceneEditorDoc::LoadRepresentRefreshPoint()
{
    HRESULT hrRetCode = NULL;
    IEKG3DScene* pScene = NULL;
    IEKG3DSceneSceneEditor* pEditor = NULL;
	BOOL bUseSO3Logical = FALSE;
	g_pEngineManager->GetUseSO3Logical(&bUseSO3Logical);

    pScene = GetScene();
    KGLOG_PROCESS_ERROR(pScene);

    hrRetCode = pScene->GetSceneSceneEditor(&pEditor);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	KG_PROCESS_ERROR(bUseSO3Logical);
	
Exit0:
    return;
}

void KSceneEditorDoc::BeforeChange()
{
	//////////////////////////////////////////////////////////////////////////
	{
		//DWORD dwCount = 0;
		//list<KSceneEditorDocLogical::WayPointSet*>::iterator i = m_lpLogicalData->m_listWayPointSet.begin();
		//while (i!=m_lpLogicalData->m_listWayPointSet.end())
		//{
		//	KSceneEditorDocLogical::WayPointSet* pPath = *i;

		//	pPath->nIndex = dwCount+1;

		//	i++;
		//	dwCount++;
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_lpLogicalData->m_listNPCInfo.begin();
		while (i!=m_lpLogicalData->m_listNPCInfo.end())
		{
			KSceneEditorDocLogical::NPCInfo* pNpc = *i;

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

HRESULT KSceneEditorDoc::ClearAllTrap()
{
	BOOL bRetCode = FALSE;

	BOOL bUseSO3Logic = FALSE;
	HRESULT hr = g_pEngineManager->GetUseSO3Logical(&bUseSO3Logic);
	KGLOG_COM_PROCESS_ERROR(hr);
	if(bUseSO3Logic)
	{
		KScene *lpLogicalScene = NULL;
		GETSCENE() FALSE; GETEDITOR() FALSE; GETLOGIC() FALSE;
		pLogic->GetLogicScene((LPVOID*)&lpLogicalScene);
		KGLOG_PROCESS_ERROR(lpLogicalScene);

		bRetCode = lpLogicalScene->ClearAllScript();
		KGLOG_PROCESS_ERROR(bRetCode);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

void KSceneEditorDoc::ExchangeNpc(KScene *pLogicalScene)
{
	BOOL bRetCode = FALSE;
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strFile[MAX_PATH];
	//TCHAR strPreloadNpcFilePath[MAX_PATH];
	//std::set<unsigned int> setUseNpcModels;

	_splitpath_s(m_strPathName, 
		strDriver,
		MAX_PATH,
		strPath,
		MAX_PATH,
		strFile,
		MAX_PATH,
		NULL,
		0);

	/*sprintf_s(strPreloadNpcFilePath, 
		MAX_PATH,
		"%s%s%spreload.dat",
		strDriver,
		strPath,
		strFile);*/

	//FILE *fpPreLoadNpc = fopen(strPreloadNpcFilePath, "wb");
	//KG_PROCESS_ERROR(fpPreLoadNpc);

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
			//KRepresentNPCEquipResource NPCRes;
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
            npcData.nProgressID         = pNpcInfo->nProgressID;

			if(pNpcInfo->pWayPointSet)
				npcData.nPatrolPathID   = pNpcInfo->pWayPointSet->nPatrolPathOrderID;
			else
				npcData.nOrderIndex     = 0;

			if(pNpcInfo->dwNpcAISet)
			{
				npcData.dwAIType        = (pLogicalScene->m_dwMapID<<24) | (pNpcInfo->dwNpcAISet);
			}
			else
				npcData.dwAIType        = 0;
			
            npcData.dwRepresentID       = pNpcInfo->dwRepresentID;

            // 指定表现ID后，随机NPC的模型就不会变了。
            if (pNpcInfo->dwNpcRandomID != 0)
            {
                npcData.dwRepresentID = 0;
            }

            strncpy(npcData.szNickName, pNpcInfo->szNickName.c_str(), sizeof(npcData.szNickName));
            npcData.szNickName[sizeof(npcData.szNickName) - 1] = '\0';

            piNpcFile->Push(npcData);

			nCount++;

			/*g_pRepresentHandler->GetNPCEquipResource(pNpcInfo->dwRepresentID, &NPCRes);
			unsigned int uHash = g_FileNameHash(NPCRes.szMDLFilePath);
			if (!setUseNpcModels.count(uHash))
			{
				setUseNpcModels.insert(uHash);
				TCHAR strOutput[MAX_PATH];
				strcpy_s(strOutput, MAX_PATH, NPCRes.szMDLFilePath);
				fwrite(strOutput, MAX_PATH, 1, fpPreLoadNpc);
			}*/

            if (nCount % 16 == 0)
            {
			    ShowProgress("Exchange NPC",nCount*1.0F/nTotalCount);
            }

			++it;
		}
	}

    piNpcFile->Save(pLogicalScene->m_szName);

	/*if (fpPreLoadNpc)
	{
		fclose(fpPreLoadNpc);
		fpPreLoadNpc = NULL;
	}*/
    KG_COM_RELEASE(piNpcFile);
	return ;

Exit0:
	/*if (fpPreLoadNpc)
	{
		fclose(fpPreLoadNpc);
		fpPreLoadNpc = NULL;
	}*/
    KG_COM_RELEASE(piNpcFile);
	return ;
}

void KSceneEditorDoc::ExchangeDoodad(KScene *pLogicalScene)
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
		WORD nObstacleDoodadCount = 0;
		list<int> lsDoodadExceeded;
#define MAX_DOODAD_OBSTACLE 4000

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
            DoodadData.dwScriptID       = pDoodadInfo->dwScriptID;
            DoodadData.nProgressID      = pDoodadInfo->nProgressID;
			// doodad加nickname
			strncpy(DoodadData.szNickName, pDoodadInfo->szNickName.c_str(), sizeof(DoodadData.szNickName));
			DoodadData.szNickName[sizeof(DoodadData.szNickName) - 1] = '\0';
            // 动态障碍
			if (pDoodadInfo->bUseDefaultObstacle)
			{
				const KDoodadTemplate* pcDoodadTemplate = g_pSO3World->m_Settings.m_DoodadTemplateList.GetTemplate(pDoodadInfo->nTempleteID);
				if (pcDoodadTemplate && pcDoodadTemplate->m_dwTemplateID != 0 && pcDoodadTemplate->m_bDynamicObstacle)
					DoodadData.wObstacleGroup = ++nObstacleDoodadCount;
				else
					DoodadData.wObstacleGroup = 0;
			}
			else
			{
				DoodadData.wObstacleGroup = pDoodadInfo->bGenerateObstacle ? ++nObstacleDoodadCount : 0;
			}
			if (DoodadData.wObstacleGroup > MAX_DOODAD_OBSTACLE - 1)
			{
				DoodadData.wObstacleGroup = 0;
				lsDoodadExceeded.push_back(pDoodadInfo->nIndex);
			}

            piDoodadFile->Push(DoodadData);

			nCount++;
            if (nCount % 8 == 0)
            {
			    ShowProgress("Exchange Doodad",nCount*1.0F/nTotalCount);
            }

			++it;
		}

		// 如果有超过4000个有障碍的doodad，要msgbox一下
		if (lsDoodadExceeded.size() > 0)
		{
			TCHAR msg[4196];
			TCHAR buf[16];
			msg[0] = '\0';
			buf[0] = '\0';
			sprintf(msg, "以下nIndex的doodad无法保存动态障碍，已经超过本场景上限%d。\r\n", MAX_DOODAD_OBSTACLE);
			for (list<int>::iterator iter = lsDoodadExceeded.begin(); iter != lsDoodadExceeded.end(); iter++)
			{
				itoa(*iter, buf, 10);
				strcat(msg, buf);
				strcat(msg, " ");
			}

			MessageBox(NULL, msg, "未保存全部", 0);
			lsDoodadExceeded.clear();
		}
	}

    piDoodadFile->Save(pLogicalScene->m_szName);

    KG_COM_RELEASE(piDoodadFile);
	return ;

Exit0:
    KG_COM_RELEASE(piDoodadFile);
	return ;
}

void KSceneEditorDoc::CalcDoodadObstacleGroupIdIntoRepresentObject()
{
	WORD nObstacleDoodadCount = 0;
#define MAX_DOODAD_OBSTACLE 4000

	list<KSceneEditorDocLogical::DoodadInfo*>::iterator it = m_lpLogicalData->m_listDoodadInfo.begin();
	while (it != m_lpLogicalData->m_listDoodadInfo.end())
	{
		KSceneEditorDocLogical::DoodadInfo* pDoodadInfo = *it;
		int iObstacleGroupIndex = INVALID_OBSTACLE_GROUP;
		// 动态障碍
		if (pDoodadInfo->bUseDefaultObstacle)
		{
			const KDoodadTemplate* pcDoodadTemplate = g_pSO3World->m_Settings.m_DoodadTemplateList.GetTemplate(pDoodadInfo->nTempleteID);
			if (pcDoodadTemplate && pcDoodadTemplate->m_dwTemplateID != 0 && pcDoodadTemplate->m_bDynamicObstacle)
				iObstacleGroupIndex = ++nObstacleDoodadCount;
			else
				iObstacleGroupIndex = INVALID_OBSTACLE_GROUP;
		}
		else
		{
			iObstacleGroupIndex = pDoodadInfo->bGenerateObstacle ? ++nObstacleDoodadCount : INVALID_OBSTACLE_GROUP;
		}
		if (iObstacleGroupIndex > MAX_DOODAD_OBSTACLE - 1)
		{
			iObstacleGroupIndex = INVALID_OBSTACLE_GROUP;
		}
		// 算完放到representobject里
		IEKG3DRepresentObject* ro = static_cast<IEKG3DRepresentObject*>(pDoodadInfo->pPointer);
		if(NULL != ro)
		{
			ro->SetDynamicObstacleGroupIndex(iObstacleGroupIndex);
		}

		++it;
	}
}

void KSceneEditorDoc::ExchaneNpcRefreshPointSet(LPCTSTR pFileName,LPCTSTR pMapName)
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

void KSceneEditorDoc::ExchangeLogicalData(LPCTSTR pFileName)
{
    HRESULT hrRetCode = NULL;
    IEKG3DScene* pScene = NULL;
    IEKG3DSceneSceneEditor* pEditor = NULL;
    IEKG3DSceneLogicalEditorBase * pLogicEditor = NULL;
    KScene *lpLogicalScene = NULL;

    pScene = GetScene();
    KGLOG_PROCESS_ERROR(pScene);

    hrRetCode = pScene->GetSceneSceneEditor(&pEditor);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = pEditor->GetIELogicScene(&pLogicEditor);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = pLogicEditor->GetLogicScene((void **)&lpLogicalScene);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    KGLOG_PROCESS_ERROR(lpLogicalScene);

	BeforeChange();

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
	TCHAR szAIParameterDir[MAX_PATH];
	wsprintf(szMapDir,"%s%s\\map\\%s\\",g_szDefWorkDirectory,SETTING_DIR,fname);
	wsprintf(szDoodadReliveDir,"%s%s\\map\\%s\\DoodadRelive\\",g_szDefWorkDirectory,SETTING_DIR,fname);
	wsprintf(szNpcReliveDir,"%s%s\\map\\%s\\NpcRelive\\",g_szDefWorkDirectory,SETTING_DIR,fname);
	wsprintf(szPatrolPathDir,"%s%s\\map\\%s\\PatrolPath\\",g_szDefWorkDirectory,SETTING_DIR,fname);
	wsprintf(szNpcTeamDir,"%s%s\\map\\%s\\NpcTeam\\",g_szDefWorkDirectory,SETTING_DIR,fname);
	wsprintf(szRoadDir,"%s%s\\Road\\",g_szDefWorkDirectory,SETTING_DIR);
	wsprintf(szAIParameterDir,"%s%s\\map\\%s\\NpcAIParameter\\",g_szDefWorkDirectory,SETTING_DIR,fname);
	mkdir(szMapDir);
	mkdir(szDoodadReliveDir);
	mkdir(szNpcReliveDir);
	mkdir(szPatrolPathDir);
	mkdir(szNpcTeamDir);
	mkdir(szRoadDir);
	mkdir(szAIParameterDir);

	lpLogicalScene->DeleteAllObjects();

	m_lpLogicalData->UpdataNpcAIParameters(m_lpLogicalData->m_bUseNpcWayPointSetPointer);
	m_lpLogicalData->ResortNpcAISet();

	ExchangeNpc(lpLogicalScene);

	ExchangeDoodad(lpLogicalScene);

	// wsprintf(Name,"map\\%s\\NpcRelive\\",fname);
	// ExchaneNpcRefreshPointSet(Name,fname);

	// wsprintf(Name,"map\\%s\\DoodadRelive\\",fname);
	// ExchaneDoodadRefreshPointSet(Name,fname);

	wsprintf(Name,"map\\%s\\NpcTeam\\",fname);
	ExchangeAIGroup(Name,fname);

	wsprintf(Name,"map\\%s\\PatrolPath\\",fname);
	ExchangeWayPointSet(Name,fname);

	ExchangeTrafficPointSet(szRoadDir);
	SetCurrentDirectory(g_szDefWorkDirectory);
	lpLogicalScene->Save(szFileName,&ShowProgress);
	//////////////////////////////////////////////////////////////////////////
	TCHAR szNpcReliveFile[MAX_PATH];
	wsprintf(szNpcReliveFile,"%s%s\\map\\%s\\NpcRelive\\MapReviveList.tab",g_szDefWorkDirectory,SETTING_DIR,fname);
	ExchangeNpcReliveSetting(szNpcReliveFile);

	TCHAR szDoodadReliveFile[MAX_PATH];
	wsprintf(szDoodadReliveFile,"%s%s\\map\\%s\\DoodadRelive\\DoodadReviveList.tab",g_szDefWorkDirectory,SETTING_DIR,fname);
	ExchangeDoodadReliveSetting(szDoodadReliveFile);

	TCHAR szNpcRandomFile[MAX_PATH];
	wsprintf(szNpcRandomFile,"%s%s\\map\\%s\\NpcRelive\\NpcRandomList.tab",g_szDefWorkDirectory,SETTING_DIR,fname);
	ExchangeNpcRandomSetting(szNpcRandomFile,szNpcReliveDir);

	TCHAR szNpcAISETFile[MAX_PATH];
	wsprintf(szNpcAISETFile,"%s%s\\map\\%s\\NpcAIParameter\\NpcAIParameterList.tab",g_szDefWorkDirectory,SETTING_DIR,fname);
	m_lpLogicalData->ExchangeNpcAISet(lpLogicalScene->m_dwMapID,szNpcAISETFile);

	;
Exit0:
	;
}

HRESULT KSceneEditorDoc::ExchangeNpcReliveSetting(LPTSTR pFileName)
{
	ITabFile* pFile = NULL;
	KGLOG_PROCESS_ERROR(pFileName);
	pFile = g_OpenTabFile(pFileName,true,true);
	KGLOG_PROCESS_ERROR(pFile);

    pFile->Clear();

	pFile->InsertNewCol("ReviveID");
	pFile->InsertNewCol("Min");
	pFile->InsertNewCol("Max");
	pFile->InsertNewCol("IsRandom");

	m_lpLogicalData->ExchangeNpcReviveGroups(pFile);

	pFile->Save(pFileName);

	return S_OK;
Exit0:
	SAFE_RELEASE(pFile);
	return E_FAIL;
}

HRESULT KSceneEditorDoc::ExchangeDoodadReliveSetting(LPTSTR pFileName)
{
	ITabFile* pFile = NULL;
	KGLOG_PROCESS_ERROR(pFileName);
	pFile = g_OpenTabFile(pFileName,true,true);
	KGLOG_PROCESS_ERROR(pFile);

    pFile->Clear();

	pFile->InsertNewCol("ReviveID");
	pFile->InsertNewCol("Min");
	pFile->InsertNewCol("Max");
	pFile->InsertNewCol("IsRandom");

	m_lpLogicalData->ExchangeDoodadReviveGroups(pFile);

	pFile->Save(pFileName);

	return S_OK;
Exit0:
	SAFE_RELEASE(pFile);
	return E_FAIL;
}

HRESULT KSceneEditorDoc::ExchangeNpcRandomSetting(LPTSTR pFileName,LPTSTR pDir)
{
	ITabFile* pFile = NULL;
	KGLOG_PROCESS_ERROR(pFileName);
	pFile = g_OpenTabFile(pFileName,true,true);
	KGLOG_PROCESS_ERROR(pFile);

    pFile->Clear();

	pFile->InsertNewCol("RandomID");
	pFile->InsertNewCol("FileName");

	m_lpLogicalData->ExchangeNpcRandomGroups(pFile,pDir);

	pFile->Save(pFileName);

	return S_OK;
Exit0:
	SAFE_RELEASE(pFile);
	return E_FAIL;
}

void KSceneEditorDoc::ExchaneDoodadRefreshPointSet(LPCTSTR pFileName,LPCTSTR pMapName)
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

void KSceneEditorDoc::ExchangeAIGroup(LPCTSTR pFileName,LPCTSTR pMapName)
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
		//pTeam = g_pSO3World->m_Settings.m_NpcTeamList.GetNpcTeam((DWORD)m_lpLogicalData->m_nSceneID,dwCount+1);

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

void KSceneEditorDoc::ImportOldWayPointSet()
{
	DWORD dwMapID = (DWORD)m_LogicalConnection.m_lpLogicalScene->m_dwMapID;

	g_pSO3World->m_Settings.m_PatrolPathList.UnInit();
	g_pSO3World->m_Settings.m_PatrolPathList.Load(dwMapID);

	int nCount = g_pSO3World->m_Settings.m_PatrolPathList.GetSize();
	for (int i=0;i<nCount;i++)
	{
		KPatrolPath* pLogicalPath = g_pSO3World->m_Settings.m_PatrolPathList.GetPatrolPath(dwMapID,i+1);
		KSceneEditorDocLogical::WayPointSet* pWayPointSet = m_lpLogicalData->FindWayPointSetByIndex(i+1);
		if(pWayPointSet && pLogicalPath)
		{
			pWayPointSet->nPatrolPathOrderID = pLogicalPath->m_nOrderID;
			pWayPointSet->nPatrolPathWalkSpeed = pLogicalPath->m_nWalkSpeed;
		}
	}

}

void KSceneEditorDoc::ExchangeWayPointSet(LPCTSTR pFileName,LPCTSTR pMapName)
{
	int nTotalCount = (int)m_lpLogicalData->m_mapWayPointSet.size();
	int nCount = 0;
	DWORD dwCount = 0;

	//////////////////////////////////////////////////////////////////////////
	m_lpLogicalData->SortWayPointSet();
	//////////////////////////////////////////////////////////////////////////
	g_pSO3World->m_Settings.m_PatrolPathList.UnInit();
	map<DWORD,KSceneEditorDocLogical::WayPointSet*>::iterator i = m_lpLogicalData->m_mapWayPointSet.begin();
	while (i!=m_lpLogicalData->m_mapWayPointSet.end())
	{
		KSceneEditorDocLogical::WayPointSet* pSet = i->second;

		nCount++;
		ShowProgress("Exchange WayPointSet",nCount*1.0F/nTotalCount);

		TCHAR Name[MAX_PATH];
		wsprintf(Name,"%sPatrolPathList%d.Tab",pFileName,pSet->nIndex-1);
		
		KPatrolPath* pPath = g_pSO3World->m_Settings.m_PatrolPathList.AddPatrolPath((DWORD)m_lpLogicalData->m_nSceneID, pSet->nIndex, Name);
		pPath->m_nOrderID = pSet->nPatrolPathOrderID;
		pPath->m_nWalkSpeed = pSet->nPatrolPathWalkSpeed;

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

void KSceneEditorDoc::ExchangeTrafficPointSet(LPCTSTR pFileName)
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
		pNode->eCamp = *((KCAMP*)(&((iter->second).m_dwForceID)));
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
			m_lpLogicalData->SceneToLogical(TrackPoint.nX,TrackPoint.nY,TrackPoint.nZ,(pSet->vecTrafficPoint[s])->vPostion,0);
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

void KSceneEditorDoc::OnPhysiscsetting()
{
	HRESULT hrRetCode = S_OK;
	IEKG3DScene* pScene = NULL;
	IEKG3DSceneSceneEditor* pEditor = NULL;
	IEKG3DSceneLogicalEditorBase * pLogicEditor = NULL;

	pScene = GetScene();
	KGLOG_PROCESS_ERROR(pScene);

	hrRetCode = pScene->GetSceneSceneEditor(&pEditor);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);


	if(!m_lpPhysiscSetting)
	{
		m_lpPhysiscSetting = new KGPhysicsParamEditorDlg;
		KGLOG_PROCESS_ERROR(m_lpPhysiscSetting);
		m_lpPhysiscSetting->Create(KGPhysicsParamEditorDlg::IDD);
	}

	m_lpPhysiscSetting->SetSceneEditor(pScene);


	m_lpPhysiscSetting->ShowWindow(TRUE);
Exit0:

	;
}

void KSceneEditorDoc::OnClearalltrap()
{
	ClearAllTrap();
}

int ShowProgress(LPCTSTR pName,float fPencent)
{
	TCHAR szMessage[MAX_PATH];

	snprintf(szMessage, sizeof(szMessage), "%s",pName);
	szMessage[sizeof(szMessage) - 1] = '\0';
	g_pProgressBar->SetWindowText(pName);
	g_pProgressBar->m_staticModel.SetWindowText(szMessage);
	g_pProgressBar->m_progressModel.SetPos((int)(fPencent * 100) );

	return 0;
}



BOOL KLogicalSceneConnection::CheckObstacle(int nXCell, int nYCell, int nLayer)
{
	if(m_lpLogicalScene)
		return m_lpLogicalScene->CheckObstacle(nXCell,nYCell,nLayer);
	else
		return FALSE;
}


KRegion* KLogicalSceneConnection::GetRegion(int nRegionX, int nRegionY)
{
	if(m_lpLogicalScene)
		return m_lpLogicalScene->GetRegion(nRegionX,nRegionY);
	else
		return NULL;
}

HRESULT KLogicalSceneConnection::CreateLogicalRegionInfo()
{
	SAFE_DELETE_ARRAY(m_lpLogicalRegionInfo);

	int nX = m_lpLogicalScene->GetRegionWidth();
	int nY = m_lpLogicalScene->GetRegionHeight();

	m_lpLogicalRegionInfo = new _KG3DLogicalRegionInfo[nX * nY];
	KGLOG_PROCESS_ERROR(m_lpLogicalRegionInfo);

	ZeroMemory(m_lpLogicalRegionInfo,sizeof(_KG3DLogicalRegionInfo) * nX * nY);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KLogicalSceneConnection::FillLogicalCellsInfo( D3DXVECTOR3 A,int X,int Z,int& nCellCount,KCell* pCell,
													  IELogicalVertex* pLLine, IELogicalVertex* pLFace, BOOL bTerrian,BOOL bItem, BOOL bSelected, int nMaxCount )
{


	if(nCellCount >= nMaxCount)
		return E_FAIL;

	float xx = A.x + X * LOGICAL_CELL_CM_LENGTH;
	float zz = A.z + Z * LOGICAL_CELL_CM_LENGTH;
	float y0 = A.y + pCell->m_wLowLayer * (float)ALTITUDE_UNIT + TERRAIN_MIN_HEIGHT;
	y0 = max(-200,y0);
	float y1 = A.y + pCell->m_wHighLayer * (float)ALTITUDE_UNIT+ TERRAIN_MIN_HEIGHT + 1;
	DWORD c3 = 0xFFFFFFFF;
	DWORD c1 = 0xA0FF00FF;
	DWORD c2 = 0xA0FF0000;

	AABBOX Box;
	Box.AddPosition(D3DXVECTOR3(xx,y0,zz));
	Box.AddPosition(D3DXVECTOR3(xx + LOGICAL_CELL_CM_LENGTH, y1, zz + LOGICAL_CELL_CM_LENGTH));
	D3DXVECTOR3 BoxPos[8];
	Box.Extract(BoxPos);

	if (bTerrian)
	{
		// Line
		int nCellCountBase = nCellCount*8;
		pLLine[nCellCountBase+0].posi = BoxPos[BOX_A2];pLLine[nCellCountBase+1].posi = BoxPos[BOX_A1];pLLine[nCellCountBase+0].color=c1;pLLine[nCellCountBase+1].color=c3;
		pLLine[nCellCountBase+2].posi = BoxPos[BOX_B2];pLLine[nCellCountBase+3].posi = BoxPos[BOX_B1];pLLine[nCellCountBase+2].color=c1;pLLine[nCellCountBase+3].color=c3;
		pLLine[nCellCountBase+4].posi = BoxPos[BOX_C2];pLLine[nCellCountBase+5].posi = BoxPos[BOX_C1];pLLine[nCellCountBase+4].color=c1;pLLine[nCellCountBase+5].color=c3;
		pLLine[nCellCountBase+6].posi = BoxPos[BOX_D2];pLLine[nCellCountBase+7].posi = BoxPos[BOX_D1];pLLine[nCellCountBase+6].color=c1;pLLine[nCellCountBase+7].color=c3;

		int nCellCount12 = nCellCount * 12; 
		// Face
		pLFace[nCellCount12 + 0].posi = BoxPos[BOX_A1];
		pLFace[nCellCount12 + 1].posi = BoxPos[BOX_C1];
		pLFace[nCellCount12 + 2].posi = BoxPos[BOX_B1];

		pLFace[nCellCount12 + 3].posi = BoxPos[BOX_A1];
		pLFace[nCellCount12 + 4].posi = BoxPos[BOX_D1];
		pLFace[nCellCount12 + 5].posi = BoxPos[BOX_C1];

		pLFace[nCellCount12 + 6].posi = BoxPos[BOX_A2];
		pLFace[nCellCount12 + 7].posi = BoxPos[BOX_C2];
		pLFace[nCellCount12 + 8].posi = BoxPos[BOX_B2];

		pLFace[nCellCount12 + 9].posi = BoxPos[BOX_A2];
		pLFace[nCellCount12 + 10].posi = BoxPos[BOX_D2];
		pLFace[nCellCount12 + 11].posi = BoxPos[BOX_C2];

		pLFace[nCellCount12 + 0].color = c1;
		pLFace[nCellCount12 + 1].color = c1;
		pLFace[nCellCount12 + 2].color = c1;
		pLFace[nCellCount12 + 3].color = c1;
		pLFace[nCellCount12 + 4].color = c1;
		pLFace[nCellCount12 + 5].color = c1;
		pLFace[nCellCount12 + 6].color = c2;
		pLFace[nCellCount12 + 7].color = c2;
		pLFace[nCellCount12 + 8].color = c2;
		pLFace[nCellCount12 + 9].color = c2;
		pLFace[nCellCount12 + 10].color = c2;
		pLFace[nCellCount12 + 11].color = c2;

		nCellCount++;
	}

	if (bItem && pCell->m_pNext)
	{
		FillLogicalCellsInfo(
			A, X, Z, nCellCount, pCell->m_pNext, pLLine, pLFace, TRUE, TRUE, FALSE, nMaxCount);
	}

	return S_OK;
}

FLOAT KLogicalSceneConnection::GetLogicalHeight( const D3DXVECTOR3& vLogicalCoord, size_t* pOrderOfCell )
{
	POINT regionIndex = LogicalToLogicalRegionIndex(vLogicalCoord.x, vLogicalCoord.y);
	KRegion* pRegion = this->GetRegion(regionIndex.x, regionIndex.y);
	KG_PROCESS_ERROR(NULL != pRegion);
	{
		INT nCellIndexLocalX = (INT)(vLogicalCoord.x / CELL_LENGTH) % REGION_GRID_WIDTH;
		INT nCellIndexLocalY = (INT)(vLogicalCoord.y / CELL_LENGTH) % REGION_GRID_HEIGHT;
		KCell* pCell = GetCurOrLowerObstacle(*pRegion, nCellIndexLocalX, nCellIndexLocalY, (INT)vLogicalCoord.z, pOrderOfCell);
		KG_PROCESS_ERROR(NULL != pCell);

		return (FLOAT)(pCell->m_wHighLayer * POINT_PER_ALTITUDE);
	}
Exit0:
	return -FLT_MAX;
}
KCell* KLogicalSceneConnection::GetNextCell(const KCell* pCell, BOOL bIgnoreDynamicSwitch)
{
	KCell*  pCellNode     = NULL;

	pCellNode = pCell->m_pNext;

	while (pCellNode && pCellNode->m_BaseInfo.dwDynamic)
	{
		//这里一定忽略动态障碍
		//// 忽略动态障碍时,动态障碍一律视为disable
		//if (!bIgnoreDynamicSwitch)
		//{
		//	KDynamicCell* pDynamicCell   = (KDynamicCell*)pCellNode;
		//	BOOL bDynamicSwitch = m_pScene->GetDynamicObstacleState(pDynamicCell->m_wGroupIndex);
		//	if (bDynamicSwitch)
		//	{
		//		return pCellNode;
		//	}
		//}

		pCellNode = pCellNode->m_pNext;
	}

	return pCellNode;
}

KCell* KLogicalSceneConnection::GetLowestObstacle(const KRegion& regionCur, int nXCell, int nYCell)
{
	assert(nXCell >= 0);
	assert(nXCell < REGION_GRID_WIDTH);
	assert(nYCell >= 0);
	assert(nYCell < REGION_GRID_HEIGHT);

	return regionCur.m_pCells + REGION_GRID_WIDTH * nYCell + nXCell;
}
POINT KLogicalSceneConnection::LogicalToLogicalRegionIndex( FLOAT fLogicalX, FLOAT fLogicalY )
{
	POINT pTemp = 
	{
		(LONG)((fLogicalX / static_cast<FLOAT>(CELL_LENGTH)) / static_cast<FLOAT>(REGION_GRID_WIDTH)),
		(LONG)((fLogicalY / static_cast<FLOAT>(CELL_LENGTH)) / static_cast<FLOAT>(REGION_GRID_HEIGHT)),
	};
	return pTemp;
}
KCell* KLogicalSceneConnection::GetCurOrLowerObstacle( const KRegion& regionCur, int nXCell, int nYCell, int nZ, size_t* pOrderOfCell )
{
	KCell*  pResult     = NULL;
	KCell*  pCellNode   = NULL;
	int     nAltitude   = nZ / POINT_PER_ALTITUDE;
	size_t uOrder = 0;

	// 从最下面的一个Cell开始找,找到最上一个 "上表面<=nZ" 的Cell,也就是链表中最后一个 "上表面<=nZ" 的Cell
	// 如果所有Cell上表面都高于nZ,则返回NULL

	assert(nXCell >= 0);
	assert(nXCell < REGION_GRID_WIDTH);
	assert(nYCell >= 0);
	assert(nYCell < REGION_GRID_HEIGHT);

	pCellNode = GetLowestObstacle(regionCur, nXCell, nYCell);
	pResult		= pCellNode;

	while(NULL != pCellNode)
	{
		if (pCellNode->m_wLowLayer > nAltitude)	//得到下一个底部比当前高的，用上一个就行了
		{
			break;
		}

		pResult = pCellNode;

		if (pCellNode->m_wLowLayer <= nAltitude && nAltitude <= pCellNode->m_wHighLayer)	//如果正好包住，那用当前的
		{
			break;
		}

		pCellNode = GetNextCell(pCellNode, false);
		++uOrder;
	}

	if (NULL != pOrderOfCell)
	{
		*pOrderOfCell = uOrder;
	}

	return pResult;
}

KCell* KLogicalSceneConnection::GetCellOfRegion( KRegion* pRegion, INT nIndex )
{
	_ASSERTE(NULL != pRegion && nIndex < REGION_GRID_WIDTH * REGION_GRID_HEIGHT);
	return &pRegion->m_pCells[nIndex];
}
