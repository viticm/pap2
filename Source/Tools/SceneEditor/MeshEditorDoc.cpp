// MeshEditorDoc.cpp : 实现文件
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "MeshEditorDoc.h"
//#include "MainFrm.h" //(by dengzhihui 2006年7月6日 14:51:33
//#include "kg3denginemanager.h"
//#include "SceneEditorCommon.h"//by dengzhihui Refactor 2006年11月6日
#include "KG3DTypes.h"
#include "IEKG3DMaterial.h"
#include "MeshEditorView.h"
#include "IEEditor.h"
#include "KSceneModelEditorAnimationPanel.h"
#include "KSceneModelEditorFrameWnd.h"
#include "KGPhysicsParamEditorDlg.h"
#include "IEKG3DSceneOutputWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(KSceneModelEditorDoc, COleDocument)

KSceneModelEditorDoc::KSceneModelEditorDoc()
{
	m_lpScene = NULL;
}

BOOL KSceneModelEditorDoc::OnNewDocument()
{
	if (!COleDocument::OnNewDocument())
		return FALSE;

	IEKG3DScene* pScene = NULL;
	g_pEngineManager->NewOneScene(SCENETYPE_MODELEDITOR,(IKG3DScene**)&pScene);//by dengzhihui Refactor 2006年11月6日
	_ASSERTE(pScene);
	SetScene(pScene);

	return TRUE;
}

KSceneModelEditorDoc::~KSceneModelEditorDoc()
{
}


BEGIN_MESSAGE_MAP(KSceneModelEditorDoc, COleDocument)
    	ON_COMMAND(ID_PHYSISCSETTING, &KSceneModelEditorDoc::OnPhysiscsetting)
		ON_COMMAND(ID_RELOADMESH, &KSceneModelEditorDoc::OnReloadmesh)
END_MESSAGE_MAP()



void KSceneModelEditorDoc::OnPhysiscsetting()
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = S_OK;
    int nRetCode = false;
    IEKG3DScene* pScene = NULL;
    IEKG3DSceneModelEditor* pEditor = NULL;
    KGPhysicsParamEditorDlg *pDialog = NULL;
    int nDlgCreateFlag = false;

    pScene = GetScene();
    KGLOG_PROCESS_ERROR(pScene);

    hrRetCode = pScene->GetSceneModelEditor(&pEditor);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    pDialog = new KGPhysicsParamEditorDlg; // auto destroy when Dialog close
    KGLOG_PROCESS_ERROR(pDialog);
    
    nRetCode = pDialog->Create(KGPhysicsParamEditorDlg::IDD); 
    KGLOG_PROCESS_ERROR(nRetCode);
    nDlgCreateFlag = true;

    hrRetCode = pDialog->SetSceneEditor(pScene);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    nRetCode = pDialog->ShowWindow(SW_SHOW);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (nDlgCreateFlag)
        {
            pDialog->DestroyWindow();
            nDlgCreateFlag = false;
        }
        KG_DELETE(pDialog);
    }
}

// KSceneModelEditorDoc 诊断

#ifdef _DEBUG
void KSceneModelEditorDoc::AssertValid() const
{
	COleDocument::AssertValid();
}

void KSceneModelEditorDoc::Dump(CDumpContext& dc) const
{
	COleDocument::Dump(dc);
}
#endif //_DEBUG


// KSceneModelEditorDoc 序列化

void KSceneModelEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// KSceneModelEditorDoc 命令
HRESULT KSceneModelEditorDoc::SetPanelContainer(IEKG3DModel *pModel)
{
	HRESULT hr = E_FAIL;
	KSceneModelEditorAnimationPanel* pPanel = NULL;
	KSceneModelEditorFrameWnd* pFrameWnd = NULL;
	KG_PROCESS_ERROR(pModel);
	pFrameWnd = static_cast<KSceneModelEditorFrameWnd*>(GetFirstFrame());
	KG_PROCESS_ERROR(pFrameWnd);

	pPanel = pFrameWnd->GetAnimationSeekBar();
	KG_PROCESS_ERROR(pPanel);
	pModel->GetAnimationTagContainer(&(pPanel->m_pContainer));
	hr = S_OK;
Exit0:
	return hr;
}
BOOL KSceneModelEditorDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	BOOL bReturn = FALSE;
	HRESULT hr = E_FAIL;
	IEKG3DScene* pScene = NULL;
	CFrameWnd* pFrame = GetFirstFrame();
	IEKG3DSceneModelEditor* pModelEditor = NULL;
	CView* pOrgView = NULL;
	KSceneModelEditorView* pView = NULL;
	//POSITION Pos;
	IEKG3DModel *pModel = NULL;
    KSceneModelEditorAnimationPanel* pPanel = NULL;
    KSceneModelEditorFrameWnd* pFrameWnd = NULL;

	KG_PROCESS_ERROR(pFrame);

	::SetCurrentDirectory(g_szDefWorkDirectory);
	TCHAR RetPath[MAX_PATH];
	g_pEngineManager->GetRelativePath(g_szDefWorkDirectory,lpszPathName,RetPath);

	hr = g_pEngineManager->NewOneScene(SCENETYPE_MODELEDITOR,(IKG3DScene**)&pScene);//by dengzhihui Refactor 2006年11月6日
	KG_COM_PROCESS_ERROR(hr);

	hr = SetScene(pScene);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pEngineManager->SetFocusScene(pScene);//by dengzhihui Refactor 2006年11月6日
	KG_COM_PROCESS_ERROR(hr);

	//pModelEditor = static_cast<KG3DSceneModelEditor*>(pScene);
	pScene->GetSceneModelEditor(&pModelEditor);
	KG_PROCESS_ERROR(pModelEditor);
	
	pModelEditor->OpenModel(RetPath,"","", FALSE, &pModel);
    KG_PROCESS_ERROR(pModel);

    pFrameWnd = static_cast<KSceneModelEditorFrameWnd*>(GetFirstFrame());
    KG_PROCESS_ERROR(pFrameWnd);

    pPanel = pFrameWnd->GetAnimationSeekBar();
    KG_PROCESS_ERROR(pPanel);
    pModel->GetAnimationTagContainer(&(pPanel->m_pContainer));

	/*Pos = GetFirstViewPosition();
	KG_PROCESS_ERROR(Pos);

	pOrgView = GetNextView(Pos);
	KG_PROCESS_ERROR(pOrgView);

	pView = static_cast<KSceneModelEditorView*>(pOrgView);*/
	pView = pFrameWnd->GetModelEditorView();
	pView->Zoom();
	
	DWORD dwType = 0;
	pModel->GetType(&dwType);

	if (dwType == MESHTYPE_SPEEDTREE)
	{
		pView->OnSpeedtreePanel();
	}

	bReturn = TRUE;
Exit0:
	return bReturn;
}

void KSceneModelEditorDoc::OnCloseDocument()
{
	// TODO: Add your specialized code here and/or call the base class
	g_pEngineManager->RemoveScene(m_lpScene);//by dengzhihui Refactor 2006年11月6日
	m_lpScene = NULL;
	__super::OnCloseDocument();
}

BOOL KSceneModelEditorDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
    BOOL bRestore = FALSE;
	HRESULT hrRetCode = E_FAIL;
	TCHAR strCurrentPathSave[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, strCurrentPathSave);
	::SetCurrentDirectory(g_szDefWorkDirectory);
	
	IEKG3DModel *pModel = NULL;
	hrRetCode = GetCurrentModel(&pModel);
	KG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = pModel->SaveToFile(lpszPathName, 0);
	KG_COM_PROCESS_ERROR(hrRetCode);


	/*vector<IEKG3DModel*>vecModels;
	hrRetCode = GetAllModels(&vecModels);
	KG_COM_PROCESS_ERROR(hrRetCode);

	int nSucceed = 0,nFailed = 0;
	if (vecModels.size() == 1 &&
		vecModels[0])
	{
		IEKG3DModel *pModel = vecModels[0];
		TCHAR strLower[MAX_PATH];
		TCHAR strLowerDest[MAX_PATH];
		const TCHAR *strFileName = NULL;
		pModel->GetName(&strFileName);
		strcpy_s(strLower, MAX_PATH, strFileName);
		_strlwr_s(strLower, MAX_PATH);

		strcpy_s(strLowerDest, MAX_PATH, lpszPathName);
		_strlwr_s(strLowerDest, MAX_PATH);

		TCHAR *pFind = strstr(strLowerDest, strLower);
		if (!pFind)
		{
			hrRetCode = pModel->SaveToFile(lpszPathName, 0);
			if (FAILED(hrRetCode))
			{
				nFailed++;
			}
			else
			{
				nSucceed++;
			}
			goto Exit1;
		}
	}

	bRestore = TRUE;
	for(size_t i=0;i<vecModels.size();i++)
	{
		IEKG3DModel *pModel = vecModels[i];

		hrRetCode = pModel->SaveToFile(lpszPathName, 1);
		if (FAILED(hrRetCode))
		{
			nFailed ++;
		}
		else
		{
			nSucceed++;
		}
	}*/
	
