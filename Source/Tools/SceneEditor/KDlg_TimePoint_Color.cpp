// KDlg_TimePoint_Color.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

//#include "SceneEditor.h"
//#include "KDlg_TimePoint_Color.h"
//#include "KTimePointColor.h"
//
//
//// KDlg_TimePoint_Color dialog
//namespace PropertyBase
//{
//
//IMPLEMENT_DYNAMIC(KDlg_TimePoint_Color, CDialog)
//KDlg_TimePoint_Color::KDlg_TimePoint_Color(ModelSFX::KTimePointColor* pData, CWnd* pParent)
//	: CDialog(KDlg_TimePoint_Color::IDD, pParent)
//{
//	m_pTPBarBuffer = new DWORD[LIFE_SCREEN_POINT_SIZE * 23];
//	memset(m_pTPBarBuffer, 0, LIFE_SCREEN_POINT_SIZE * sizeof(DWORD) * 23);
//	m_pData = pData;
//	Create(IDD, pParent);
//}
//
//KDlg_TimePoint_Color::~KDlg_TimePoint_Color()
//{
//	int iSize = m_vecButton.size();
//	int i;
//	m_vecTP.clear();
//	for ( i = 0; i < iSize; i++ )
//	{
//		delete m_vecButton[i];
//	}
//	m_vecButton.clear();
//	delete[] m_pTPBarBuffer;
//}
//
//void KDlg_TimePoint_Color::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_BUTTON_PREV, m_ctlBtPrev);
//	DDX_Control(pDX, IDC_BUTTON_NEXT, m_ctlBtNext);
//	DDX_Control(pDX, IDC_BUTTON_DEL, m_ctlBtDel);
//	//DDX_Control(pDX, IDC_BUTTON_NEW, m_ctlBtNew);
//
//	DDX_Control(pDX, IDC_SLIDER_RED, m_ctlSliderRed);
//	DDX_Control(pDX, IDC_SLIDER_GREEN, m_ctlSliderGreen);
//	DDX_Control(pDX, IDC_SLIDER_BLUE, m_ctlSliderBlue);
//	DDX_Control(pDX, IDC_SLIDER_ALPHA, m_ctlSliderAlpha);
//
//
//	DDX_Text(pDX, IDC_EDIT_POSITION, m_fPosition);
//	DDX_Text(pDX, IDC_EDIT_RED, m_iRed);
//	DDX_Text(pDX, IDC_EDIT_BLUE, m_iBlue);
//	DDX_Text(pDX, IDC_EDIT_GREEN, m_iGreen);
//	DDX_Text(pDX, IDC_EDIT_ALPHA, m_iAlpha);
//
//	DDV_MinMaxInt(pDX, m_iRed, 0, 255);
//	DDV_MinMaxInt(pDX, m_iBlue, 0, 255);
//	DDV_MinMaxInt(pDX, m_iGreen, 0, 255);
//	DDV_MinMaxInt(pDX, m_iAlpha, 0, 255);
//
//	DDV_MinMaxFloat(pDX, m_fPosition, 0.0f, 1.0f);
//}
//
//
//BEGIN_MESSAGE_MAP(KDlg_TimePoint_Color, CDialog)
//	ON_BN_CLICKED(IDC_BUTTON_PREV, OnBnClickedButtonPrev)
//	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnBnClickedButtonNext)
//	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
//	//ON_BN_CLICKED(IDC_BUTTON_NEW, OnBnClickedButtonNew)
//	ON_BN_CLICKED(IDC_BUTTON_PICK, OnBnClickedButtonPick)
//	ON_WM_PAINT()
//	ON_WM_HSCROLL()
//	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
//	ON_EN_CHANGE(IDC_EDIT_POSITION, OnEnChangeEditPosition)
//	ON_EN_CHANGE(IDC_EDIT_RED, OnEnChangeEditRed)
//	ON_EN_CHANGE(IDC_EDIT_GREEN, OnEnChangeEditGreen)
//	ON_EN_CHANGE(IDC_EDIT_BLUE, OnEnChangeEditBlue)
//	ON_EN_CHANGE(IDC_EDIT_ALPHA, OnEnChangeEditAlpha)
//	ON_WM_LBUTTONDOWN()
//	ON_WM_MOUSEMOVE()
//END_MESSAGE_MAP()
//
//
//// KDlg_TimePoint_Color message handlers
//
//BOOL KDlg_TimePoint_Color::OnInitDialog()
//{
//	CDialog::OnInitDialog();
//
//	HINSTANCE hInst = AfxGetApp()->m_hInstance;
//	m_curNormal = LoadCursor(NULL, IDC_ARROW);
//	m_curAdd = LoadCursor(hInst, MAKEINTRESOURCE(IDC_CURSOR_ADD));
//	m_iIndex = 0;
//	RECT rectWindow;
//	RECT rectButton;
//	CClientDC dc(this);
//
//	m_ctlBtNext.SetBitmap(::LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_NEXT)));
//	m_ctlBtPrev.SetBitmap(::LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_PREV)));
//	m_ctlBtDel.SetBitmap(::LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_DEL)));
//	GetDlgItem(IDC_BUTTON_PICK)->GetWindowRect(&rectButton);
//	GetWindowRect(&rectWindow);
//
//	ModelSFX::KTimePointColor* tpColor = m_pData;//reinterpret_cast<ModelSFX::KTimePointColor*>(m_lpProperty->Address);
//	int iSize = tpColor->Size();
//	int i;
//
//	DWORD color0 = Conversion(tpColor->m_vecTP[0].crColor);
//	TimePoint tp0 =  { color0, 0};
//	m_vecTP.push_back(tp0);
//
//	for ( i = 0; i < iSize; i++ )
//	{
//		DWORD color = Conversion(tpColor->m_vecTP[i].crColor);
//		TimePoint tp = { color, (int)(LIFE_SCREEN_POINT_SIZE * tpColor->m_vecTP[i].fTime)};
//		m_vecTP.push_back(tp);
//
//		POINT pt = { tp.iOffset + 11, 49 };
//		if( i == iSize - 1 )
//			pt.x -= 1;
//		KTimePointButton* pPB = new KTimePointButton;
//		pPB->Create(pt, this, IDC_BUTTON_START + i);
//		m_vecButton.push_back(pPB);
//	}
//
//
//	//////////////////////////////////////////////////////////////////////////
//	color0 = Conversion(tpColor->m_vecTP[i-1].crColor);
//	TimePoint tpend = { color0, LIFE_SCREEN_POINT_SIZE};
//	m_vecTP.push_back(tpend);
//	//////////////////////////////////////////////////////////////////////////
//
//	m_crColorSel = m_vecTP[1].crColor;//This is for draw, cause GDI's RGB is differ from D3D
//	m_dwColor = tpColor->GetValue(0.0f);//This is for data
//	m_vecButton[0]->SetStatus(TRUE);
//	//////////////////////////////////////////////////////////////////////////
//
//	m_rectSelColor.top = 135;
//	m_rectSelColor.bottom = 158;
//	m_rectSelColor.left = 15;
//	m_rectSelColor.right = 88;
//
//	m_rectColorTP.left = 10;
//	m_rectColorTP.right = 10 + LIFE_SCREEN_POINT_SIZE;
//	m_rectColorTP.top = 25;
//	m_rectColorTP.bottom = 48;
//
//	m_rectEffReg = m_rectColorTP;
//	m_rectEffReg.bottom += 10;
//	m_rectDropReg = m_rectEffReg;
//	m_rectDropReg.bottom += 10;
//
//	m_iAlpha = (BYTE)GetAlpha(m_dwColor);
//	m_iBlue  = (BYTE)GetBlue(m_dwColor);
//	m_iRed   = (BYTE)GetRed(m_dwColor);
//	m_iGreen = (BYTE)GetGreen(m_dwColor);
//
//	m_fPosition = (float)m_vecTP[m_iIndex+1].iOffset / LIFE_SCREEN_POINT_SIZE;
//
//	m_ctlSliderAlpha.SetRange(0, 255);
//	m_ctlSliderRed.SetRange(0, 255);
//	m_ctlSliderBlue.SetRange(0, 255);
//	m_ctlSliderGreen.SetRange(0, 255);
//
//	m_ctlSliderAlpha.SetPos(m_iAlpha);
//	m_ctlSliderRed.SetPos(m_iRed);
//	m_ctlSliderBlue.SetPos(m_iBlue);
//	m_ctlSliderGreen.SetPos(m_iGreen);
//
//	UpdateData(FALSE);
//	DrawSelectColor(&dc);
//
//	return TRUE;
//
//}
//
//
//void KDlg_TimePoint_Color::OnBnClickedButtonPrev()
//{
//	if ( m_iIndex > 0 )
//		ChangeActiveButton(m_iIndex - 1);
//	this->SetFocus();
//}
//
//void KDlg_TimePoint_Color::OnBnClickedButtonNext()
//{
//	int iSize = m_vecButton.size() - 1;
//	if ( m_iIndex < iSize )
//		ChangeActiveButton(m_iIndex + 1);
//	this->SetFocus();
//}
//
//void KDlg_TimePoint_Color::OnBnClickedButtonDel()
//{
//	if ( m_vecButton.size() == 1 )
//		return;
//	int i;
//	int iSize = m_vecButton.size();
//	for ( i = m_iIndex + 1; i < iSize ; i++ )
//	{
//		m_vecButton[i]->SetDlgCtrlID(m_vecButton[i]->GetDlgCtrlID() - 1);
//	}
//	vector<TimePoint>::iterator it = m_vecTP.begin();
//	vector<KTimePointButton*>::iterator it1 = m_vecButton.begin();
//	advance(it1, m_iIndex);
//	advance(it, m_iIndex + 1);
//	m_vecButton[m_iIndex]->ShowWindow(SW_HIDE);
//	delete m_vecButton[m_iIndex];
//	m_vecButton.erase(it1);
//	m_vecTP.erase(it);
//	if ( iSize == 2 )
//		m_iIndex = 0;
//	if ( m_iIndex >= iSize - 1)
//		m_iIndex = iSize - 2;
//	ChangeActiveButton(m_iIndex);
//	InvalidateRect(&m_rectColorTP);
//	this->SetFocus();
//}
//
//void KDlg_TimePoint_Color::OnBnClickedButtonPick()
//{
//	CColorDialog dlgColor(m_crColorSel&0x00ffffff, CC_FULLOPEN|CC_RGBINIT);
//	if ( dlgColor.DoModal() == IDOK )
//	{
//		m_crColorSel = dlgColor.GetColor();
//		m_iBlue  = (BYTE)((m_crColorSel<<8)>>24);
//		m_iRed   = (BYTE)((m_crColorSel<<24)>>24);
//		m_iGreen = (BYTE)((m_crColorSel<<16)>>24);
//		m_iAlpha = 0xff;
//		m_ctlSliderRed.SetPos(m_iRed);
//		m_ctlSliderGreen.SetPos(m_iGreen);
//		m_ctlSliderBlue.SetPos(m_iBlue);
//		m_ctlSliderAlpha.SetPos(m_iAlpha);
//		m_vecTP[m_iIndex+1].crColor = m_crColorSel | 0xff000000;
//		UpdateData(FALSE);
//		Invalidate();
//	}
//}
//
//void KDlg_TimePoint_Color::DrawTimePointColor(CDC* pdc)
//{
//	CPen PenEdge(PS_SOLID, 1, 0xff000000);
//	pdc->Rectangle(&m_rectColorTP);
//	int iSize = m_vecTP.size();
//    int i;
//	int y1 = 26;
//	int y2 = 47;
//	for ( i = 0; i < iSize - 1; i++ )
//	{
//		int x1 = m_vecTP[i].iOffset;
//		int x2 = m_vecTP[i + 1].iOffset;
//		COLORREF crStart = m_vecTP[i].crColor;
//		COLORREF crEnd = m_vecTP[i+1].crColor;
//		int iOffset = x2 - x1;
//		float fRed = (float)((int)(crEnd<<24>>24) - (int)(crStart<<24>>24)) / (float)iOffset;
//		float fGreen = (float)((int)(crEnd<<16>>24) - (int)(crStart<<16>>24)) / (float)iOffset;
//		float fBlue = (float)((int)(crEnd<<8>>24) - (int)(crStart<<8>>24)) / (float)iOffset;
//		float fAlpha = ( float)((int)(crEnd & 0x000000ff) - (int)(crStart & 0x000000ff)) / (float)iOffset;
//		BYTE RedStart   = (BYTE)(crStart<<24>>24);
//		BYTE GreenStart = (BYTE)(crStart<<16>>24);
//		BYTE BlueStart  = (BYTE)(crStart<<8>>24);
//		BYTE AlphaStart = (BYTE)(crStart & 0x000000ff);
//
//		COLORREF crStep;
//		int j;
//		for( j = 0; j < iOffset; j++ )
//		{
//			crStep = RGB( (BYTE)(RedStart + fRed * j), (BYTE)(GreenStart + fGreen * j), (BYTE)(BlueStart + fBlue * j));
//			for ( int y = y1; y < y2; y++ )
//			{
//				pdc->SetPixelV(x1 + j , y, crStep);
//			}
//		}
//	}
//}
//
//void KDlg_TimePoint_Color::DrawSelectColor(CDC* pdc)
//{
//	CPen PenSelColor(PS_SOLID, 1, 0xff000000);
//	CBrush BrushSelColor(m_crColorSel& 0x00ffffff);
//	CPen* PenSave = pdc->SelectObject(&PenSelColor);
//	CBrush* BrushSave = pdc->SelectObject(&BrushSelColor);
//	pdc->Rectangle(&m_rectSelColor);
//	pdc->SelectObject(BrushSave);
//	pdc->SelectObject(PenSave);
//
//}
//
//void KDlg_TimePoint_Color::OnPaint()
//{
//	CPaintDC dc(this); // device context for painting
//	DrawSelectColor(&dc);
//	m_vecTP[0].crColor = m_vecTP[1].crColor;
//	m_vecTP[m_vecTP.size() - 1].crColor = m_vecTP[m_vecTP.size() - 2].crColor;
//	RenderToBmp(&dc);
//}
//
//void KDlg_TimePoint_Color::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
//{
//	CSliderCtrl* Slider = reinterpret_cast<CSliderCtrl*>(pScrollBar);
//	if ( Slider == &m_ctlSliderRed )
//	{
//		m_iRed = Slider->GetPos();
//	}
//	else if ( Slider == &m_ctlSliderGreen )
//	{
//		m_iGreen = Slider->GetPos();
//	}
//	else if ( Slider == &m_ctlSliderBlue )
//	{
//		m_iBlue = Slider->GetPos();
//	}
//	else if ( Slider == &m_ctlSliderAlpha)
//	{
//		m_iAlpha = Slider->GetPos();
//	}
//	m_crColorSel = RGB(m_iRed, m_iGreen, m_iBlue) | (m_iAlpha << 24);
//	//m_dwColor = m_iAlpha << 24 | m_iRed << 16 | m_iGreen << 8 | m_iBlue;
//	m_vecTP[m_iIndex+1].crColor = m_crColorSel;
//	if ( m_iIndex == 1)
//	{
//		m_vecTP[0].crColor = m_crColorSel;
//	}
//	else if ( m_iIndex == m_vecButton.size() - 1)
//	{
//		m_vecTP[m_vecTP.size() - 1].crColor = m_crColorSel;
//	}
//
//	UpdateData(FALSE);
//	Slider->Invalidate();
//	InvalidateRect(&m_rectSelColor);
//	InvalidateRect(&m_rectColorTP);
//	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
//}
//
//COLORREF KDlg_TimePoint_Color::Conversion(DWORD color)
//{
//	//////////////////////////////////////////////////////////////////////////
//	//color is d3d ARGB format, Conversion is used to conversion it to GDI RGB
//	//Red: (color << 8)>>24
//	//Green: (color << 16) >> 24
//	//Blue: (color << 24) >> 24
//	//////////////////////////////////////////////////////////////////////////
//	COLORREF temp = (( color & 0xff000000) )| //Alpha
//		            (( color & 0x00ff0000) >> 16 ) | //Red
//					(( color & 0x0000ff00) ) | //Green
//					(( color & 0x000000ff) << 16); //Blue
//
//	return temp;
//}
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////
////class KTimePointButton
//
//
//IMPLEMENT_DYNAMIC(KTimePointButton, CButton)
//KTimePointButton::KTimePointButton()
//{
//	m_bSelected = false;
//
//}
//
//KTimePointButton::~KTimePointButton()
//{
//}
//
//
//BEGIN_MESSAGE_MAP(KTimePointButton, CButton)
//	ON_WM_PAINT()
//	ON_WM_MOUSEMOVE()
//	ON_WM_LBUTTONDOWN()
//	ON_WM_LBUTTONUP()
//	ON_CONTROL_REFLECT(BN_CLICKED, OnBnClicked)
//	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, OnBnDoubleclicked)
////	ON_WM_DRAWITEM()
//END_MESSAGE_MAP()
//
//
//
//// KTimePointButton message handlers
//
//
//void KTimePointButton::OnPaint()
//{
//	CPaintDC dc(this); // device context for painting
//	// Do not call CButton::OnPaint() for painting messages
//	DrawButton(&dc);
//
//}
//
//
//
//
//
//BOOL KTimePointButton::Create(CPoint pt, CWnd *pParentWnd, UINT nID)
//{
//
//	m_rectPos.left  = pt.x - m_iRegion - 1;
//	m_rectPos.top = pt.y - 1;
//	m_rectPos.right = pt.x + m_iRegion + 1;
//	m_rectPos.bottom = pt.y + (m_iRegion << 1) + 1;
//	m_ptLeftOrg.x = pt.x - m_rectPos.left;
//	m_ptLeftOrg.y = pt.y - m_rectPos.top;
//	return CButton::Create("", WS_VISIBLE|BS_OWNERDRAW | BS_BITMAP | WS_TABSTOP , m_rectPos, pParentWnd , nID);
//}
//
//
//
//void KTimePointButton::DrawButton(CDC* pdc)
//{
//	CPen pen;
//	CBrush brush;
//	COLORREF crFill;
//	KDlg_TimePoint_Color* pParent = (KDlg_TimePoint_Color*)GetParent();
//	crFill = pParent->m_vecTP[GetDlgCtrlID() - IDC_BUTTON_START + 1].crColor & 0x00ffffff;
//	brush.CreateSolidBrush(crFill);
//	pen.CreatePen(PS_SOLID, 1, crFill);
//	CBrush* pBrush = pdc->SelectObject(&brush);
//	CPen* pPen = pdc->SelectObject(&pen);
//	/*pdc->MoveTo(m_iRegion + 1, 1);
//	pdc->LineTo(1, 2 * m_iRegion + 1 );
//	pdc->LineTo(2 * m_iRegion + 1 , 2 * m_iRegion + 1);
//	pdc->LineTo(m_iRegion + 1, 1);
//	*/
//	pdc->MoveTo(m_iRegion + 1, 2);
//	pdc->LineTo(2, 2 * m_iRegion );
//	pdc->LineTo(2 * m_iRegion, 2 * m_iRegion );
//	pdc->LineTo(m_iRegion + 1, 2);
//	pdc->FloodFill(m_iRegion, m_iRegion, crFill);
//	pdc->SelectObject(pBrush);
//	pdc->SelectObject(pPen);
//
//	//Draw outline
//	if ( m_bSelected )
//	{
//		CPen outline;
//		outline.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
//		pPen = pdc->SelectObject(&outline);
//		pdc->MoveTo(m_iRegion + 1, 1);
//		pdc->LineTo(1, 2 * m_iRegion + 1);
//		pdc->LineTo(2 * m_iRegion + 1, 2 * m_iRegion + 1);
//		pdc->LineTo(m_iRegion + 1, 1);
//		pdc->SelectObject(pPen);
//	}
//}
//
//
//
//void KTimePointButton::OnMouseMove(UINT nFlags, CPoint point)
//{
//	if ( nFlags & MK_LBUTTON )//如果左键按下移动
//	{
//		KDlg_TimePoint_Color* pParent = reinterpret_cast<KDlg_TimePoint_Color*>(GetParent());
//		int iDisX = point.x - m_ptLeftOrg.x;
//		int iaaaaa = pParent->m_vecTP.size();
//		int iCenter = ( m_rectPos.left + m_rectPos.right) / 2;
//		iCenter += iDisX;
//		if ( iCenter < 11 )
//			iCenter = 11;
//		else if ( iCenter > 364 )
//			iCenter = 364;
//		if ( iCenter > 11 && iCenter < 364 )
//		{
//			m_rectPos.left = iCenter - m_iRegion - 1;
//			m_rectPos.right = iCenter + m_iRegion + 1;
//		}
//		iCenter -= 11;
//		MoveWindow(&m_rectPos);
//
//		//Rearrange the sequence of the time points and buttons
//		if ( iCenter < pParent->m_vecTP[(pParent->m_iIndex)].iOffset )
//		{
//			pParent->m_vecTP[pParent->m_iIndex + 1].iOffset = iCenter ;
//			KDlg_TimePoint_Color::TimePoint tp = pParent->m_vecTP[(pParent->m_iIndex) ];
//			pParent->m_vecTP[(pParent->m_iIndex) ] = pParent->m_vecTP[pParent->m_iIndex+1];
//			pParent->m_vecTP[pParent->m_iIndex+1] = tp;
//			KTimePointButton* kpt = pParent->m_vecButton[pParent->m_iIndex - 1];
//			pParent->m_vecButton[pParent->m_iIndex - 1] = pParent->m_vecButton[pParent->m_iIndex];
//			pParent->m_vecButton[pParent->m_iIndex] = kpt;
//			pParent->m_vecButton[pParent->m_iIndex]->SetDlgCtrlID(IDC_BUTTON_START + pParent->m_iIndex);
//			pParent->m_vecButton[pParent->m_iIndex-1]->SetDlgCtrlID(IDC_BUTTON_START + pParent->m_iIndex - 1);
//
//			pParent->m_iIndex --;
//		}
//		else if ( iCenter > pParent->m_vecTP[(pParent->m_iIndex) + 2].iOffset )
//		{
//
//			pParent->m_vecTP[pParent->m_iIndex+1].iOffset = iCenter;
//			KDlg_TimePoint_Color::TimePoint tp = pParent->m_vecTP[(pParent->m_iIndex) + 2];
//			pParent->m_vecTP[(pParent->m_iIndex) + 2] = pParent->m_vecTP[pParent->m_iIndex+1];
//			pParent->m_vecTP[pParent->m_iIndex+1] = tp;
//			KTimePointButton* kpt = pParent->m_vecButton[pParent->m_iIndex + 1];
//			pParent->m_vecButton[pParent->m_iIndex + 1] = pParent->m_vecButton[pParent->m_iIndex];
//			pParent->m_vecButton[pParent->m_iIndex] = kpt;
//
//			pParent->m_vecButton[pParent->m_iIndex]->SetDlgCtrlID(IDC_BUTTON_START + pParent->m_iIndex);
//			pParent->m_vecButton[pParent->m_iIndex+1]->SetDlgCtrlID(IDC_BUTTON_START + pParent->m_iIndex + 1);
//
//			pParent->m_iIndex++;
//
//		}
//		else
//		{
//			pParent->m_vecTP[pParent->m_iIndex+1].iOffset = iCenter;
//		}
//		pParent->m_vecTP[0].crColor = pParent->m_vecTP[1].crColor;
//		pParent->m_vecTP[pParent->m_vecTP.size()-1].crColor = pParent->m_vecTP[pParent->m_vecTP.size()-2].crColor;
//		pParent->InvalidateRect(&pParent->m_rectColorTP);
//		pParent->m_fPosition = (float)pParent->m_vecTP[pParent->m_iIndex+1].iOffset / LIFE_SCREEN_POINT_SIZE;
//		pParent->UpdateData(FALSE);
//
//		if ( pParent->m_iIndex != 0 )
//			pParent->m_vecButton[pParent->m_iIndex-1]->Invalidate();
//		if ( pParent->m_iIndex != pParent->m_vecButton.size() - 1)
//			pParent->m_vecButton[pParent->m_iIndex + 1]->Invalidate();
//		Invalidate();
//
//	}
//
//	CButton::OnMouseMove(nFlags, point);
//}
//
//void KTimePointButton::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	SetCapture();
//	m_bSelected = true;
//	m_ptLeftOrg = point;
//	Invalidate();
//	KDlg_TimePoint_Color* pParent = reinterpret_cast<KDlg_TimePoint_Color*>(GetParent());
//	int iOffset = GetDlgCtrlID() - IDC_BUTTON_START;
//	pParent->ChangeActiveButton(iOffset);
//	CButton::OnLButtonDown(nFlags, point);
//}
//
//
//
//
//void KTimePointButton::OnLButtonUp(UINT nFlags, CPoint point)
//{
//	// TODO: Add your message handler code here and/or call default
//	ReleaseCapture();
//	CButton::OnLButtonUp(nFlags, point);
//
//}
//
//void KTimePointButton::OnBnClicked()
//{
//	// TODO: Add your control notification handler code here
//	m_bSelected = true;
//	//Invalidate();
//}
//
//void KDlg_TimePoint_Color::ChangeActiveButton(int iIndex)
//{
//	m_vecButton[m_iIndex]->SetStatus(FALSE);
//	m_vecButton[iIndex]->SetStatus(TRUE);
//
//	m_iIndex = iIndex;
//	m_fPosition = (float)m_vecTP[m_iIndex+1].iOffset / LIFE_SCREEN_POINT_SIZE;
//	ChangeSelColor();
//
//}
//
//void KDlg_TimePoint_Color::ChangeSelColor(void)
//{
//	//m_iIndex
//	m_crColorSel = m_vecTP[m_iIndex+1].crColor;
//	m_iBlue  = (BYTE)((m_crColorSel<<8)>>24);
//	m_iRed   = (BYTE)((m_crColorSel<<24)>>24);
//	m_iGreen = (BYTE)((m_crColorSel<<16)>>24);
//	m_iAlpha = (BYTE)((m_crColorSel & 0xff000000 )>> 24);
//	m_ctlSliderRed.SetPos(m_iRed);
//	m_ctlSliderBlue.SetPos(m_iBlue);
//	m_ctlSliderGreen.SetPos(m_iGreen);
//	m_ctlSliderAlpha.SetPos(m_iAlpha);
//	InvalidateRect(&m_rectSelColor);
//	UpdateData(FALSE);
//
//}
//
//void KDlg_TimePoint_Color::RenderToBmp(CDC* pdc)
//{
//	CBitmap bmp;
//	bmp.CreateCompatibleBitmap(pdc, LIFE_SCREEN_POINT_SIZE, 23);
//	CDC dc;
//	dc.CreateCompatibleDC(pdc);
//	int iSize = m_vecTP.size();
//	int i;
//	for ( i = 0; i < iSize - 1; i++ )
//	{
//		int x1 = m_vecTP[i].iOffset;
//		int x2 = m_vecTP[i + 1].iOffset;
//		COLORREF crStart = m_vecTP[i].crColor;
//		COLORREF crEnd = m_vecTP[i+1].crColor;
//		int iOffset = x2 - x1;
//		float fRed = (float)((int)(crEnd<<24>>24) - (int)(crStart<<24>>24)) / (float)iOffset;
//		float fGreen = (float)((int)(crEnd<<16>>24) - (int)(crStart<<16>>24)) / (float)iOffset;
//		float fBlue = (float)((int)(crEnd<<8>>24) - (int)(crStart<<8>>24)) / (float)iOffset;
//		float fAlpha = ( float)((int)((crEnd & 0xff000000) >> 24) - (int)((crStart & 0xff000000) >> 24)) / (float)iOffset;
//
//		BYTE RedStart   = (BYTE)(crStart<<24>>24);
//		BYTE GreenStart = (BYTE)(crStart<<16>>24);
//		BYTE BlueStart  = (BYTE)(crStart<<8>>24);
//		BYTE AlphaStart = (BYTE)((crStart & 0xff000000) >> 24);
//
//		if ( x1 == 0 ) x1++;
//		if ( x2 == LIFE_SCREEN_POINT_SIZE ) iOffset--;
//		COLORREF crStepGray;
//		COLORREF crStepWhite;
//		int j;
//		for( j = 0; j < iOffset; j++ )
//		{
//			int iBk = (j + x1) / 5 % 2;
//			float fCurAlpha = (AlphaStart + fAlpha * j) / 255;
//			crStepGray =  (BYTE)((RedStart + fRed * j) * fCurAlpha + 192 * (1.0f - fCurAlpha)) << 16 |
//				      (BYTE)((GreenStart + fGreen * j) * fCurAlpha + 192 * ( 1.0f - fCurAlpha)) << 8 |
//					  (BYTE)((BlueStart + fBlue * j) * fCurAlpha + 192 * ( 1.0f - fCurAlpha));//| 0xff000000;
//			crStepWhite = (BYTE)((RedStart + fRed * j) * fCurAlpha + 255 * (1.0f - fCurAlpha)) << 16 |
//				(BYTE)((GreenStart + fGreen * j) * fCurAlpha + 255 * ( 1.0f - fCurAlpha)) << 8 |
//				(BYTE)((BlueStart + fBlue * j) * fCurAlpha + 255 * ( 1.0f - fCurAlpha));//| 0xff000000;
//			int y;
//			for ( y = 1; y < 22; y++ )
//			{
//				if (  y  % 6 == 0 )
//					iBk = (iBk + 1) %2;
//				if ( iBk)
//					*(m_pTPBarBuffer + x1 + j + y * LIFE_SCREEN_POINT_SIZE) = crStepGray;
//				else
//					*(m_pTPBarBuffer + x1 + j + y * LIFE_SCREEN_POINT_SIZE) = crStepWhite;
//			}
//
//		}
//	}
//	int re = bmp.SetBitmapBits(LIFE_SCREEN_POINT_SIZE * 23 * 4, m_pTPBarBuffer);
//	CBitmap* pOld = dc.SelectObject(&bmp);
//	BITMAP map;
//	bmp.GetObject(sizeof(BITMAP),&map);
//	pdc->BitBlt(m_rectColorTP.left, m_rectColorTP.top, LIFE_SCREEN_POINT_SIZE, 23, &dc, 0, 0, SRCCOPY);
//	dc.SelectObject(pOld);
//
//
//}
//
//void KDlg_TimePoint_Color::OnBnClickedButtonApply()
//{
//	ConvertToTimePointColor();
//}
//
//void KDlg_TimePoint_Color::ConvertToTimePointColor(void)
//{
//	//Convert from m_vecTPColor to Luancher's m_tpColor
//	ModelSFX::KTimePointColor* ptpColor = m_pData;//reinterpret_cast<ModelSFX::KTimePointColor*>(m_lpProperty->Address);
//	ptpColor->Reset();
//	int iSize = m_vecTP.size();
//	int size = ptpColor->Size();
//	int i;
//	for ( i = 0; i < iSize; i++ )
//	{
//		ModelSFX::KTimePointColor::TimePointColor tpc;
//		COLORREF color = m_vecTP[i].crColor;
//		tpc.crColor = ( color& 0xff000000 ) | //alpha
//			          ((color & 0x00ff0000 ) >> 16 ) |
//					   (color & 0x0000ff00) |
//					   ((color & 0x00000ff) << 16 );
//		tpc.fTime = (float)m_vecTP[i].iOffset / LIFE_SCREEN_POINT_SIZE;
//		size = ptpColor->Size();
//		ptpColor->Insert(tpc);
//	}
//}
//
//void KDlg_TimePoint_Color::OnEnChangeEditPosition()
//{
//	int iPos = (int)(m_fPosition * LIFE_SCREEN_POINT_SIZE);
//	MoveTimePoint(m_iIndex, iPos);
//}
//
//void KDlg_TimePoint_Color::MoveTimePoint(int iIndex, int iPos)
//{
//
//}
//
//void KDlg_TimePoint_Color::OnEnChangeEditRed()
//{
//	UpdateData( );
//	m_ctlSliderRed.SetPos(m_iRed);
//	m_crColorSel = (m_crColorSel & 0xffffff00) | m_iRed;
//	m_vecTP[m_iIndex+1].crColor = m_crColorSel;
//	InvalidateRect(&m_rectSelColor);
//	InvalidateRect(&m_rectColorTP);
//}
//
//void KDlg_TimePoint_Color::OnEnChangeEditGreen()
//{
//	UpdateData( );
//	m_ctlSliderRed.SetPos(m_iGreen);
//	m_crColorSel = (m_crColorSel & 0xffff00ff) | (m_iGreen<<8);
//	m_vecTP[m_iIndex+1].crColor = m_crColorSel;
//	InvalidateRect(&m_rectSelColor);
//	InvalidateRect(&m_rectColorTP);
//
//}
//
//void KDlg_TimePoint_Color::OnEnChangeEditBlue()
//{
//	UpdateData( );
//
//	m_ctlSliderRed.SetPos(m_iBlue);
//	m_crColorSel = (m_crColorSel & 0xff00ffff) | (m_iBlue<<16);
//	m_vecTP[m_iIndex+1].crColor = m_crColorSel;
//	InvalidateRect(&m_rectSelColor);
//	InvalidateRect(&m_rectColorTP);
//}
//
//void KDlg_TimePoint_Color::OnEnChangeEditAlpha()
//{
//	UpdateData( );
//	m_ctlSliderRed.SetPos(m_iAlpha);
//	m_crColorSel = (m_crColorSel & 0x00ffffff) | (m_iAlpha << 24);
//	m_vecTP[m_iIndex+1].crColor = m_crColorSel;
//	InvalidateRect(&m_rectSelColor);
//	InvalidateRect(&m_rectColorTP);
//}
//
//void KTimePointButton::OnBnDoubleclicked()
//{
//	KDlg_TimePoint_Color* pPar = (KDlg_TimePoint_Color*)GetParent();
//	CColorDialog dlgColor(pPar->m_crColorSel&0x00ffffff, CC_FULLOPEN|CC_RGBINIT);
//	if ( dlgColor.DoModal() == IDOK )
//	{
//		pPar->m_crColorSel = dlgColor.GetColor();
//		pPar->m_iBlue  = (BYTE)((pPar->m_crColorSel<<8)>>24);
//		pPar->m_iRed   = (BYTE)((pPar->m_crColorSel<<24)>>24);
//		pPar->m_iGreen = (BYTE)((pPar->m_crColorSel<<16)>>24);
//		pPar->m_iAlpha = (BYTE)(0xff);
//		pPar->m_ctlSliderRed.SetPos(pPar->m_iRed);
//		pPar->m_ctlSliderGreen.SetPos(pPar->m_iGreen);
//		pPar->m_ctlSliderBlue.SetPos(pPar->m_iBlue);
//		pPar->m_ctlSliderAlpha.SetPos(pPar->m_iAlpha);
//		pPar->m_vecTP[pPar->m_iIndex+1].crColor = pPar->m_crColorSel | 0xff000000;
//		pPar->UpdateData(FALSE);
//		pPar->Invalidate();
//	}
//
//}
//
//void KDlg_TimePoint_Color::OnLButtonDown(UINT nFlags, CPoint point)
//{
//
//	if (PointInRect(point, m_rectEffReg))
//	{
//		AddButton(point);
//	}
//}
//
//bool KDlg_TimePoint_Color::PointInRect(CPoint point, RECT& rect)
//{
//	if ( point.y < rect.bottom && point.y > rect.top && point.x < rect.right && point.x > rect.left )
//		return true;
//	else
//		return false;
//}
//
//void KDlg_TimePoint_Color::AddButton(CPoint pt, COLORREF color)
//{
//	POINT ptBt = { pt.x, 49};
//	KTimePointButton* pbt = new KTimePointButton;
//	int iSize = m_vecButton.size();
//	int i;
//	int j;
//	for ( i = 0;  i < iSize; i++ )
//	{
//		if ( ( m_vecButton[i]->m_rectPos.left + m_vecButton[i]->m_rectPos.right) / 2 >= pt.x)
//			break;
//	}
//    for ( j = i; j < iSize ; j++)
//	{
//		m_vecButton[j]->SetDlgCtrlID(m_vecButton[j]->GetDlgCtrlID() + 1);
//	}
//
//	vector<KTimePointButton*>::iterator itbt = m_vecButton.begin();
//	vector<TimePoint>::iterator ittp = m_vecTP.begin();
//	TimePoint tp = { 0xffffffff, pt.x - 11};
//	pbt->Create(ptBt, this, IDC_BUTTON_START + i);
//	advance(itbt, i);
//	advance(ittp, i+1);
//	m_vecButton.insert(itbt, pbt);
//	m_vecTP.insert(ittp, tp);
//
//
//	if ( m_iIndex >= i )
//		m_iIndex++;
//	ChangeActiveButton(i);
//	InvalidateRect(&m_rectEffReg);
//}
//
//void KDlg_TimePoint_Color::OnMouseMove(UINT nFlags, CPoint point)
//{
//	if ( PointInRect( point, m_rectEffReg ) )
//	{
//		SetCursor(m_curAdd);
//	}
//	else
//	{
//		SetCursor(m_curNormal);
//	}
//}
//
//void KTimePointButton::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
//{
//	//Do not remove this
//}
//
//}
//
//void PropertyBase::KDlg_TimePoint_Color::PostNcDestroy()
//{
//	delete this;
//}
//
//void PropertyBase::KDlg_TimePoint_Color::OnCancel()
//{
//	DestroyWindow();
//}
//
//void PropertyBase::KDlg_TimePoint_Color::OnOK()
//{
//	CDialog::OnOK();
//	DestroyWindow();
//}
