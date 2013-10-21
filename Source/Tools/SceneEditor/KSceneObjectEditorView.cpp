// KSceneObjectEditorView.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneObjectEditorView.h"
#include ".\ksceneobjecteditorview.h"
#include "KObjectEditorDoc.h"
#include "IEKG3DScene.h"
//#include "KG3DEngineManager.h"//by dengzhihui Refactor 2006年11月6日
//#include "SceneEditorCommon.h"	//by dengzhihui Refactor 2006年11月6日

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorView

IMPLEMENT_DYNCREATE(KSceneObjectEditorView, CView)

KSceneObjectEditorView::KSceneObjectEditorView()
{
//	m_bAddOutputWnd = FALSE;
	m_nOutputWindowID = -1;
}

KSceneObjectEditorView::~KSceneObjectEditorView()
{
}


BEGIN_MESSAGE_MAP(KSceneObjectEditorView, CView)
	//{{AFX_MSG_MAP(KSceneObjectEditorView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorView drawing

void KSceneObjectEditorView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorView diagnostics

#ifdef _DEBUG
void KSceneObjectEditorView::AssertValid() const
{
	CView::AssertValid();
}

void KSceneObjectEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorView message handlers


LRESULT KSceneObjectEditorView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	LRESULT hr = CView::WindowProc(message, wParam, lParam);

	switch(message)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_MOUSEMOVE:
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		{
			g_pEngineManager->ProcessMessage(message,wParam,lParam); //<PROCESS_MESSAGE>
			break;
		}
	}

	return hr;
}

void KSceneObjectEditorView::OnSetFocus(CWnd* pOldWnd)
{
	/*
	KSceneObjectEditorDoc* pDoc = (KSceneObjectEditorDoc*) GetDocument();
	if(pDoc)
	{
		KG3DScene* pScene = pDoc->GetScene();
		if(pScene)
		{
			g_pEngineManager->SetFocusScene(pScene);
		}
	}*/

}
void KSceneObjectEditorView::OnSize(UINT nType, int cx, int cy)
{
	KSceneObjectEditorDoc* pDoc = (KSceneObjectEditorDoc*) GetDocument();
	if(pDoc)
	{
		IEKG3DScene* pScene = pDoc->GetScene();
		if(pScene)
		{
			pScene->OnResizeWindow(NULL);
		}
	}

}
int KSceneObjectEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}
void KSceneObjectEditorView::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	/*if(!m_bAddOutputWnd)
	{
		KSceneObjectEditorDoc* pDoc = (KSceneObjectEditorDoc*) GetDocument();
		if(pDoc)
		{
			KG3DScene* pScene = pDoc->GetScene();
			if(pScene)
			{
				int nOutputWindowID = 0;
				pScene->AddOutputWindow("Object Main",m_hWnd,OUTPUTWND_OBJECTEDITOR_MAIN,0,&nOutputWindowID);
				m_bAddOutputWnd = TRUE;
			}
		}
	}*/
	CView::OnTimer(nIDEvent);
}

void KSceneObjectEditorView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	
	KSceneObjectEditorDoc* pDoc = (KSceneObjectEditorDoc*) GetDocument();
	if(pDoc)
	{
		IEKG3DScene* pScene = pDoc->GetScene();
		if(pScene)
		{
			pScene->AddOutputWindow("Object Main",m_hWnd, OUTPUTWND_OBJECTEDITOR_MAIN,0,&m_nOutputWindowID);
		}
	}
	// TODO: Add your specialized code here and/or call the base class
	//SetTimer(0,1000,NULL);
}

void KSceneObjectEditorView::OnDestroy()
{
	KSceneObjectEditorDoc* pDoc = (KSceneObjectEditorDoc*) GetDocument();
	KGLOG_PROCESS_ERROR(pDoc);

	IEKG3DScene* pScene = pDoc->GetScene();
	KG_PROCESS_ERROR(pScene);

	pScene->RemoveOutputWindow(m_nOutputWindowID);
Exit0:
	CView::OnDestroy();
}