//Exit1:
	
	if (FAILED(hrRetCode))
	{
		TCHAR szName[MAX_PATH << 1];
		sprintf_s(szName, 
			MAX_PATH << 1, 
			"Save model %s failed.",
			lpszPathName);
		MessageBox(AfxGetMainWnd()->GetSafeHwnd(), szName, "错误", MB_OK);
	}
	if (bRestore)
	{
		::SetCurrentDirectory(strCurrentPathSave);
	}
Exit0:
	return TRUE;
}

HRESULT KSceneModelEditorDoc::SetScene(IEKG3DScene* pScene)
{
	HRESULT hResult = E_FAIL;

	POSITION Pos = GetFirstViewPosition();

	KGLOG_PROCESS_ERROR(pScene);

	IEKG3DScene* pOldScene = GetScene();

	while (Pos != NULL)
	{
		CView* pOrgView = GetNextView(Pos);
		ENSURE_VALID(pOrgView);

		if (pOrgView->IsKindOf(RUNTIME_CLASS(KSceneModelEditorView)))
		{
			KSceneModelEditorView* pView = (KSceneModelEditorView *)pOrgView;
			if (pOldScene)
			{
				pOldScene->RemoveOutputWindow(pView->GetOutputWindowID());
			}

			pScene->AddOutputWindow("Model Main", pView->GetSafeHwnd(), OUTPUTWND_MODELEDITOR_MAIN, 0,
				&pView->GetOutputWindowID());
		}

	}

	KEditorDocumentBase::SetScene(pScene);
	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KSceneModelEditorDoc::GetCurrentModel(IEKG3DModel **ppiModel)
{
    HRESULT hrResult  = E_FAIL;
	HRESULT hrRetCode = E_FAIL;
    IEKG3DScene *piScene = NULL;
    IEKG3DSceneModelEditor *piEditor = NULL;
    IEKG3DModel *piModel = NULL;
    
    KGLOG_PROCESS_ERROR(ppiModel);

    piScene = GetScene();
	KG_PROCESS_ERROR(piScene);
    
    hrRetCode = piScene->GetSceneModelEditor(&piEditor);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = piEditor->GetCurModel(ppiModel);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = S_OK;
Exit0:
    return hrRetCode;
}

HRESULT KSceneModelEditorDoc::GetAllModels(vector<IEKG3DModel*>*pvecModel)
{
	HRESULT hrResult  = E_FAIL;
	HRESULT hrRetCode = E_FAIL;
	IEKG3DScene *piScene = NULL;
	IEKG3DSceneModelEditor *piEditor = NULL;
	IEKG3DModel *piModel = NULL;

	KGLOG_PROCESS_ERROR(pvecModel);

	piScene = GetScene();
	KG_PROCESS_ERROR(piScene);

	hrRetCode = piScene->GetSceneModelEditor(&piEditor);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = piEditor->GetAllModels(pvecModel);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = S_OK;
Exit0:
	return hrRetCode;
}

void KSceneModelEditorDoc::OnReloadmesh()
{
	HRESULT hrRetCode = E_FAIL;


	vector<IEKG3DModel*>vecModels;
	hrRetCode = GetAllModels(&vecModels);
	KG_COM_PROCESS_ERROR(hrRetCode);

	int nSucceed = 0,nFailed = 0;
	for(size_t i=0;i<vecModels.size();i++)
	{
		IEKG3DModel *pModel = vecModels[i];

		hrRetCode = pModel->ReloadMesh();
		if (FAILED(hrRetCode))
		{
			nFailed ++;
		}
		else
		{
			nSucceed++;
		}
	}

Exit0:
	return ;
}
