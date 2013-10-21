// SwordTestView.cpp : 实现文件
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "SwordTestView.h"
#include "ChildFrm.h"
//#include "MainFrm.h" //(by dengzhihui 2006年7月6日 14:53:11
#include "SwordTestDoc.h"
#include "kg3denginemanager.h"
#include "KG3DSceneClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

// KSwordTestView

IMPLEMENT_DYNCREATE(KSwordTestView, CView)

KSwordTestView::KSwordTestView()
{
//	m_bAddToScene = FALSE;
	m_nOutputWindowID = -1;
}

KSwordTestView::~KSwordTestView()
{
}

BEGIN_MESSAGE_MAP(KSwordTestView, CView)
	//{{AFX_MSG_MAP(KSceneEditorView)
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// KSwordTestView 绘图

void KSwordTestView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// KSwordTestView 诊断

#ifdef _DEBUG

void KSwordTestView::AssertValid() const
{
	CView::AssertValid();
}

void KSwordTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif _DEBUG

void KSwordTestView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);

	//g_cEngineManager.SetRenderWindow(m_hWnd);

	KSwordTestDoc* pDoc = (KSwordTestDoc*) GetDocument();
	//g_cEngineManager.SetCurScene(pDoc->m_lpSwordTest);

	RECT r;
	GetWindowRect(&r);
	float Weigth = (r.right - r.left)*1.0f;
	float Height = (r.bottom - r.top)*1.0f ;
	//m_Camera.Aspect = Weigth/Height;
	//g_GraphiceEngine.SetCurCamera(&m_Camera);

}

int KSwordTestView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
	//g_cEngineManager.SetRenderWindow(m_hWnd);

	KSwordTestDoc* pDoc = (KSwordTestDoc*) GetDocument();
	//g_cEngineManager.SetCurScene(pDoc->m_lpSwordTest);

	//SetTimer(0,1000,NULL);
	return 0;
}

void KSwordTestView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	KSwordTestDoc* pDoc = (KSwordTestDoc*) GetDocument();
	if(pDoc)
	{
		KG3DScene* pScene = pDoc->GetScene();
		if(pScene)
		{
			pScene->OnResizeWindow();
		}
	}

}

void KSwordTestView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
	//g_cEngineManager.SetRenderWindow(m_hWnd);

	RECT r;
	GetWindowRect(&r);
	float Weigth = (r.right - r.left)*1.0f;
	float Height = (r.bottom - r.top)*1.0f ;
	//m_Camera.Aspect = Weigth/Height;

	KSwordTestDoc* pDoc = (KSwordTestDoc*) GetDocument();
	//g_cEngineManager.SetCurScene(pDoc->m_lpSwordTest);
	// TODO: remove this code when final selection model code is written
}

LRESULT KSwordTestView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	LRESULT hr = CView::WindowProc(message, wParam, lParam);

	switch(message)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_MOUSEMOVE:
	case WM_CHAR:
	case WM_KEYDOWN:
	case WM_KEYUP:

	case ID_EDIT_UNDO:
	case ID_EDIT_CUT:
	case ID_EDIT_COPY:
	case ID_EDIT_PASTE:
	case ID_EDIT_REDO:
		{

			CMDIFrameWnd *pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
			CMDIChildWnd* pWnd = pMainFrame->MDIGetActive();

			//if (pWnd==pChildFrame)
			//{
				//g_GraphiceEngine.SetCurCamera(&m_Camera);
				//g_cEngineManager.ProcessMessage(message,wParam,lParam);
				//m_Camera = *g_GraphiceEngine.GetCurCamera();
			//}
			break;
		}
	}

	return hr;
}

void KSwordTestView::OnTimer(UINT nIDEvent)
{

	CView::OnTimer(nIDEvent);
}


// KSwordTestView 消息处理程序
