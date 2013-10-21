// KSceneDataFlowEditorDoc.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneDataFlowEditorDoc.h"

// KSceneDataFlowEditorDoc

IMPLEMENT_DYNCREATE(KSceneDataFlowEditorDoc, CDocument)

KSceneDataFlowEditorDoc::KSceneDataFlowEditorDoc()
{
}


BOOL KSceneDataFlowEditorDoc::OnNewDocument()
{

	HRESULT hrRetCode = NULL;
	IEKG3DScene* pScene = NULL;

	if (!CDocument::OnNewDocument())
		return FALSE;

	{
		IEKG3DScene* pScene = NULL;
		g_pEngineManager->NewOneScene(SCENETYPE_DATAFLOWEDITOR,(IKG3DScene**)&pScene);//by dengzhihui Refactor 2006年11月6日
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
	//pScene = GetScene();
	//KGLOG_PROCESS_ERROR(pScene);

	//hrRetCode = pScene->GetSceneSceneEditor(&pEditor);
	//KGLOG_COM_PROCESS_ERROR(hrRetCode);

	//(by dengzhihui 2006年8月4日 11:34:00
	SetModifiedFlag();
	//)
	return TRUE;
//
//Exit0:
//
//	return FALSE;
//
//	if (!CDocument::OnNewDocument())
//		return FALSE;
//	return TRUE;
}

KSceneDataFlowEditorDoc::~KSceneDataFlowEditorDoc()
{
}


BEGIN_MESSAGE_MAP(KSceneDataFlowEditorDoc, CDocument)
END_MESSAGE_MAP()


// KSceneDataFlowEditorDoc diagnostics

#ifdef _DEBUG
void KSceneDataFlowEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void KSceneDataFlowEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// KSceneDataFlowEditorDoc serialization

void KSceneDataFlowEditorDoc::Serialize(CArchive& ar)
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
#endif


void KSceneDataFlowEditorDoc::OnCloseDocument()
{
	// TODO: Add your specialized code here and/or call the base class
	g_pEngineManager->RemoveScene(m_lpScene);//by dengzhihui Refactor 2006年11月6日
	m_lpScene = NULL;
	__super::OnCloseDocument();
}

// KSceneDataFlowEditorDoc commands
