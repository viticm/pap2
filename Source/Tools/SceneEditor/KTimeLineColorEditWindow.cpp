#include "stdafx.h"
//#include "KTimeLineColorEditWindow.h"
//#include "KColorDialog.h"
//#include "sceneeditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

//
//
//const int KTimeLineColorEditBar::m_nGridSize = 6;
//const int KTimeLineColorEditBar::m_nSpan = 2;
//const RECT KTimeLineColorEditBar::m_rectButtonRgn = {0, 0, 11, 20};
//
//
//
//IMPLEMENT_DYNAMIC(KTimeLineColorEditBar, CWnd)
//KTimeLineCo   lorEditBar::KTimeLineColorEditBar(KTimeLine<float, DWORD>* pTimeLine,
//											 CWnd* pParent,
//											 const RECT& rectWindow,
//											 const RECT& rectBar,
//											 const RECT* prectDrop /*= NULL*/)
//{
//	m_pColorBarBuffer = NULL;
//	ASSERT(pTimeLine);
//	m_pTimeLine = pTimeLine;
//	m_bDirty = false;
//	m_nSelectButton = -1;
//	m_pBmpBar = NULL;
//	m_pBackBuffer = NULL;
//	m_bVerneirsMove = TRUE;
//	Create(pParent, rectWindow, rectBar, prectDrop);
//}
//
//KTimeLineColorEditBar::~KTimeLineColorEditBar()
//{
//	SAFE_DELETE_ARRAY(m_pColorBarBuffer);
//	SAFE_DELETE(m_pBmpBar);
//	SAFE_DELETE(m_pBackBuffer);
//	SAFE_DELETE(m_bmpBackBuffer);
//	m_vecKeys.clear();
//}
//
//BOOL KTimeLineColorEditBar::Create(CWnd* pParent, const RECT& rectWindow, const RECT& rectBar, const RECT* prectDrop /*= NULL*/)
//{
//	BOOL bCreate = FALSE;
//	if (!pParent)//Must have a parent window and the key name is validate
//		return FALSE;
//	SetWindowRect(rectWindow);
//	SetColorBarRect(rectBar);
//	prectDrop ? SetDropRegion(*prectDrop) : SetDropRegion(rectWindow);
//	m_rectInsertRgn = m_rectBarRegion;
//	m_rectInsertRgn.bottom = m_rectInsertRgn.bottom + m_nSpan + m_rectButtonRgn.bottom;
//	UINT uNumKeys = m_pTimeLine->GetNumFrames();
//	KTimeLine<float, DWORD>::FrameData* pKeys = new KTimeLine<float, DWORD>::FrameData[uNumKeys];
//	m_pTimeLine->GetKeyFrameRangeByIndex(0, uNumKeys, pKeys);
//	m_vecKeys.insert(m_vecKeys.begin(), pKeys, pKeys + uNumKeys);
//	RECT rectClearRgn = { m_rectBarRegion.left - m_rectButtonRgn.right / 2 - 1,
//		m_rectBarRegion.bottom + 1,
//		m_rectBarRegion.right + m_rectButtonRgn.right / 2 + 1,
//		m_rectBarRegion.bottom + m_rectButtonRgn.bottom + 2 };
//	m_rectClearRgn = rectClearRgn;
//	bCreate = CWnd::Create(NULL, "Color Edit Bar", WS_CHILD | WS_VISIBLE, rectWindow, pParent, 9999);
//	//Initialize controls
//	OnInit();
//	SAFE_DELETE_ARRAY(pKeys);
//	return bCreate;
//}
//
//inline void KTimeLineColorEditBar::SetColorBarRect(const RECT& rectBarRegion)
//{
//	m_rectBarRegion = rectBarRegion;
//	SAFE_DELETE(m_pColorBarBuffer);
//	int nWidth = abs(m_rectBarRegion.right - m_rectBarRegion.left) - 2;
//	int nHeight = abs(m_rectBarRegion.bottom - m_rectBarRegion.top) - 2;
//	m_pColorBarBuffer = new DWORD[nWidth * nHeight];
//}
//
//
//void KTimeLineColorEditBar::DrawColorBar(CDC* pdc)
//{
//	int nWidth = abs(m_rectBarRegion.right - m_rectBarRegion.left);
//	int nHeight = abs(m_rectBarRegion.bottom - m_rectBarRegion.top);
//	CPen Pen(PS_SOLID, 1, RGB(0, 0, 0));
//	CPen* pPenOld = pdc->SelectObject(&Pen);
//	RenderColorBar(pdc, m_DrawStyle);
//	if ( m_bVerneirsMove )
//	{
//		ClearRegion(pdc, m_rectClearRgn);
//	}
//	UINT uNumKeys = m_pTimeLine->GetNumFrames();
//	POINT pt;
//	pt.y = m_rectBarRegion.bottom + m_nSpan;
//	for ( UINT i = 0; i < uNumKeys; i++ )
//	{
//		pt.x = TimeToCoordinate(m_vecKeys[i].Time);
//		DWORD dwColor = *m_pTimeLine->GetSingleKeyFrame(m_vecKeys[i].Time);
//		DrawButton(pdc, pt, RGB(GETRED(dwColor), D3DCOLOR_GETGREEN(dwColor), D3DCOLOR_GETBLUE(dwColor)));
//	}
//	m_bVerneirsMove = FALSE;
//	pdc->SelectObject(pPenOld);
//}
//
//void KTimeLineColorEditBar::RenderColorBar(CDC* pdc, enumDrawStyle DrawStyle)
//{
//	int nWidth = abs(m_rectBarRegion.right - m_rectBarRegion.left) - 2;
//	int nHeight = abs(m_rectBarRegion.bottom - m_rectBarRegion.top) - 2;
//	if ( !m_pBmpBar )
//	{
//		m_pBmpBar =new CBitmap;
//		m_pBmpBar->CreateCompatibleBitmap(pdc, nWidth, nHeight);
//	}
//	float fMaxTime = 1.0f;
//	int nFlagX = 0;
//	int nFlagY = 0;
//
//	for ( int i = 0; i < nWidth; i++ )
//	{
//		if (( i / m_nGridSize ) % 2 )
//			nFlagX = 1;
//		else
//			nFlagX = 0;
//		float fLocation = static_cast<float>(i) / static_cast<float>(nWidth);
//		DWORD dwColor = 0xffffffff;
//		m_pTimeLine->GetCurrentFrame(fLocation, dwColor);
//		float fAlpha = ((dwColor & 0xff000000)>>24) / 255.0f;
//		BYTE byGrey = static_cast<BYTE>(192 * (1.0f - fAlpha));
//		BYTE byWhite = static_cast<BYTE>(255 * (1.0f - fAlpha));
//
//		DWORD dwColorBKGrey = static_cast<DWORD>(GETRED(dwColor) * fAlpha +  byGrey) << 16  |//Red Channel
//			static_cast<DWORD>(D3DCOLOR_GETGREEN(dwColor) * fAlpha + byGrey) << 8  |//Green Channel
//			static_cast<DWORD>(D3DCOLOR_GETBLUE(dwColor) * fAlpha + byGrey)        &//Blue Channel
//			0x00ffffff;                                                    //Alpha Channel, must be 0 for GDI
//		DWORD dwColorBKWhite = static_cast<DWORD>(GETRED(dwColor) * fAlpha +  byWhite) << 16 |//Red Channel
//			static_cast<DWORD>(D3DCOLOR_GETGREEN(dwColor) * fAlpha + byWhite) << 8 |//Green Channel
//			static_cast<DWORD>(D3DCOLOR_GETBLUE(dwColor) * fAlpha + byWhite)       &//Blue Channel
//			0x00ffffff;                                                    //Alpha Channel, must be 0 for GDI
//		for( int j = 0; j < nHeight; j++ )
//		{
//			if (( nFlagX + j / m_nGridSize ) % 2)
//				*(m_pColorBarBuffer + i + j * nWidth) = dwColorBKGrey;
//			else
//				*(m_pColorBarBuffer + i + j * nWidth) = dwColorBKWhite;
//		}
//	}
//	int nReturnCode = m_pBmpBar->SetBitmapBits(nWidth * nHeight * 4, m_pColorBarBuffer);
//	CBrush Brush(m_pBmpBar);
//	CBrush* pOldBrush = pdc->SelectObject(&Brush);
//	pdc->SetBrushOrg(m_rectBarRegion.left + 1, m_rectBarRegion.top + 1);
//	pdc->Rectangle(&m_rectBarRegion);
//	pdc->SelectObject(pOldBrush);
//
//}
//
//BEGIN_MESSAGE_MAP(KTimeLineColorEditBar, CWnd)
//	ON_WM_CREATE()
//	ON_WM_PAINT()
//	ON_WM_LBUTTONDOWN()
//	ON_WM_LBUTTONUP()
//	ON_WM_MOUSEMOVE()
//END_MESSAGE_MAP()
//
//
//void KTimeLineColorEditBar::OnPaint()
//{
//	CPaintDC dc(this);
//	BOOL bReturn;
//	int nWidth = m_rectWindow.right - m_rectWindow.left;
//	//int nHeight = m_rectWindow.bottom - m_rectWindow.top;
//	int nHeight = m_rectBarRegion.bottom - m_rectBarRegion.left + m_nSpan + m_rectButtonRgn.bottom + 2;
//	if ( !m_pBackBuffer )
//	{
//		m_bmpBackBuffer = new CBitmap;
//		m_pBackBuffer = new CDC;
//		bReturn = m_pBackBuffer->CreateCompatibleDC(&dc);
//		m_bmpBackBuffer->CreateCompatibleBitmap(&dc, nWidth, nHeight);
//		m_pBackBuffer->SelectObject(m_bmpBackBuffer);
//		m_pBackBuffer->FillSolidRect(0,0, nWidth, nHeight, dc.GetPixel(0, 0));
//	}
//	DrawColorBar(m_pBackBuffer);
//
//	bReturn = dc.BitBlt(0, m_rectBarRegion.top, nWidth, nHeight, m_pBackBuffer, 0, 0, SRCCOPY);
//
//}
//
//void KTimeLineColorEditBar::DrawButton(CDC* pdc, POINT& pt, COLORREF crColor)
//{
//	if ( crColor == 0x0 )//折中的办法，因为使用floodfill， 所以不能让按钮成为黑色（0x0）
//		crColor = 0x1;
//	RECT rect = { pt.x - m_rectButtonRgn.right / 2, pt.y - m_rectButtonRgn.bottom / 2, pt.x + m_rectButtonRgn.right / 2, pt.y + m_rectButtonRgn.bottom/2 };
//	CPen Pen(PS_SOLID, 1, RGB(0, 0, 0));
//	CBrush Brush(crColor);
//	CPen* pOldPen = pdc->SelectObject(&Pen);
//	CBrush* pOldBrush = pdc->SelectObject(&Brush);
//	pdc->MoveTo(pt.x, pt.y);
//	pdc->LineTo(rect.right, rect.bottom);
//	pdc->LineTo(rect.left, rect.bottom);
//	pdc->LineTo(pt.x, pt.y);
//	pdc->FloodFill(pt.x, pt.y + 4, RGB(0, 0, 0));
//	pdc->SelectObject(pOldBrush);
//	pdc->SelectObject(pOldPen);
//}
//
//int KTimeLineColorEditBar::IsBounceRegion(POINT& pt)
//{
//	int nReturn = -1;
//	if ( m_vecKeys.size() )
//	{
//		int nWidth = abs(m_rectBarRegion.right - m_rectBarRegion.left) - 2;
//		RECT rect = {0, m_rectBarRegion.bottom + m_nSpan, 0, m_rectBarRegion.bottom + m_nSpan + m_rectButtonRgn.bottom};
//		UINT uNumKeys = m_pTimeLine->GetNumFrames();
//		for ( UINT i = 0; i < uNumKeys; i++ )
//		{
//			//int nX = static_cast<int>(m_pKeys[i].Time * (nWidth - 2) + m_rectBarRegion.left + 1);
//			int nX = TimeToCoordinate(m_vecKeys[i].Time);
//			rect.left = nX - m_rectButtonRgn.right / 2;
//			rect.right = nX + m_rectButtonRgn.right / 2;
//			if ( ::PtInRect(&rect, pt) )
//			{
//				nReturn = i;
//				break;
//			}
//		}
//	}
//	return nReturn;
//}
//
//void KTimeLineColorEditBar::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	TCHAR strValule[14];
//	SetCapture();
//	m_nSelectButton = IsBounceRegion(point);
//	//If no key is selected and is in the insert region, insert a new key with white color
//	if ( m_nSelectButton == -1 )
//	{
//		if( ::PtInRect(&m_rectInsertRgn, point) )
//		{
//			float fTime = CoordinateToTime(point.x);
//			AddKeyColor(fTime, 0xff0000ff);
//			m_nSelectButton = static_cast<int>(m_vecKeys.size()) - 1;
//			for(int i = 0; i < 4; i++ )
//			{
//				int nPos = (0xff0000ff&(0xff<<(i<<3)))>>(i<<3);
//				m_ctlSliderChannel[i].SetPos(nPos);
//				sprintf(strValule, "%3d", nPos);
//				m_ctlEditChannel[i].SetWindowText(strValule);
//			}
//		}
//
//	}
//	else
//	{
//		DWORD dwColor = *m_pTimeLine->GetSingleKeyFrame(m_vecKeys[m_nSelectButton].Time);
//		for(int i = 0; i < 4; i++ )
//		{
//
//			int nPos = (dwColor&(0x000000ff<<(i<<3)))>>(i<<3);
//			m_ctlSliderChannel[i].SetPos(nPos);
//			sprintf(strValule, "%3d", nPos);
//			m_ctlEditChannel[i].SetWindowText(strValule);
//		}
//	}
//	Invalidate();
//	CWnd::OnLButtonDown(nFlags, point);
//}
//
//int KTimeLineColorEditBar::AddKeyColor(float fTime, DWORD dwColor)
//{
//	int nReturn = 0;
//	nReturn = m_pTimeLine->AddFrame(fTime, dwColor);
//	DWORD* pColor = m_pTimeLine->GetSingleKeyFrame(fTime);
//	//m_pTimeLine->PrintAll();
//	if ( pColor )
//	{
//		KTimeLine<float, DWORD>::FrameData Keys = {fTime, pColor};//RGB(GETRED(dwColor), D3DCOLOR_GETGREEN(dwColor), D3DCOLOR_GETBLUE(dwColor))};
//		m_vecKeys.push_back(Keys);
//	}
//	else
//	{
//		nReturn = ERR_KEYDONTEXSIT;
//	}
//	return nReturn;
//}
//
//int KTimeLineColorEditBar::RemoveSelectedKeyColor()
//{
//	int nReturn = 0;
//	nReturn = m_pTimeLine->RemoveFrame(m_vecKeys[m_nSelectButton].Time);
//	return nReturn;
//}
//
//float KTimeLineColorEditBar::CoordinateToTime(int nX)
//{
//	float fWidth = abs(m_rectBarRegion.right - m_rectBarRegion.left) - 2.0f;
//	return (nX - m_rectBarRegion.left - 1) / fWidth;
//}
//
//int KTimeLineColorEditBar::TimeToCoordinate(float fTime)
//{
//	int nWidth = abs(m_rectBarRegion.right - m_rectBarRegion.left) - 2;
//	return static_cast<int>(fTime * (nWidth - 2) + m_rectBarRegion.left + 1);
//}
//
//void KTimeLineColorEditBar::OnLButtonUp(UINT nFlags, CPoint point)
//{
//	if ( m_nSelectButton != -1 )
//	{
//		if (!::PtInRect(&m_rectDropRegion, point))
//		{
//			//Drop selected key
//			vector<KTimeLine<float, DWORD>::FrameData>::iterator it = m_vecKeys.begin();
//			m_pTimeLine->RemoveFrame(m_vecKeys[m_nSelectButton].Time);
//			std::advance(it, m_nSelectButton);
//			m_vecKeys.erase(it);
//			m_bVerneirsMove = TRUE;
//		}
//		GetParent()->Invalidate();
//	}
//	//m_nSelectButton = -1;
//	ReleaseCapture();
//	CWnd::OnLButtonUp(nFlags, point);
//}
//
//void KTimeLineColorEditBar::OnMouseMove(UINT nFlags, CPoint point)
//{
//	if ( ::PtInRect(&m_rectInsertRgn, point) )
//	{
//		SetCursor(::LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSOR_ADD)));
//	}
//	else
//	{
//		SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
//	}
//	if ( m_nSelectButton != -1 )
//	{
//		if ( nFlags & MK_LBUTTON )
//		{
//			m_bVerneirsMove = TRUE;
//			//if( (point.x >= m_rectBarRegion.left + 1) && (point.x <= m_rectBarRegion.right))
//			{
//				int nX = max(min(m_rectBarRegion.right-1, point.x), m_rectBarRegion.left+1);
//				float fTime = CoordinateToTime(nX);
//				//Avoid overwriting existed key frame
//				float fStep = 100000.0f;
//				while ( m_pTimeLine->ValidateKey(fTime) )
//				{
//					fTime += 1.0f / fStep;
//					fStep *= 10.0f;
//				}
//				m_pTimeLine->MoveFrame(m_vecKeys[m_nSelectButton].Time, fTime);
//				m_vecKeys[m_nSelectButton].Time = fTime;
//				Invalidate();
//				if ( !::PtInRect(&m_rectDropRegion, point) )
//				{
//					SetCursor(::LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSOR_DEL)));
//				}
//			}
//		}
//	}
//	CWnd::OnMouseMove(nFlags, point);
//}
//
//void KTimeLineColorEditBar::ChangeColor()
//{
//	DWORD dwNewColor;
//	m_pTimeLine->SetFrameValue(m_vecKeys[m_nSelectButton].Time, dwNewColor);
//}
//
//void KTimeLineColorEditBar::ClearRegion(CDC* pDc, RECT& rect)
//{
//	//Assume there is nothing in the  left, upper corner
//	COLORREF crBK = pDc->GetPixel(0, 0);
//	CBrush Brush(crBK);
//	CPen Pen(PS_NULL, 1, crBK);
//	CPen* pOldPen = pDc->SelectObject(&Pen);
//	CBrush* pOldBrush = pDc->SelectObject(&Brush);
//	pDc->Rectangle(&rect);
//	pDc->SelectObject(pOldBrush);
//	pDc->SelectObject(pOldBrush);
//}
//
//BOOL KTimeLineColorEditBar::OnInit()
//{
//	BOOL bReturn = TRUE;
//
//	TCHAR *strStatic[] = { "B:", "G:", "R:", "A:"};
//	RECT rectStatic = { max(0, m_rectBarRegion.left - 5), m_rectBarRegion.bottom + m_nSpan + m_rectButtonRgn.bottom + 2, rectStatic.left + 15, rectStatic.top + 20 };
//	RECT rectSlider = { rectStatic.right + 5, rectStatic.top, rectSlider.left + 255, rectStatic.bottom};
//	RECT rectEdit   = { rectSlider.right + 5, rectStatic.top, rectEdit.left + 60, rectStatic.bottom - 5};
//	DWORD dwStyle = WS_CHILD | WS_VISIBLE;
//
//	TCHAR strValue[4];
//	for(int i = 0; i< 4; i++ )
//	{
//		bReturn &= m_ctlStaticChannel[i].Create(strStatic[i], dwStyle, rectStatic, this);
//		bReturn &= m_ctlSliderChannel[i].Create(dwStyle|TBS_NOTICKS , rectSlider, this, i * 10);
//		bReturn &= m_ctlEditChannel[i].Create(dwStyle|ES_RIGHT|WS_BORDER, rectEdit, this, i * 10 + 1);
//
//		m_ctlSliderChannel[i].SetRange(0, 255);
//		DWORD dwColor = 0xffffffff;
//		//if ( m_pTimeLine->GetNumFrames() )
//		{
//			//	dwColor = m_vecKeys[0].Frame;
//		}
//		BYTE Channel = static_cast<BYTE>(dwColor>>(i<<3));
//		m_ctlSliderChannel[i].SetPos(Channel);
//		sprintf(strValue, "%3d", Channel);
//		m_ctlEditChannel[i].SetWindowText(strValue);
//		rectStatic.bottom += 20;
//		rectStatic.top += 20;
//		rectSlider.top += 20;
//		rectSlider.bottom += 20;
//		rectEdit.top += 21;
//		rectEdit.bottom += 21;
//	}
//	return bReturn;
//}
//
//
//LRESULT KTimeLineColorEditBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	switch(message)
//	{
//	case WM_HSCROLL:
//		{
//			if ( m_nSelectButton != -1 )//在有游标被选中的时候才可以更改
//			{
//				int i = 0;
//				while (reinterpret_cast<HWND>(lParam) != m_ctlSliderChannel[i].m_hWnd && i < 4 )
//					i++;
//				if ( i < 4 )
//				{
//					TCHAR strValue[4];
//					DWORD dwPos =m_ctlSliderChannel[i].GetPos();//得到位置
//					sprintf(strValue, "%3d", dwPos);
//					//(*m_vecKeys[m_nSelectButton].Frame) &= (~(0x000000ff << (i<<3)));//转换成颜色
//					//(*m_vecKeys[m_nSelectButton].Frame) |= (dwPos << (i<<3));
//					DWORD* pdwColor = m_pTimeLine->GetSingleKeyFrame(m_vecKeys[m_nSelectButton].Time);
//					*pdwColor &= (~(0x000000ff << (i<<3)));//转换成颜色
//					*pdwColor |= (dwPos << (i<<3));
//					m_ctlEditChannel[i].SetWindowText(strValue);
//					Invalidate();
//				}
//				return S_OK;
//			}
//		}
//		break;
//	case WM_LBUTTONDBLCLK:
//		{
//			POINT pt  = { LOWORD(lParam), HIWORD(lParam)};
//			int nSelectVernier = IsBounceRegion(pt);
//			if ( nSelectVernier != -1 )
//			{
//				GetColorPickerDialog()->Init(m_pTimeLine->GetSingleKeyFrame(m_vecKeys[nSelectVernier].Time), "", this, TRUE);
//			}
//			return S_OK;
//		}
//		break;
//	}
//	return CWnd::WindowProc(message, wParam, lParam);
//}
//
//KColorBarDialog::KColorBarDialog(KTimeLine<float, DWORD>* pTimeLine, CWnd* pParent/* = NULL */)
//{
//	m_pTimeLine = pTimeLine;
//	Create(IDD_DIALOG_COLOR_BAR, pParent);
//}
//
//KColorBarDialog::~KColorBarDialog()
//{
//
//}
//
//
//BEGIN_MESSAGE_MAP(KColorBarDialog, CDialog)
//
//END_MESSAGE_MAP()
//
//void KColorBarDialog::OnCancel()
//{
//	DestroyWindow();
//}
//
//void KColorBarDialog::OnOK()
//{
//}
//
//BOOL KColorBarDialog::OnInitDialog()
//{
//	CDialog::OnInitDialog();
//	RECT rectWindow = { 10, 10, 500, 350};
//	RECT rectBar = { 10, 2, 280, 25 };
//	RECT rectDrop = { 0, 2, 500, 75 };
//	KTimeLineColorEditBar* pBar = new KTimeLineColorEditBar(m_pTimeLine, this, rectWindow, rectBar, &rectDrop);
//	ShowWindow(SW_SHOW);
//	return TRUE;
//}
//
//void KColorBarDialog::PostNcDestroy()
//{
//	delete this;
//}
//
//void KTimeLineColorEditBar::PostNcDestroy()
//{
//	// TODO: Add your specialized code here and/or call the base class
//
//	delete this;
//}
