// KSceneObjectEditorDoc.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KObjectEditorDoc.h"
//(by dengzhihui 2006年7月6日 11:33:27
//#include "MainFrm.h"
//)
#include "IEEditor.h"

//#include "kg3denginemanager.h" //by dengzhihui Refactor 2006年11月6日
//#include "SceneEditorCommon.h"   //by dengzhihui Refactor 2006年11月6日

//#include "KG3DSceneObjectEditor.h"
#include "../../../Include/kg3dengine/KG3DTypes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorDoc

IMPLEMENT_DYNCREATE(KSceneObjectEditorDoc, CDocument)

KSceneObjectEditorDoc::KSceneObjectEditorDoc()
{
	//m_lpObjectEditorScene = NULL;
}

BOOL KSceneObjectEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

KSceneObjectEditorDoc::~KSceneObjectEditorDoc()
{
}


BEGIN_MESSAGE_MAP(KSceneObjectEditorDoc, CDocument)
	//{{AFX_MSG_MAP(KSceneObjectEditorDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorDoc diagnostics

#ifdef _DEBUG
void KSceneObjectEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void KSceneObjectEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorDoc serialization

void KSceneObjectEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorDoc commands

BOOL KSceneObjectEditorDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	MFCFramework::KSceneEditorApp* pApp = (MFCFramework::KSceneEditorApp*) AfxGetApp();
	if (pApp)
	{
		//(by dengzhihui 2006年7月6日 11:33:35
		//MFCFramework::CMainFrame* pMainFrame = (MFCFramework::CMainFrame*) pApp->m_pMainWnd;
		//)

		IEKG3DScene* pScene = NULL;
		g_pEngineManager->NewOneScene(SCENETYPE_OBJECTEDITOR, (IKG3DScene **)&pScene);//by dengzhihui Refactor 2006年11月6日
		SetScene(pScene);
		g_pEngineManager->SetFocusScene(pScene);//by dengzhihui Refactor 2006年11月6日
	}
	TCHAR str[256];
	wsprintf(str,"%s",lpszPathName);

	IEKG3DSceneObjectEditor* pObjEditor = NULL;
	m_lpScene->GetSceneObjectEditor(&pObjEditor);
	_ASSERTE(pObjEditor);
    IEKG3DSceneObjectEditor* lpObjectEdit = pObjEditor;//(KG3DSceneObjectEditor*)m_lpScene;
    lpObjectEdit->OpenObject((LPSTR)lpszPathName);
	//if(!m_lpObjectEditorScene->m_lpObject)
	//{
	//	if(FAILED(m_lpObjectEditorScene->LoadbjectFromFile(str)))
	//	{
	//		MessageBox(NULL,str,"打开文件失败",0);		
	//	}
	//}
	
	return TRUE;
}

BOOL KSceneObjectEditorDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// TODO: Add your specialized code here and/or call the base class
	/*if(m_lpObjectEditorScene)
	{
		if(m_lpObjectEditorScene->m_lpObject)
		{
			TCHAR Name[256];
			wsprintf(Name,"%s",lpszPathName);

			if(FAILED(m_lpObjectEditorScene->m_lpObject->Save2File(Name)))
			{
				MessageBox(NULL,Name,"保存失败",0);
			}
		}
	}*/
	return TRUE;
}

void KSceneObjectEditorDoc::OnCloseDocument() 
{
	// TODO: Add your specialized code here and/or call the base class
	IEKG3DScene* pScene = GetScene();

	if (pScene)
	{
		g_pEngineManager->RemoveScene(pScene);//by dengzhihui Refactor 2006年11月6日
		SetScene(NULL);
	}
	
	CDocument::OnCloseDocument();
}
