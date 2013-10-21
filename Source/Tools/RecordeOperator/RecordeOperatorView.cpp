// RecordeOperatorView.cpp : implementation of the CRecordeOperatorView class
//

#include "stdafx.h"
#include "RecordeOperator.h"

#include "RecordeOperatorDoc.h"
#include "RecordeOperatorView.h"
#include "IRecPlayer.h"
#include "KCameraOptionDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRecordeOperatorView

IMPLEMENT_DYNCREATE(CRecordeOperatorView, CView)

BEGIN_MESSAGE_MAP(CRecordeOperatorView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_MENUE_CAMERAOPTION, &OnCameraOption)
	ON_COMMAND(ID_MENUE_MOVIESTART, &OnMovieStart)
	ON_COMMAND(ID_MENUE_MOVIESTOP, &OnMovieStop)
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	//ON_WM_CREATE()
END_MESSAGE_MAP()

// CRecordeOperatorView construction/destruction

CRecordeOperatorView::CRecordeOperatorView()
{
	// TODO: add construction code here
	m_dwLastTime = 0;
	m_pointMouse.x = 0;
	m_pointMouse.y = 0;
}

CRecordeOperatorView::~CRecordeOperatorView()
{
}

int CRecordeOperatorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect   rect(100,100,1124,868);     
	::AdjustWindowRectEx(&rect,   GetStyle(),   FALSE,   GetExStyle());     
	MoveWindow(&rect);         
	return 0;
}

BOOL CRecordeOperatorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CRecordeOperatorView drawing

void CRecordeOperatorView::OnDraw(CDC* /*pDC*/)
{
	D3DXVECTOR3 vecPos(0,0,0);
	D3DXVECTOR2 vec2MouseMove(0,0);
	CRecordeOperatorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	IERecPlayer* pRecord = pDoc->GetRecord();
	{
		if (m_dwLastTime != 0)
		{
			float fDis = (GetCurrentTime() - m_dwLastTime) * 0.001;
			if( ::GetAsyncKeyState(VK_UP) & 0x8000f)
				vecPos.x = fDis * 1000;				
			if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f)
				vecPos.x = -fDis * 1000;				
			if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f)
				vecPos.z = fDis * 1000;				
			if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
				vecPos.z = -fDis * 1000;
			if (::GetAsyncKeyState(VK_LBUTTON) & 0X800f)
			{
				CPoint Point1 = m_pointMouse;
				CPoint Point2;
				GetCursorPos(&Point2);;
				CRect   rect;   
				GetClientRect(&rect);   
				if(rect.PtInRect(Point2))   
				{   
					CPoint pointMouseMove = Point2 - Point1;
					D3DXVECTOR2 vec2Dis(pointMouseMove.x,pointMouseMove.y);
					D3DXVec2Normalize(&vec2Dis,&vec2Dis);
					vec2Dis = vec2Dis * fDis;
					vec2MouseMove = vec2Dis;
				} 
			}

		}
		m_dwLastTime = GetCurrentTime();
		//if (::GetAsyncKeyState(VK_LBUTTON) & 0X800f)
		//{
		//	CPoint Point1 = m_pointMouse;
		//	CPoint Point2;
		//	GetCursorPos(&Point2);;
		//	CRect   rect;   
		//	GetClientRect(&rect);   
		//	if(rect.PtInRect(Point2))   
		//	{   
		//		pointMouseMove = Point2 - Point1;
		//		D3DXVECTOR2 vec2Dis(pointMouseMove.x,pointMouseMove.y);
		//		D3DXVec2Normalize(&vec2Dis,&vec2Dis);
		//		vec2
		//		pointMouseMove.x *= 1000;
		//		pointMouseMove.y *= 1000;
		//	} 
		//}
		GetCursorPos(&m_pointMouse);
		pRecord->UpDateFreeCamera(vecPos,vec2MouseMove);
	}
	pRecord->FrameMoveRePlay();
}

void CRecordeOperatorView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CRecordeOperatorDoc* pDoc =  GetDocument();
	KGLOG_PROCESS_ERROR(pDoc);

	IERecPlayer* pRecord = pDoc->GetRecord();
	KGLOG_PROCESS_ERROR(pRecord);	
	pRecord->SetReplayWindow(m_hWnd);
	//pRecord->RePlay();

Exit0:
	return;
}


void CRecordeOperatorView::OnSize(UINT nType,int cx,int cy)
{
	RECT rectView;
	GetClientRect(&rectView);
	CRecordeOperatorDoc* pDoc =  GetDocument();
	if (pDoc)
	{
		IERecPlayer* pRecord = pDoc->GetRecord();
		if (pRecord)
			pRecord->OnResizeWindow(&rectView);
	}
	CView::OnSize(nType,cx,cy);
}
// CRecordeOperatorView printing

BOOL CRecordeOperatorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CRecordeOperatorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CRecordeOperatorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CRecordeOperatorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
}

// CRecordeOperatorView diagnostics

void CRecordeOperatorView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	//if(GetDocument()->GetPattern()!=NULL)
	{
		CMenu menu;
		menu.CreatePopupMenu();	
		menu.AppendMenu(MF_STRING, ID_MENUE_CAMERAOPTION,"Camera Option"); 
		menu.AppendMenu(MF_SEPARATOR);
		CRecordeOperatorDoc* pDoc =  GetDocument();
		if (pDoc)
		{
			IERecPlayer* pRecord = pDoc->GetRecord();
			if (pRecord)
			{
				if 	(pRecord->IsMovieOut())
					menu.AppendMenu(MF_STRING, ID_MENUE_MOVIESTOP,"Movie Record Stop");
				else
					menu.AppendMenu(MF_STRING, ID_MENUE_MOVIESTART,"Movie Record Start");
			}
		}
		
	//	menu.AppendMenu(MF_STRING, ID_ENTERFULLSCREEN,theApp->m_LangIni.GetValue("Short Menu", "1001"));
		menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,
			point.x,point.y,AfxGetMainWnd());
		menu.DestroyMenu();
	}

}
void CRecordeOperatorView::OnCameraOption()
{
	CRecordeOperatorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	IERecPlayer* pRecord = pDoc->GetRecord();
	KCameraOptionDialog CameraDlg(pRecord);
	CameraDlg.DoModal();
}

void CRecordeOperatorView::OnMovieStart()
{
	CRecordeOperatorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	IERecPlayer* pRecord = pDoc->GetRecord();
	if (pRecord)
		pRecord->StartMovieOut();
}

void CRecordeOperatorView::OnMovieStop()
{
	CRecordeOperatorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	IERecPlayer* pRecord = pDoc->GetRecord();
	if (pRecord)
		pRecord->StopMovieOut();
}

#ifdef _DEBUG
void CRecordeOperatorView::AssertValid() const
{
	CView::AssertValid();
}

void CRecordeOperatorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRecordeOperatorDoc* CRecordeOperatorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRecordeOperatorDoc)));
	return (CRecordeOperatorDoc*)m_pDocument;
}
#endif //_DEBUG


// CRecordeOperatorView message handlers
