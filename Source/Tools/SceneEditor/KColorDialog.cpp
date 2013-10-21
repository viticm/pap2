// KColorPickerDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "SceneEditor.h"
#include "KColorDialog.h"
#include <math.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifndef D3DCOLOR_SETALPHA
#define D3DCOLOR_SETALPHA(SRC, ALPHA) (SRC = (((SRC)&0x00ffffff)|((ALPHA)<<24)))
#endif

const INT KColorPickerDialog::s_nSpan = 2;
const POINT KColorPickerDialog::s_ptVernier = {10, 11};
const RECT KColorPickerDialog::s_rectColor1 = { 9,  9, 267, 267};
const RECT KColorPickerDialog::s_rectColor2 = { 286, 9, 312, 267};
//const INT KColorPickerDialog::s_nWidth2 = abs(s_rectColor2.right - s_rectColor2.left - 2);
//const RECT KColorPickerDialog::m_rectAlpha = {s_rectColor1.left, s_rectColor1.bottom + 2, s_rectColor1.right, s_rectColor1.left + 17};
//const RECT KColorPickerDialog::m_rectAlphaRgn = { m_rectAlpha.left, m_rectAlpha.top, m_rectAlpha.right
//								, m_rectAlpha.bottom + s_ptVernier.y + s_nSpan + 1 };
const RECT KColorPickerDialog::s_rectVernierRgn = { s_rectColor2.left - s_ptVernier.x - s_nSpan,
									s_rectColor2.top + 1,s_rectColor2.right + s_ptVernier.x + s_nSpan, s_rectColor2.bottom - 1 };
const RECT KColorPickerDialog::m_rectClearRgn[3] = 
{	
	{ s_rectColor2.left - s_nSpan - s_ptVernier.x - 1, max(s_rectColor2.top - 6, 0),
		s_rectColor2.left - s_nSpan + 1, s_rectColor2.bottom + 6},
	{ s_rectColor2.right + s_nSpan - 1, m_rectClearRgn[0].top,
		s_rectColor2.right + s_nSpan - 1 + s_ptVernier.x + 2, m_rectClearRgn[0].bottom },
	/*{ max(m_rectAlpha.left - s_ptVernier.x/2 - 1, 0), m_rectAlpha.bottom + 1,
		m_rectAlpha.right + s_ptVernier.x /2 + 1, m_rectAlpha.bottom + s_ptVernier.y + s_nSpan + 1}*/
	{0,0,0,0},
};
const RECT KColorPickerDialog::m_rectSelectColor = { 325, 9, 405, 49};
const RECT KColorPickerDialog::m_rectOriginColor = { m_rectSelectColor.left, m_rectSelectColor.bottom
													, m_rectSelectColor.right, m_rectSelectColor.bottom + 39};
const RECT KColorPickerDialog::m_rectSelAndOrg = { m_rectSelectColor.left, m_rectSelectColor.top
													, m_rectOriginColor.right, m_rectOriginColor.bottom };
static INT s_LastX = 0;
static INT s_LastY = 0;
////////////////////////////////////////////////////////////////////////////////
#define GETRED(SRC)   ((SRC&0x00ff0000)>>16)
#define GETGREEN(SRC) ((SRC&0x0000ff00)>> 8)
#define GETBLUE(SRC)  (SRC&0x000000ff)
#define GETALPHA(SRC) (SRC>>24)

#define GETRED_BYTE(src) (static_cast<BYTE>(GETRED(src)))
#define GETGREEN_BYTE(src) (static_cast<BYTE>(GETGREEN(src)))
#define GETBLUE_BYTE(src) (static_cast<BYTE>(GETBLUE(src)))
#define GETALPHA_BYTE(src) (static_cast<BYTE>(GETALPHA(src)))

KColorPickerDialog g_ColorPicker;

KColorPickerDialog* GetColorPickerDialog()
{
	return &g_ColorPicker;
}

IMPLEMENT_DYNAMIC(KColorPickerDialog, CDialog)

BEGIN_MESSAGE_MAP(KColorPickerDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_RADIO_H, OnBnClickedRadioH)
	ON_BN_CLICKED(IDC_RADIO_S, OnBnClickedRadioS)
	ON_BN_CLICKED(IDC_RADIO_B, OnBnClickedRadioB)
	ON_BN_CLICKED(IDC_RADIO_RED, OnBnClickedRadioRed)
	ON_BN_CLICKED(IDC_RADIO_GREEN, OnBnClickedRadioGreen)
	ON_BN_CLICKED(IDC_RADIO_BLUE, OnBnClickedRadioBlue)
	ON_EN_CHANGE(IDC_EDIT_H, &KColorPickerDialog::OnEnChangeEditH)
	ON_EN_CHANGE(IDC_EDIT_S, &KColorPickerDialog::OnEnChangeEditS)
	ON_EN_CHANGE(IDC_EDIT_B, &KColorPickerDialog::OnEnChangeEditV)
	ON_EN_CHANGE(IDC_EDIT_RED, &KColorPickerDialog::OnEnChangeEditR)
	ON_EN_CHANGE(IDC_EDIT_GREEN, &KColorPickerDialog::OnEnChangeEditG)
	ON_EN_CHANGE(IDC_EDIT_BLUE, &KColorPickerDialog::OnEnChangeEditB)
	ON_EN_CHANGE(IDC_EDIT_ALPHA, &KColorPickerDialog::OnEnChangeEditA)
	ON_EN_CHANGE(IDC_EDIT_ARGB, &KColorPickerDialog::OnEnChangeEditARGB)
	ON_EN_CHANGE(IDC_EDIT_ARGB_HEX, &KColorPickerDialog::OnEnChangeEditARGBHEX)
	ON_EN_KILLFOCUS(IDC_EDIT_ARGB, &KColorPickerDialog::OnEnKillfocusEditArgb)
	ON_EN_KILLFOCUS(IDC_EDIT_ARGB_HEX, &KColorPickerDialog::OnEnKillfocusEditArgbHEX)
END_MESSAGE_MAP()

KColorPickerDialog::KColorPickerDialog()
	:m_hNotifyHWND(NULL)	
	,m_pBindedColor(NULL)
	,m_pColorValueBinded(NULL)
	,m_dwColor(D3DCOLOR_ARGB(0,0,0,0))
	,m_dwColorOld(D3DCOLOR_ARGB(0,0,0,0))
{
	//m_bCreated = FALSE;
	//m_bUpdateBmp1 = TRUE;
	//m_bUpdateBmp2 = TRUE;
	//m_pbmpColor1 = NULL;
	//m_pbmpColor2 = NULL;
	//m_pBmpAlpha = NULL;
	//m_pAlphaBuffer = NULL;
	//m_pColorBuffer1 = new DWORD[em_color_pallete_size * em_color_pallete_size];
	//m_pColorBuffer2 = new DWORD[em_color_pallete_size];
}

void KColorPickerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if (! pDX->m_bSaveAndValidate)
	{
		DWORD curColor = GetCurrentColor();
		BYTE byRed = GETRED_BYTE(curColor);
		BYTE byGreen = GETGREEN_BYTE(curColor);
		BYTE byBlue = GETBLUE_BYTE(curColor);
		BYTE byAlpha = GETALPHA_BYTE(curColor);

		DDX_Text(pDX, IDC_EDIT_RED, byRed);
		DDX_Text(pDX, IDC_EDIT_GREEN, byGreen);
		DDX_Text(pDX, IDC_EDIT_BLUE, byBlue);
		DDX_Text(pDX, IDC_EDIT_ALPHA, byAlpha);

		FLOAT H = m_HSV[0];	

		//两个都是百分比，要乘100
		FLOAT S = m_HSV[1] * 100.f;
		FLOAT V = m_HSV[2] * 100.f;
		DDX_Text(pDX, IDC_EDIT_H, H);
		DDX_Text(pDX, IDC_EDIT_S, S);
		DDX_Text(pDX, IDC_EDIT_B, V);	

		CString cs;
		if (m_bAlphaEnable)
		{
			cs.Format(_T("%x"), curColor);
		}
		else
		{
			cs.Format(_T("%x"), curColor & 0x00ffffff);
		}

		DDX_Text(pDX, IDC_EDIT_ARGB_HEX, cs);

		if (m_bAlphaEnable)
		{
			cs.Format(_T("%u,%u,%u,%u"), GETALPHA(curColor), GETRED(curColor), GETGREEN(curColor), GETBLUE(curColor));
		}
		else
		{
			cs.Format(_T("%u,%u,%u"), GETRED(curColor), GETGREEN(curColor), GETBLUE(curColor));
		}

		DDX_Text(pDX, IDC_EDIT_ARGB, cs);	
	}
	
	DDX_Control(pDX, IDC_SLIDER_ALPHA, m_SliderAlpha);
}

void KColorPickerDialog::Init(DWORD* pColor, LPCTSTR strCaption, CWnd* pNotify/* = NULL */, BOOL bAlphaEnable /* = TRUE */)
{
	UnBindAllColorPointers();
	m_pBindedColor = pColor;
	_Init(*pColor, strCaption, pNotify, bAlphaEnable);
}
void KColorPickerDialog::Init( D3DCOLORVALUE* pColor , LPCTSTR strCaption, CWnd* pNotify /*= NULL*/, BOOL bAlphaEnable /*= TRUE*/ )
{
	UnBindAllColorPointers();
	m_pColorValueBinded = pColor;
	D3DCOLOR color = ColorValue(*pColor);
	_Init(color, strCaption, pNotify, bAlphaEnable);
}
VOID KColorPickerDialog::Init( DWORD dwColor, LPCTSTR strCaption, CWnd* pNotify, BOOL bAlphaEnable )
{
    m_pColorValueBinded = NULL;
    m_pBindedColor = NULL;
    return _Init(dwColor, strCaption, pNotify, bAlphaEnable);
}

VOID KColorPickerDialog::_Init( DWORD dwColor, LPCTSTR strCaption, CWnd* pNotify, BOOL bAlphaEnable )
{
	TCHAR strWindowCaption[MAX_PATH] = _T("");
	_stprintf_s(strWindowCaption, _T("ColorPicker-%s"), strCaption);
	
	//m_nGridSize = 5;
	//m_bUseColorValue = FALSE;
	

	//s_nWidth2 = abs(m_rectColor2.right - m_rectColor2.left - 2);

	//m_bVernierSelect = FALSE;
	//m_bAlphaSelect = FALSE;
	//m_bVernierMoved = FALSE;
	//m_bAlphaMoved = FALSE;
	//m_bColor1Select = FALSE;

	m_dwWhichPartIsCursorIn = 0;

	m_bHSVModel = TRUE;

	m_enumLockChannel = ENUM_CHANNEL_H;


	/*RECT rectSel = { 340, 9, 410, 49};
	m_rectSelectColor = rectSel;*/

	/*RECT rectOrg = { rectSel.left, rectSel.bottom, rectSel.right, rectSel.bottom + 39};
	m_rectOriginColor = rectOrg;*/

	/*RECT rectSelAndOrg = { rectSel.left, rectSel.top, rectOrg.right, rectOrg.bottom };
	m_rectSelAndOrg = rectSelAndOrg;*/

	/*RECT rectClear0 = { s_rectColor2.left - s_nSpan - s_ptVernier.x - 1,
		max(s_rectColor2.top - 6, 0),
		s_rectColor2.left - s_nSpan + 1,
		s_rectColor2.bottom + 6};
	m_rectClearRgn[0] = rectClear0;*/

	/*RECT rectClear1 = { s_rectColor2.right + s_nSpan - 1,
		m_rectClearRgn[0].top,
		s_rectColor2.right + s_nSpan - 1 + s_ptVernier.x + 2,
		m_rectClearRgn[0].bottom };
	m_rectClearRgn[1] = rectClear1;*/

	/*m_byChannel[0] = static_cast<BYTE>(GETRED(m_dwColor));
	m_byChannel[1] = static_cast<BYTE>(GETGREEN(m_dwColor));
	m_byChannel[2] = static_cast<BYTE>(GETBLUE(m_dwColor));
	m_byChannel[3] = static_cast<BYTE>(GETALPHA(m_dwColor));*/

	

	//if ( m_bAlphaEnable )
	{
		//Alpha 滑动条
		/*RECT rectAlpha = {s_rectColor1.left, s_rectColor1.bottom + 2, s_rectColor1.right, rectAlpha.top + 17};
		m_rectAlpha = rectAlpha;*/
		/*if(!m_pAlphaBuffer)
			m_pAlphaBuffer = new DWORD[em_color_pallete_size * abs(m_rectAlpha.bottom - m_rectAlpha.top - 2)];*/
		/*RECT rectAlphaRgn = { rectAlpha.left, rectAlpha.top, rectAlpha.right, rectAlpha.bottom + s_ptVernier.y + s_nSpan + 1 };
		m_rectAlphaRgn = rectAlphaRgn;*/

		/*RECT rectClear2 = { max(m_rectAlpha.left - s_ptVernier.x/2 - 1, 0),
			m_rectAlpha.bottom + 1,
			m_rectAlpha.right + s_ptVernier.x /2 + 1,
			m_rectAlpha.bottom + s_ptVernier.y + s_nSpan + 1};
		m_rectClearRgn[2] = rectClear2;*/
	}

	

	if(! this->GetSafeHwnd())
	{
		BOOL bRet = Create(IDD);
		if(! bRet)
			return;
		//m_bCreated = TRUE;

		CRect rect;
		this->GetWindowRect(&rect);
		s_LastX = rect.left;
		s_LastY = rect.top;
	}

	this->SetWindowPos(NULL, s_LastX, s_LastY, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE);

	//注意SetColor要在窗口创建之后
	m_hNotifyHWND = (NULL == pNotify) ? NULL : pNotify->GetSafeHwnd();
	m_bAlphaEnable = bAlphaEnable;
	m_dwColorOld = ~dwColor;//这样子可以强行更新m_dwColor
	SetColor(dwColor, TRUE);
	m_dwColorOrg = dwColor;
	
	SetAlphaEnable(bAlphaEnable);

	static INT arrayRadioButtonIDs[] = {
		IDC_RADIO_H,                
		IDC_RADIO_S,                     
		IDC_RADIO_B,                     
		IDC_RADIO_RED,                   
		IDC_RADIO_GREEN,                 
		IDC_RADIO_BLUE,
	};

	ShowWindow(SW_SHOW);
	

	//保证它们是连续的

	for (size_t i = 0; i < _countof(arrayRadioButtonIDs) -1 ; ++i)
	{
		_ASSERTE(arrayRadioButtonIDs[i] + 1 == arrayRadioButtonIDs[i+1]);
	}

	{
		int nFirstID = arrayRadioButtonIDs[0];
		int nLastID = arrayRadioButtonIDs[_countof(arrayRadioButtonIDs)-1];
		if (0 == GetCheckedRadioButton(nFirstID, nLastID))
		{
			CButton* pFirstRadioButton = static_cast<CButton*>(GetDlgItem(nFirstID));
			if(NULL != pFirstRadioButton)
				pFirstRadioButton->SetCheck(BST_CHECKED);
		}
	}

	//看哪个选中，触发一下UpdateBitmap
	{
		int nFirstID = arrayRadioButtonIDs[0];
		int nLastID = arrayRadioButtonIDs[_countof(arrayRadioButtonIDs)-1];
		INT nID = GetCheckedRadioButton(nFirstID, nLastID);
		if (nID != 0)
		{
			CButton* pCheck = static_cast<CButton*>(this->GetDlgItem(nID));
			if (NULL != pCheck)
			{
				WPARAM wParam = MAKELONG(nID, BN_CLICKED);
				this->PostMessage(WM_COMMAND, wParam, (LPARAM)pCheck->GetSafeHwnd());
			}
		}
	}

	{
		m_SliderAlpha.EnableWindow(m_bAlphaEnable);
		m_SliderAlpha.SetRange(0, 255, TRUE);
		m_SliderAlpha.SetPos(GETALPHA(dwColor));
	}

	
	SetWindowText(strWindowCaption);
	UpdateData(FALSE);
	Invalidate();
	BringWindowToTop();
}


KColorPickerDialog::~KColorPickerDialog(void)
{
	//SAFE_DELETE_ARRAY(m_pColorBuffer1);
	//SAFE_DELETE_ARRAY(m_pColorBuffer2);
	//SAFE_DELETE_ARRAY(m_pAlphaBuffer);
	
	//SAFE_DELETE(m_pbmpColor2);
//	SAFE_DELETE(m_pbmpColor1);
	//SAFE_DELETE(m_pBmpAlpha);

}


/*
*@brief 现实左边的颜色选择框
*@param pDc     窗口的DC
*@param byRed   红色通道的值
*@param byGreen 绿色通道的值
*@param byBlue  蓝色通道的值
*@param rect    没什么用, 本来想作为重画的区域,不过没有做
*/
void KColorPickerDialog::DrawColor1(CDC* pDc,BYTE byRed, BYTE byGreen, BYTE byBlue, const RECT& rect)
{
	/*if( !m_pbmpColor1)
	{
		m_pbmpColor1 = new CBitmap;
		m_pbmpColor1->CreateCompatibleBitmap(pDc, em_color_pallete_size, em_color_pallete_size);
	}*/
	//{
	//	DWORD dwInputColor = (static_cast<DWORD>(byRed)<<16 | static_cast<DWORD>(byGreen)<<8 | static_cast<DWORD>(byBlue));
	//	DWORD dwBase = m_enumLockChannel & dwInputColor;
	//	DWORD dwStepY = (0x00000100 & ~m_enumLockChannel) ? 0x00000100 : 0x00000001;
	//	DWORD dwStepX = (0x00010000 & ~m_enumLockChannel) ? 0x00010000 : 0x00000001;
	//	if ( dwStepY == 0x100 && dwStepX == 0x1 )
	//	{
	//		DWORD dwTemp = dwStepY;
	//		dwStepY = dwStepX;
	//		dwStepX = dwTemp;
	//	}
	//	//if(bUpdateBmp)
	//	{
	//		DWORD colorBuffer[em_color_pallete_size * em_color_pallete_size];

	//		for ( DWORD y = 0; y < em_color_pallete_size; y++ )
	//		{
	//			for ( int x = em_color_pallete_size - 1; x >= 0; x-- )
	//			{
	//				colorBuffer[y * em_color_pallete_size + x] = dwBase;
	//				dwBase += dwStepX;
	//			}
	//			dwBase -= (dwStepX << 8 & 0x00ffffff);
	//			dwBase += dwStepY;
	//		}
	//		m_bmpLeft.SetBitmapBits(em_color_pallete_size << 10, colorBuffer);
	//	}
	//}
	CBrush Brush(/*m_pbmpColor1*/&m_bmpLeft);
	CBrush* pOldBrush = pDc->SelectObject(&Brush);
	pDc->SetBrushOrg(s_rectColor1.left + 1, s_rectColor1.top + 1);
	pDc->Rectangle(&s_rectColor1);

	POINT pt;
	DWORD dwMask = ~m_enumLockChannel & GetCurrentColor();
	pt.x = GETRED(~m_enumLockChannel) ? GETRED(dwMask) : GETGREEN(dwMask);
	pt.y = GETBLUE(~m_enumLockChannel) ? GETBLUE(GetCurrentColor()) : GETGREEN(dwMask);

	RECT rectClip;
	CPen Pen(PS_SOLID, 1, RGB((255 - pt.x), (255 - pt.y), (255 - GETBLUE(GetCurrentColor()))));
	CPen* pOldPen = pDc->SelectObject(&Pen);
	pDc->GetClipBox(&rectClip);
	CRgn Rgn;
	CRgn Rgn1;
	Rgn.CreateRectRgnIndirect(&s_rectColor1);
	pDc->SelectClipRgn(&Rgn);
	pDc->SelectObject(GetStockObject(HOLLOW_BRUSH));
	pDc->Ellipse( s_rectColor1.right - 1 - pt.x - 3,
		pt.y + s_rectColor1.top + 1 - 3,
		s_rectColor1.right - 1 - pt.x + 3,
		pt.y + s_rectColor1.top + 1 + 3);
	Rgn1.CreateRectRgnIndirect(&rectClip);
	pDc->SelectClipRgn(&Rgn1);
	pDc->SelectObject(pOldPen);
	pDc->SelectObject(pOldBrush);
}

/*
*@brief 显示左边的颜色条
*@param pDC    窗口的
*@param byRed   红色通道的值
*@param byGreen 绿色通道的值
*@param byBlue  蓝色通道的值
*@同时还会使用类成员m_enumLockChannel
*/
void KColorPickerDialog::DrawColor2(CDC* pDc, BYTE byRed, BYTE byGreen , BYTE byBlue)
{
	/*if( !m_pbmpColor2 )
	{
		m_pbmpColor2 = new CBitmap;
		m_pbmpColor2->CreateCompatibleBitmap(pDc, 1, em_color_pallete_size);
	}*/
	//{
	//	BYTE byRedMask   = static_cast<BYTE>((m_enumLockChannel & 0x00ff0000 & 0x00010000) >> 16);
	//	BYTE byGreenMask = static_cast<BYTE>((m_enumLockChannel & 0x0000ff00 & 0x00000100) >> 8);
	//	BYTE byBlueMask  = static_cast<BYTE>((m_enumLockChannel & 0x000000ff & 0x00000001));
	//	DWORD dwBase = ~m_enumLockChannel & GetCurrentColor();
	//	DWORD dwStep = static_cast<DWORD>(byRedMask  << 16 ) |
	//		static_cast<DWORD>(byGreenMask <<  8 ) |
	//		static_cast<DWORD>(byBlueMask) & 0x00ffffff;

	//	DWORD colorBuffer[em_color_pallete_size];
	//	//if(bUpdateBmp)
	//	{
	//		for( int i = em_color_pallete_size - 1; i >= 0; i-- )
	//		{
	//			colorBuffer[i] = dwBase;
	//			dwBase += dwStep;
	//		}
	//		m_bmpRight.SetBitmapBits(em_color_pallete_size * 4, colorBuffer);
	//	}
	//}
	CBrush Brush(&m_bmpLeft);
	CBrush* pOldBrush = pDc->SelectObject(&Brush);
	pDc->SetBrushOrg(s_rectColor2.left + 1, s_rectColor2.top + 1);
	pDc->Rectangle(&s_rectColor2);
	pDc->SelectObject(pOldBrush);
}

void KColorPickerDialog::DrawColor1HSV(CDC* pDc, float h, float s, float v)
{
	
	/*if( !m_pbmpColor1 )
	{
		m_pbmpColor1 = new CBitmap;
		m_pbmpColor1->CreateCompatibleBitmap(pDc, em_color_pallete_size, em_color_pallete_size);
	}*/
	/*if(bUpdateBmp)
	{
	float fStepX, fStepY;
	int i, j;
		DWORD colorBuffer[em_color_pallete_size * em_color_pallete_size];
		switch(m_enumLockChannel)
		{
		case ENUM_CHANNEL_H:
			{
				fStepX = 1.0f / 255.0f;
				fStepY = fStepX;
				for ( j = 0; j < em_color_pallete_size; j++ )
				{
					float v1 = j * fStepY;
					for ( i = 0; i < em_color_pallete_size; i++ )
					{
						float s1 =  i * fStepX;
						DWORD dwRGB = HSV2RGB(h, s1, v1) & 0x00ffffff;
						colorBuffer[(em_color_pallete_size - 1 - j) * em_color_pallete_size + i] = dwRGB;
					}
				}
			}
			break;
		case ENUM_CHANNEL_S:
			{
				fStepX = 360.0f / 255.0f;
				fStepY = 1.0f / 255.0f;
				for ( j = 0; j < em_color_pallete_size; j++ )
				{
					float v1 = j * fStepY;
					for ( i = 0; i < em_color_pallete_size; i++ )
					{
						float h1 =  i * fStepX;
						DWORD dwRGB = HSV2RGB(h1, s, v1);
						colorBuffer[(em_color_pallete_size - 1 - j) * em_color_pallete_size + i] = dwRGB;
					}
				}
			}
			break;
		case ENUM_CHANNEL_B:
			{
				fStepX = 360.0f / 255.0f;
				fStepY = 1.0f / 255.0f;
				for ( j = 0; j < em_color_pallete_size ; j++ )
				{
					float s1 = j * fStepY;
					for ( i = 0; i < em_color_pallete_size; i++ )
					{
						float h1 =  i * fStepX;
						DWORD dwRGB = HSV2RGB(h1, s1, v) & 0x00ffffff;
						colorBuffer[(em_color_pallete_size-1 - j) * em_color_pallete_size + i] = dwRGB;
					}
				}
			}
			break;
		}
		m_bmpLeft.SetBitmapBits(em_color_pallete_size << 10 , colorBuffer);
	}*/
	CBrush Brush(/*m_pbmpColor1*/&m_bmpLeft);
	CBrush* pOldBrush = pDc->SelectObject(&Brush);
	pDc->SetBrushOrg(s_rectColor1.left + 1, s_rectColor1.top + 1);
	pDc->Rectangle(&s_rectColor1);

	POINT pt = { 0, 0 };
	switch(m_enumLockChannel)
	{
	case ENUM_CHANNEL_B:
		{
			//pt.x = static_cast<int>((1.0f - m_HSV[0] / 360.0f) * 255.0f);
			//pt.y = static_cast<int>((1.0f - m_HSV[1])  * 255.0f);
			pt.x = static_cast<int>(m_HSV[0] * 255);
			pt.y = static_cast<int>(m_HSV[1] * 255);
		}
		break;
	case ENUM_CHANNEL_H:
		{
			pt.x = static_cast<int>((1.0f - m_HSV[1])  * 255.0f);
			pt.y = static_cast<int>((1.0f - m_HSV[2])  * 255.0f);
		}
		break;
	case ENUM_CHANNEL_S:
		{
			pt.x = static_cast<int>((1.0f - m_HSV[0] / 360.0f) * 255.0f);
			pt.y = static_cast<int>((1.0f - m_HSV[2]) * 255.0f);
		}
		break;
	}


	RECT rectClip;
	CPen Pen(PS_SOLID, 1, RGB((255 - pt.x), (255 - pt.y), (255 - GETBLUE(GetCurrentColor()))));
	CPen* pOldPen = pDc->SelectObject(&Pen);
	pDc->GetClipBox(&rectClip);
	CRgn Rgn;
	CRgn Rgn1;
	Rgn.CreateRectRgnIndirect(&s_rectColor1);
	pDc->SelectClipRgn(&Rgn);
	pDc->SelectObject(GetStockObject(HOLLOW_BRUSH));
	pDc->Ellipse( s_rectColor1.right - 1 - pt.x - 3,
		pt.y + s_rectColor1.top + 1 - 3,
		s_rectColor1.right - 1 - pt.x + 3,
		pt.y + s_rectColor1.top + 1 + 3);
	Rgn1.CreateRectRgnIndirect(&rectClip);
	pDc->SelectClipRgn(&Rgn1);
	pDc->SelectObject(pOldPen);
	pDc->SelectObject(pOldBrush);
}

void KColorPickerDialog::DrawColor2HSV(CDC* pDc, float h, float s, float v)
{
	
	/*if( !m_pbmpColor2 )
	{
		m_pbmpColor2 = new CBitmap;
		m_pbmpColor2->CreateCompatibleBitmap(pDc, 1, em_color_pallete_size);
	}*/
	//if(bUpdateBmp)
	/*{
		float fStep;
		int i;
		DWORD colorBuffer[em_color_pallete_size];
		switch(m_enumLockChannel)
		{
		case ENUM_CHANNEL_H:
			{
				fStep = 360.0f / 255.0f;
				for ( i = 0; i < em_color_pallete_size; i++ )
				{
					colorBuffer[em_color_pallete_size - 1 - i] = HSV2RGB(fStep * i, 1.0f, 1.0f);

				}
			}
			break;
		case ENUM_CHANNEL_S:
			{
				fStep = 1.0f / 255.0f;
				for ( i = 0; i < em_color_pallete_size; i++ )
				{
					colorBuffer[em_color_pallete_size - 1 - i] = HSV2RGB(h, i * fStep, v);
				}
			}
			break;
		case ENUM_CHANNEL_B:
			{
				fStep = 1.0f / 255.0f;
				for ( i = 0; i < em_color_pallete_size; i++ )
				{
					colorBuffer[em_color_pallete_size - 1 - i] = HSV2RGB(h, s, i * fStep);
				}
			}
			break;
		}
		m_bmpRight.SetBitmapBits(em_color_pallete_size << 2, colorBuffer);
	}*/
	CBrush Brush(/*m_pbmpColor2*/&m_bmpRight);
	CBrush* pOldBrush = pDc->SelectObject(&Brush);
	pDc->SetBrushOrg(s_rectColor2.left + 1, s_rectColor2.top + 1);
	pDc->Rectangle(&s_rectColor2);
	pDc->SelectObject(pOldBrush);

}





void KColorPickerDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	DrawAll(&dc);
}

/*
*@brief 显示下方的Alpha条
*@param pDC    窗口的DC
*/
//void KColorPickerDialog::DrawAlpha(CDC* pDc)
//{
//	/*if ( !m_pBmpAlpha )
//	{
//		m_pBmpAlpha = new CBitmap;
//		m_pBmpAlpha->CreateCompatibleBitmap(pDc, em_color_pallete_size, 10);
//	}*/
//	DWORD dwColor = m_dwColor & 0x00ffffff;
//	DWORD dwAlpha = m_dwColor & 0xff000000;
//	int nFlag = 0;
//	DWORD alphaBuffer[em_color_pallete_size * em_color_pallete_size];
//	for ( int i = 0; i < em_color_pallete_size; i++ )
//	{
//		if (( i / m_nGridSize ) % 2 )
//			nFlag = 1;
//		else
//			nFlag = 0;
//		float fAlpha = i / 255.0f;
//		BYTE byGrey = static_cast<BYTE>(192 * (1.0f - fAlpha));
//		BYTE byWhite = static_cast<BYTE>(255 * (1.0f - fAlpha));
//		DWORD dwColorBKGrey = (static_cast<DWORD>(GETRED(dwColor)   * fAlpha + byGrey) << 16) |
//			(static_cast<DWORD>(GETGREEN(dwColor) * fAlpha + byGrey )<< 8 ) |
//			(static_cast<DWORD>(GETBLUE(dwColor)  * fAlpha + byGrey)      ) & 0x00ffffff;
//		DWORD dwColorBKWhite = (static_cast<DWORD>(GETRED(dwColor)   * fAlpha + byWhite )<< 16) |
//			(static_cast<DWORD>(GETGREEN(dwColor) * fAlpha + byWhite )<< 8 ) |
//			(static_cast<DWORD>(GETBLUE(dwColor)  * fAlpha + byWhite)      ) & 0x00ffffff;
//
//		
//
//		for( int j = 0; j < 10; j++ )
//		{
//			if (( nFlag + j / m_nGridSize ) % 2)
//				*(alphaBuffer + i + j * em_color_pallete_size) = dwColorBKGrey;
//			else
//				*(alphaBuffer + i + j * em_color_pallete_size) = dwColorBKWhite;
//		}
//	}
//	m_bmpAlpha.SetBitmapBits(em_color_pallete_size * (m_rectAlpha.bottom - m_rectAlpha.top - 2) * sizeof(DWORD), alphaBuffer);
//	CBrush Brush(/*m_pBmpAlpha*/&m_bmpAlpha);
//	CBrush* pOldBrush = pDc->SelectObject(&Brush);
//	pDc->Rectangle(&m_rectAlpha);
//	pDc->SelectObject(pOldBrush);
//}

/*
*@brief 清除游标区域
*@param pDC    窗口的DC
*/
void KColorPickerDialog::ClearVernier(CDC* pDc)
{
	COLORREF crBK = ::GetSysColor(COLOR_BTNFACE);
	CBrush Brush(crBK);
	CPen Pen(PS_SOLID, 1, crBK);
	CBrush* pOldBrush = pDc->SelectObject(&Brush);
	CPen* pOldPen = pDc->SelectObject(&Pen);
	//if( //m_bVernierMoved )
	{
		pDc->Rectangle(&m_rectClearRgn[0]);
		pDc->Rectangle(&m_rectClearRgn[1]);
	}
	/*if ( m_bAlphaEnable && m_bAlphaMoved)
	{
		pDc->Rectangle(&m_rectClearRgn[2]);
		m_bAlphaMoved = FALSE;
	}*/
	//m_bVernierMoved = FALSE;
	pDc->SelectObject(pOldPen);
	pDc->SelectObject(pOldBrush);
}

/*
*@brief 显示游标
*@param pDC    窗口的DC
*/
void KColorPickerDialog::DrawVernier(CDC* pDc)
{
	DWORD dwTest = static_cast<DWORD>(m_enumLockChannel);
	int nOffset = 0;
	while(dwTest>>nOffset != 0xff)
	{
		nOffset += 8;
	}

	POINT ptPos = { s_rectVernierRgn.left, s_rectColor2.bottom - 1 - ((GetCurrentColor()&m_enumLockChannel)>>nOffset) - s_ptVernier.y / 2};
	if ( m_bHSVModel )
	{
		switch(m_enumLockChannel)
		{
		case ENUM_CHANNEL_B:
			{
				ptPos.y = s_rectColor2.bottom - 1 - static_cast<int>(m_HSV[2] * 255.0f) - s_ptVernier.y / 2;
			}
			break;
		case ENUM_CHANNEL_H:
			{
				ptPos.y = s_rectColor2.bottom - 1 - static_cast<int>(m_HSV[0] / 360.0f * 255.0f) - s_ptVernier.y / 2;
			}
			break;
		case ENUM_CHANNEL_S:
			{
				ptPos.y = s_rectColor2.bottom - 1 - static_cast<int>(m_HSV[1] * 255.0f) - s_ptVernier.y / 2;
			}
			break;
		}
	}
	CPen Pen(PS_SOLID, 1, RGB(0, 0, 0));
	CBrush Brush(RGB(255, 255, 255));
	CBrush* pOldBrush = pDc->SelectObject(&Brush);
	CPen* pOldPen = pDc->SelectObject(&Pen);

	//Left vernier
	pDc->MoveTo(ptPos);
	if ( m_bHSVModel )
	{
		switch(m_enumLockChannel)
		{
		case ENUM_CHANNEL_B:
			{
				pDc->LineTo(s_rectVernierRgn.left + s_ptVernier.x, s_rectColor2.bottom - 1 - static_cast<int>(m_HSV[2] * 255.0f));
			}
			break;
		case ENUM_CHANNEL_H:
			{
				pDc->LineTo(s_rectVernierRgn.left + s_ptVernier.x, s_rectColor2.bottom - 1 - static_cast<int>(m_HSV[0] / 360.0f  * 255.0f));
			}
			break;
		case ENUM_CHANNEL_S:
			{
				pDc->LineTo(s_rectVernierRgn.left + s_ptVernier.x, s_rectColor2.bottom - 1 - static_cast<int>(m_HSV[1] * 255.0f));
			}
			break;
		}
	}
	else
	{
		pDc->LineTo(s_rectVernierRgn.left + s_ptVernier.x, s_rectColor2.bottom - 1 - ((GetCurrentColor()&m_enumLockChannel)>>nOffset));
	}
	pDc->LineTo(ptPos.x, ptPos.y + s_ptVernier.y);
	pDc->LineTo(ptPos);
	pDc->FloodFill(ptPos.x + 1 , ptPos.y + s_ptVernier.y / 2, RGB(0, 0, 0));

	//Right vernier
	ptPos.x = s_rectVernierRgn.right;
	pDc->MoveTo(ptPos);
	if ( m_bHSVModel )
	{
		switch(m_enumLockChannel)
		{
		case ENUM_CHANNEL_B:
			{
				pDc->LineTo(s_rectVernierRgn.right - s_ptVernier.x, s_rectColor2.bottom - 1 - static_cast<int>(m_HSV[2] * 255.0f));
			}
			break;
		case ENUM_CHANNEL_H:
			{
				pDc->LineTo(s_rectVernierRgn.right - s_ptVernier.x, s_rectColor2.bottom - 1 - static_cast<int>(m_HSV[0] / 360.0f * 255.0f));
			}
			break;
		case ENUM_CHANNEL_S:
			{
				pDc->LineTo(s_rectVernierRgn.right - s_ptVernier.x, s_rectColor2.bottom - 1 - static_cast<int>(m_HSV[1] * 255.0f));
			}
			break;
		}
	}
	else
	{
		pDc->LineTo(s_rectVernierRgn.right - s_ptVernier.x, s_rectColor2.bottom - 1 - ((GetCurrentColor()&m_enumLockChannel)>>nOffset));
	}
	pDc->LineTo(ptPos.x, ptPos.y + s_ptVernier.y);
	pDc->LineTo(ptPos);
	pDc->FloodFill(ptPos.x - 1, ptPos.y + s_ptVernier.y / 2, RGB(0, 0, 0));

	//if ( m_bAlphaEnable)//Draw alpha vernier
	//{
	//	ptPos.x = GETALPHA(m_dwColor) + m_rectAlpha.left - 1;
	//	ptPos.y = m_rectAlpha.bottom + s_nSpan;
	//	pDc->MoveTo(ptPos.x, ptPos.y);
	//	pDc->LineTo(ptPos.x + s_ptVernier.x/2, ptPos.y + s_ptVernier.y);
	//	pDc->LineTo(ptPos.x - s_ptVernier.x/2, ptPos.y + s_ptVernier.y);
	//	pDc->LineTo(ptPos.x, ptPos.y);
	//	pDc->FloodFill(ptPos.x, ptPos.y + 1, RGB(0, 0, 0));
	//}
	pDc->SelectObject(pOldPen);
	pDc->SelectObject(pOldBrush);
}

void KColorPickerDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
    //IEKG3DEngineManager *pManager = g_GetEngineIEInterface();

	CClientDC dc(this);
	SetCapture();
	
	if ( ::PtInRect(&s_rectVernierRgn, point) )//If in color selection box2(right)
	{
		//m_bVernierSelect = TRUE;
		m_dwWhichPartIsCursorIn = em_part_right_vernier;

		//m_bVernierMoved = TRUE;
		//m_bUpdateBmp1 = TRUE;
		//m_bUpdateBmp2 = FALSE;
		DWORD dwPos = max(min(s_rectColor2.bottom - 1 - point.y, 255), 0);
		if ( m_bHSVModel )
		{
			switch(m_enumLockChannel)
			{
			case ENUM_CHANNEL_H:
				{
					m_HSV[0] = dwPos / 255.0f * 360.0f;
				}
				break;
			case ENUM_CHANNEL_S:
				{
					m_HSV[1] = dwPos / 255.0f;
				}
				break;
			case ENUM_CHANNEL_B:
				{
					m_HSV[2] = dwPos / 255.0f;
				}
				break;
			}
			SetColor(m_HSV[0], m_HSV[1], m_HSV[2]);
		}
		else
		{
			dwPos = dwPos<<16 | dwPos<<8 | dwPos;
			D3DCOLOR colorNew = GetCurrentColor() &(~m_enumLockChannel) | (dwPos & m_enumLockChannel);		
			SetColor(colorNew, FALSE);
		}
		UpdateBitmapLeft();
	}
	else if ( ::PtInRect(&s_rectColor1, point) )//If in color selection box1(left)
	{
		m_dwWhichPartIsCursorIn = em_part_center_color_pallete;

		//m_bColor1Select = TRUE;
		//m_bUpdateBmp1 = FALSE;
		//m_bUpdateBmp2 = TRUE;
		if ( m_bHSVModel )//HSV mode
		{
			int i1 = max(min(s_rectColor1.right - point.x, 0xff), 0);
			int i2 = max(min(point.y - s_rectColor1.top + 1, 0xff), 0);
			switch(m_enumLockChannel)
			{
			case ENUM_CHANNEL_H:
				{
					m_HSV[1] = 1.0f - i1 / 255.0f;
					m_HSV[2] = 1.0f - i2 / 255.0f;
				}
				break;
			case ENUM_CHANNEL_S:
				{
					m_HSV[0] = 360.0f - i1 / 255.0f * 360.0f;
					m_HSV[2] = 1.0f - i2 / 255.0f;
				}
				break;
			case ENUM_CHANNEL_B:
				{
					m_HSV[0] = 360.0f - i1 / 255.0f * 360.0f;
					m_HSV[1] = 1.0f - i2 / 255.0f;
				}
				break;
			}
			SetColor(m_HSV[0], m_HSV[1], m_HSV[2]);
		}
		else
		{
			DWORD dwChannel1 = DWORD(max(min(s_rectColor1.right - point.x, 0xff), 0));
			DWORD dwChannel2 = DWORD(max(min(point.y - s_rectColor1.top + 1, 0xff), 0));
			D3DCOLOR colorNew = GetCurrentColor();
			colorNew &= m_enumLockChannel;
			colorNew |= (~m_enumLockChannel & (dwChannel1<<16)) ? (dwChannel1<<16) : (dwChannel1<<8);
			colorNew |= (~m_enumLockChannel & dwChannel2) ? dwChannel2 : (dwChannel2<<8);
			SetColor(colorNew, FALSE);
		}
		UpdateBitmapRight();
	}
	else if( ::PtInRect(&m_rectOriginColor, point) )//Select original color
	{
		//m_bVernierMoved = TRUE;
		//m_bAlphaMoved = TRUE;
		//m_bUpdateBmp1 = TRUE;
		//m_bUpdateBmp2 = TRUE;
		SetColor(m_dwColorOrg, TRUE);
		UpdateBitmaps();
	}
	//else if ( ::PtInRect(&m_rectAlphaRgn, point) )//If in alpha selection region
	//{
	//	//m_bAlphaSelect = TRUE;
	//	m_dwWhichPartIsCursorIn = em_part_alpha_vernier;

	//	m_bAlphaMoved = TRUE;
	//	m_bUpdateBmp1 = FALSE;
	//	m_bUpdateBmp2 = FALSE;
	//	//m_byChannel[3] = static_cast<BYTE>(max(min(point.x - m_rectAlpha.left + 1, 0xff), 0));
	//	//DWORD dwAlpha = static_cast<DWORD>(m_byChannel[3])<<24 & 0xff000000;

	//	D3DCOLOR colorNew = m_dwColor;
	//	colorNew &= 0x00ffffff;
	//	//colorNew |= dwAlpha;
	//	colorNew |= (GETALPHA(m_dwColor) << 24);
	//	SetColor(colorNew);
	//}
	else
	{
		m_dwWhichPartIsCursorIn = 0;
	}

	if (m_dwWhichPartIsCursorIn != 0)
	{
		UpdateData(FALSE);
		CClientDC dc(this);
		this->DrawAll(&dc);

		if (m_hNotifyHWND)
		{
			DWORD dwColor = GetCurrentColor();
			::SendMessage(m_hNotifyHWND, WM_COLORPOINTER_CHANGE, dwColor, reinterpret_cast<LPARAM>(this->GetSafeHwnd()));
		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}

/*
*@brief 显示当前选择的颜色
*@param pDC    窗口的DC
*位置在类成员m_rectSelectColor中指定
*/
void KColorPickerDialog::DrawSelectColor(CDC* pDc)
{
	CBrush Brush(RGB(GETRED(GetCurrentColor()), GETGREEN(GetCurrentColor()), GETBLUE(GetCurrentColor())));
	CPen Pen(PS_NULL, 1, RGB(0, 0, 0));
	CPen Pen1(PS_SOLID, 1, RGB(0, 0, 0));
	CBrush* pOldBrush = pDc->SelectObject(&Brush);
	CPen* pOldPen = pDc->SelectObject(&Pen);
	pDc->Rectangle(&m_rectSelectColor);
	pDc->SelectObject(GetStockObject(HOLLOW_BRUSH));
	pDc->SelectObject(&Pen1);
	pDc->Rectangle(&m_rectSelAndOrg);
	pDc->SelectObject(pOldPen);
	pDc->SelectObject(pOldBrush);
}

/*
*@brief 显示初始的颜色(打开颜色框是传入的颜色)
*@param pDC    窗口的DC
*位置在类成员m_rectOrginColor中指定
*/
void KColorPickerDialog::DrawOriginColor(CDC* pDc)
{
	COLORREF crColor = RGB(GETRED(m_dwColorOrg), GETGREEN(m_dwColorOrg), GETBLUE(m_dwColorOrg));
	CBrush Brush(crColor);
	CPen Pen(PS_NULL, 1, RGB(0, 0, 0));
	CPen Pen1(PS_SOLID, 1, RGB(0, 0, 0));
	CBrush* pOldBrush = pDc->SelectObject(&Brush);
	CPen* pOldPen = pDc->SelectObject(&Pen);
	pDc->Rectangle(&m_rectOriginColor);

	pDc->SelectObject(GetStockObject(HOLLOW_BRUSH));
	pDc->SelectObject(&Pen1);
	pDc->Rectangle(&m_rectSelAndOrg);
	pDc->SelectObject(pOldPen);
	pDc->SelectObject(pOldBrush);

}

void KColorPickerDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
	//m_bVernierSelect = FALSE;
	//m_bColor1Select = FALSE;

	m_dwWhichPartIsCursorIn = 0;

	//m_bAlphaSelect = FALSE;
	//m_bUpdateBmp1 = FALSE;
	//m_bUpdateBmp2 = FALSE;
	/*D3DCOLOR colorNew = m_dwColor;
	colorNew |= static_cast<DWORD>(m_byChannel[3])<<24;
	SetColor(colorNew);*/
	ReleaseCapture();
	CDialog::OnLButtonUp(nFlags, point);
}

//BOOL KColorPickerDialog::OnInit()
//{
//	//Called in Create, after CWnd::Create() is called
//	//Create controls in the dialog
//	BOOL bReturn = TRUE;
//	RECT rectRadio = { 340, 140, 380, 160 };
//	RECT rectEdit = { 380, 140, 410, 160};
//
//	//bReturn &= m_ctlChannel[0].Create(_T("R:"), BS_AUTORADIOBUTTON|WS_GROUP|WS_VISIBLE|WS_CHILD, rectRadio, this, IDC_CHANNEL_RED);
//	//bReturn &= m_ctlEditChannel[0].Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_RIGHT|WS_CHILD|WS_DISABLED, rectEdit, this, IDC_CHANNEL_RED + 1);
//
//	rectRadio.top += 22;
//	rectRadio.bottom += 22;
//	rectEdit.top += 22;
//	rectEdit.bottom += 22;
//	//bReturn &= m_ctlChannel[1].Create(_T("G:"), BS_AUTORADIOBUTTON|WS_VISIBLE|WS_CHILD, rectRadio, this, IDC_CHANNEL_GREEN);
//	//bReturn &= m_ctlEditChannel[1].Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_RIGHT|WS_CHILD|WS_DISABLED, rectEdit, this, IDC_CHANNEL_GREEN + 1);
//
//	rectRadio.top += 22;
//	rectRadio.bottom += 22;
//	rectEdit.top += 22;
//	rectEdit.bottom += 22;
//
//	//bReturn &= m_ctlChannel[2].Create(_T("B:"), BS_AUTORADIOBUTTON|WS_VISIBLE|WS_CHILD, rectRadio, this, IDC_CHANNEL_BLUE);
//	//bReturn &= m_ctlEditChannel[2].Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_RIGHT|WS_CHILD|WS_DISABLED, rectEdit, this, IDC_CHANNEL_BLUE + 1);
//
//	/*int nChannel = 3;
//	if ( m_bAlphaEnable )
//	{
//		nChannel = 4;
//	}*/
//
//	//TCHAR strValue[4];
//	//for(int i = 0; i < nChannel; i++ )
//	//{
//	//	sprintf(strValue, "%d", m_byChannel[i]);
//	//	//m_ctlEditChannel[i].SetWindowText(strValue);
//	//	//m_ctlEditChannel[i].RedrawWindow();
//	//}
//
//	//m_ctlChannel[2].SetCheck(BST_CHECKED);
//
//	return bReturn;
//}

void KColorPickerDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	BOOL bLeftKeyDown = nFlags   &   MK_LBUTTON;
	if(! bLeftKeyDown)
		return;

	if ( ::PtInRect(&s_rectColor1, point) )
	{
		if ( m_bHSVModel )
		{
			int i1 = s_rectColor1.right - point.x - 1;
			int i2 = point.y - s_rectColor1.top - 1;
			i1 = max(min(i1, 0xff), 0);
			i2 = max(min(i2, 0xff), 0);
			switch(m_enumLockChannel)
			{
			case ENUM_CHANNEL_H:
				{
					m_HSV[1] = 1.0f - i1 / 255.0f;
					m_HSV[2] = 1.0f - i2 / 255.0f;

				}
				break;
			case ENUM_CHANNEL_S:
				{
					m_HSV[0] = (1.0f - i1 / 255.0f) * 360.0f;
					m_HSV[2] = 1.0f - i2 / 255.0f;
				}
				break;
			case ENUM_CHANNEL_B:
				{
					m_HSV[0] = (1.0f - i1 / 255.0f) * 360.0f;
					m_HSV[1] = 1.0f - i2 / 255.0f;
				}
				break;
			}
			SetColor(m_HSV[0], m_HSV[1], m_HSV[2]);
		}
		else
		{
			//If left button down and point is in the color1 rectangle, change color as well
			int i1 = s_rectColor1.right - point.x - 1;
			int i2 = point.y - s_rectColor1.top - 1;

			//Make sure the value is in the range of ( 0 - 255 )
			DWORD dwChannel1 = DWORD(max(min(i1, 0xff), 0));
			DWORD dwChannel2 = DWORD(max(min(i2, 0xff), 0));

			D3DCOLOR colorNew = GetCurrentColor();
			colorNew &= m_enumLockChannel;
			colorNew |= (~m_enumLockChannel & (dwChannel1<<16)) ? (dwChannel1<<16) : (dwChannel1<<8);
			colorNew |= (~m_enumLockChannel & dwChannel2) ? dwChannel2 : (dwChannel2<<8);
			SetColor(colorNew, FALSE);
			/*m_byChannel[0] = static_cast<BYTE>(GETRED(m_dwColor));
			m_byChannel[1] = static_cast<BYTE>(GETGREEN(m_dwColor));
			m_byChannel[2] = static_cast<BYTE>(GETBLUE(m_dwColor));*/

			//for(int i = 0; i < 3; i++ )
			//{
			//	sprintf(strValue, "%d", m_byChannel[i]);
			//	//m_ctlEditChannel[i].SetWindowText(strValue);
			//	//m_ctlEditChannel[i].RedrawWindow();
			//}

		}
		UpdateBitmapRight();
	}
	else if ( ::PtInRect(&s_rectVernierRgn, point) )
	{
		//If the vernier is selected, move the vernier
		//m_bVernierMoved = TRUE;
		//m_bAlphaMoved = TRUE;
		DWORD dwPos = max(min(s_rectColor2.bottom - 1 - point.y, 255), 0);
		if ( m_bHSVModel )
		{
			switch(m_enumLockChannel)
			{
			case ENUM_CHANNEL_H:
				{
					m_HSV[0] = dwPos / 255.0f * 360.0f;
				}
				break;
			case ENUM_CHANNEL_S:
				{
					m_HSV[1] = dwPos / 255.0f;
				}
				break;
			case ENUM_CHANNEL_B:
				{
					m_HSV[2] = dwPos / 255.0f;
				}
				break;
			}
			SetColor(m_HSV[0], m_HSV[1], m_HSV[2]);
		}
		else
		{
			dwPos = dwPos<<16 | dwPos<<8 | dwPos;
			D3DCOLOR colorNew = GetCurrentColor();
			colorNew = colorNew &(~m_enumLockChannel) | (dwPos & m_enumLockChannel); 
			SetColor(colorNew, FALSE);
		}
		UpdateBitmapLeft();
	}
	//else if (em_part_alpha_vernier == m_dwWhichPartIsCursorIn)//Alpha vernier is selected
	//{
	//	m_bAlphaMoved = TRUE;
	//	//m_byChannel[3] = static_cast<BYTE>(max(min(point.x - m_rectAlpha.left + 1, 0xff), 0));
	//	BYTE alphaTemp = static_cast<BYTE>(max(min(point.x - m_rectAlpha.left + 1, 0xff), 0));
	//	D3DCOLOR colorNew = m_dwColor;
	//	colorNew &= 0x00ffffff;
	//	colorNew |= (static_cast<DWORD>(/*m_byChannel[3]*/alphaTemp) << 24);
	//	SetColor(colorNew);
	//}

	CDialog::OnMouseMove(nFlags, point);
}



void KColorPickerDialog::PostNcDestroy()
{
//	delete this;
	m_pBindedColor = NULL;
	m_pColorValueBinded = NULL;
}

void KColorPickerDialog::OnOK()
{
	CRect rect;
	this->GetWindowRect(&rect);
	s_LastX = rect.left;
	s_LastY = rect.top;
	UpdateData();
    WPARAM pWnd = reinterpret_cast<WPARAM>(this);
    ::SendMessage(m_hNotifyHWND, WM_COLORDIALOG_CLOSE, pWnd, NULL);
	CDialog::OnOK();
}

void KColorPickerDialog::RGB2HSV(DWORD rgb, float& hRet, float& sRet, float& vRet)
{
	float ColorMin;
	float ColorMax;
	float r = GETRED(rgb) / 255.0f;
	float g = GETGREEN(rgb)/ 255.0f;
	float b = GETBLUE(rgb) / 255.0f;
	FLOAT h,s,v;
	h = s = v = 0.0f;
	float Delta;

	ColorMax = max(r, g);
	ColorMax = max(ColorMax, b);
	ColorMin = min(r, g);
	ColorMin = min(ColorMin, b);
	v = ColorMax;
	if ( ColorMax != 0 )
	{
		s = (ColorMax - ColorMin) / ColorMax;
	}

	if ( s != 0 )
	{
		Delta = ColorMax - ColorMin;
		if ( r == ColorMax )
		{
			h = (g - b) / Delta;
		}
		else if ( g == ColorMax )
		{
			h = 2.0f + ( b - r ) / Delta;
		}
		else //B is max
		{
			h = 4.0f + ( r - g ) / Delta;
		}

		h *= 60.0f;

		if ( h < 0.0f )
		{
			h += 360;
		}
	}
	hRet = static_cast<FLOAT>(static_cast<INT>(h + 0.5f));
	sRet = static_cast<FLOAT>((static_cast<INT>(s * 100.f + 0.5f)))/100.0f;
	vRet = static_cast<FLOAT>((static_cast<INT>(v * 100.f + 0.5f)))/100.0f;
}

DWORD KColorPickerDialog::HSV2RGB(float h, float s, float v)//输出的Alpha位一定是0
{
	DWORD dwRGB = 0x00000000;
	float r , g , b;
	float i, f, p, q, t;
	r = g = b = 0.0f;

	if ( s == 0.0f )
	{
		if ( v != 0.0f )
		{
			DWORD channel = static_cast<DWORD>(v * 255);
			dwRGB = channel << 16 | channel << 8 | channel;
		}
	}
	else
	{
		if ( h == 360.0f )
		{
			h = 0.0f;
		}

		h /= 60.0f;
		i = ::floor(h);
		f = h - i;
		p = v * ( 1.0f - s );
		q = v * ( 1.0f - ( s * f ) );
		t = v * ( 1.0f - ( s * ( 1.0f - f) ) );

		if ( i == 0.0f )
		{
			r = v;
			g = t;
			b = p;
		}
		else if ( i == 1.0f )
		{
			r = q;
			g = v;
			b = p;
		}
		else if ( i == 2.0f )
		{
			r = p;
			g = v;
			b = t;
		}
		else if ( i == 3.0f )
		{
			r = p;
			g = q;
			b = v;
		}
		else if ( i == 4.0f )
		{
			r = t;
			g = p;
			b = v;
		}
		else if ( i == 5.0f )
		{
			r = v;
			g = p;
			b = q;
		}
		dwRGB = ((static_cast<DWORD>(r * 255) << 16) & 0x00ff0000) |
			((static_cast<DWORD>(g * 255) << 8 ) & 0x0000ff00) |
			(static_cast<DWORD>(b * 255) ) & 0x000000ff;
	}
	return dwRGB & 0x00ffffff;
}


void KColorPickerDialog::OnBnClickedRadioH()
{
	m_bHSVModel = TRUE;
	m_enumLockChannel = ENUM_CHANNEL_H;
	//m_bVernierMoved = TRUE;
	UpdateBitmaps();
}

void KColorPickerDialog::OnBnClickedRadioS()
{
	m_bHSVModel = TRUE;
	m_enumLockChannel = ENUM_CHANNEL_S;
	//m_bVernierMoved = TRUE;
	UpdateBitmaps();
}

void KColorPickerDialog::OnBnClickedRadioB()
{
	m_bHSVModel = TRUE;
	m_enumLockChannel = ENUM_CHANNEL_B;
	//m_bVernierMoved = TRUE;
	UpdateBitmaps();
}

void KColorPickerDialog::OnBnClickedRadioRed()
{
	m_bHSVModel = FALSE;
	m_enumLockChannel = ENUM_CHANNEL_RED;
	//m_bVernierMoved = TRUE;
	UpdateBitmaps();
}

void KColorPickerDialog::OnBnClickedRadioGreen()
{
	m_bHSVModel = FALSE;
	m_enumLockChannel = ENUM_CHANNEL_GREEN;
	//m_bVernierMoved = TRUE;
	UpdateBitmaps();
}

void KColorPickerDialog::OnBnClickedRadioBlue()
{
	m_bHSVModel = FALSE;
	m_enumLockChannel = ENUM_CHANNEL_BLUE;
	//m_bVernierMoved = TRUE;
	UpdateBitmaps();
}


LRESULT KColorPickerDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{
	//case WM_COLORPOINTER_CHANGE:
	//	{
	//		//通知颜色的指针发生了变化
	//		SetColor(*reinterpret_cast<DWORD*>(wParam));
	//		return 0;
	//	}
	//	break;
	case WM_SETFOCUS:
		{
			Invalidate();
		}
		break;
	case WM_KEYDOWN:
		{
			int nDebug = 0;
			break;
		}
	case WM_HSCROLL:
		{
			if (lParam == reinterpret_cast<LPARAM>(m_SliderAlpha.GetSafeHwnd()))
			{
				INT nPos = m_SliderAlpha.GetPos();
				if(nPos >= 0 && nPos <= 255)
				{
					DWORD dwCurColor = GetCurrentColor();
					this->SetColor(D3DCOLOR_SETALPHA(dwCurColor, nPos), TRUE);
				}
			}
		}
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void KColorPickerDialog::OnCancel()
{
	if(this->GetSafeHwnd())
	{
		CRect rect;
		this->GetWindowRect(&rect);
		s_LastX = rect.left;
		s_LastY = rect.top;

		SetColor(m_dwColorOrg, TRUE);//还原颜色	
	}

	UnInit();
	//DestroyWindow();

	CDialog::OnCancel();
}
VOID KColorPickerDialog::UnInit(void)
{
	if ( m_hNotifyHWND )
	{
		WPARAM pWnd = reinterpret_cast<WPARAM>(this);
		::SendMessage(m_hNotifyHWND, WM_COLORDIALOG_CLOSE, pWnd, NULL);
		m_hNotifyHWND = NULL;
	}
}

BOOL KColorPickerDialog::PreTranslateMessage(MSG* pMsg)
{
	/*int i = 0;
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		UpdateData(TRUE);

        DWORD dwColor = 0xffffffff;

        if (
            pMsg->hwnd == GetDlgItem(IDC_EDIT_H)->m_hWnd ||
            pMsg->hwnd == GetDlgItem(IDC_EDIT_S)->m_hWnd ||
            pMsg->hwnd == GetDlgItem(IDC_EDIT_B)->m_hWnd
        )
        {
		    dwColor = HSV2RGB(m_HSV[0], m_HSV[1], m_HSV[2]);
			dwColor |= (GETALPHA(m_dwColor) << 24);
        }

        SetColor(dwColor);		
        
        Invalidate();
		if (m_hNotifyHWND)
		{
			::SendMessage(m_hNotifyHWND, WM_COLORPOINTER_CHANGE, dwColor, reinterpret_cast<LPARAM>(this->GetSafeHwnd()));
		}
		return TRUE;
	}
	else*/ return CDialog::PreTranslateMessage(pMsg);
}

//VOID KColorPickerDialog::UpdateBindedColor()
//{
//	if (m_bUseColorValue)
//	{
//		*m_pColorValue = ::Color2Value(m_dwColor);//pManager->Color2Value(m_dwColor);
//	}
//}

void KColorPickerDialog::SetColorCore( D3DCOLOR color )
{
	__try
	{
		if (NULL != m_pBindedColor)
		{
			*m_pBindedColor = color;
		}
		if (NULL != m_pColorValueBinded)
		{
			*m_pColorValueBinded = Color2Value(color);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		KGLogPrintf(KGLOG_ERR, _T("和ColorPicker绑定的指针已经失效"));
	}
}

VOID KColorPickerDialog::SetColor( D3DCOLOR colorInput, BOOL bModifyAlpha)
{
	D3DCOLOR color = colorInput;

	if (! bModifyAlpha)
	{
		DWORD dwAlpha = GETALPHA(GetCurrentColor());
		D3DCOLOR_SETALPHA(color, dwAlpha);
	}

	if (m_dwColorOld == color)
		return;
	
	const_cast<DWORD&>(m_dwColor) = color;

	BOOL IsOnlyAlphaIsModified = ((m_dwColorOld & 0x00ffffff) == (color & 0x00ffffff));

	m_dwColorOld = color;	

	SetColorCore(color);

	if (! IsOnlyAlphaIsModified)
	{
		RGB2HSV(GetCurrentColor(), m_HSV[0], m_HSV[1], m_HSV[2]);
	}

	m_SliderAlpha.SetPos(GETALPHA(color));

	UpdateData(FALSE);

	if (! IsOnlyAlphaIsModified)	//如果仅仅改变了Alpha，是不需要重新绘制的
	{
		CClientDC dc(this);
		this->DrawAll(&dc);
	}

	if (m_hNotifyHWND)
	{
		DWORD dwColor = m_dwColor;
		::SendMessage(m_hNotifyHWND, WM_COLORPOINTER_CHANGE, dwColor, reinterpret_cast<LPARAM>(this->GetSafeHwnd()));
	}
}

VOID KColorPickerDialog::SetColor( FLOAT fH, FLOAT fS, FLOAT fV )
{
	DWORD color = HSV2RGB(fH, fS, fV);

	DWORD dwAlpha = GETALPHA(GetCurrentColor());
	D3DCOLOR_SETALPHA(color, dwAlpha);

	if (m_dwColorOld == color)
	{
		return;
	}

	const_cast<DWORD&>(m_dwColor) = color;
	m_dwColorOld = color;
	m_HSV[0] = fH;
	m_HSV[1] = fS;
	m_HSV[2] = fV;

	SetColorCore(color);

	m_SliderAlpha.SetPos(GETALPHA(color));

	UpdateData(FALSE);
	CClientDC dc(this);
	this->DrawAll(&dc);

	if (m_hNotifyHWND)
	{
		DWORD dwColor = m_dwColor;
		::SendMessage(m_hNotifyHWND, WM_COLORPOINTER_CHANGE, dwColor, reinterpret_cast<LPARAM>(this->GetSafeHwnd()));
	}

}
void KColorPickerDialog::DrawAll( CDC* pDC )
{
	_ASSERTE(NULL != pDC);
	/*m_byChannel[0] = static_cast<BYTE>(GETRED(m_dwColor));
	m_byChannel[1] = static_cast<BYTE>(GETGREEN(m_dwColor));
	m_byChannel[2] = static_cast<BYTE>(GETBLUE(m_dwColor));
	m_byChannel[3] = static_cast<BYTE>(GETALPHA(m_dwColor));*/
	if ( m_bHSVModel )
	{
		DrawColor2HSV(pDC, m_HSV[0], m_HSV[1], m_HSV[2]);
		DrawColor1HSV(pDC, m_HSV[0], m_HSV[1], m_HSV[2]);
	}
	else
	{
		DWORD curColor = GetCurrentColor();
		DrawColor2(pDC, GETRED_BYTE(curColor), GETGREEN_BYTE(curColor), GETBLUE_BYTE(curColor));
		DrawColor1(pDC, GETRED_BYTE(curColor), GETGREEN_BYTE(curColor), GETBLUE_BYTE(curColor), s_rectColor1);
	}
	ClearVernier(pDC);
	DrawVernier(pDC);
	DrawSelectColor(pDC);
	DrawOriginColor(pDC);
	int nChannel = 3;
	/*if ( m_bAlphaEnable )
	{
		nChannel = 4;
		DrawAlpha(pDC);
	}*/
}
//////////////////////////////////////////////////////////////////////////
//KColorPickerButton方法定义开始
//////////////////////////////////////////////////////////////////////////
KColorPickerButton::KColorPickerButton(void)
:m_d3dc_bkColor( D3DCOLOR_ARGB(255, 0, 0, 0 ) )
,m_pd3dc_BindedColor(NULL)
,m_pd3dc_BindedColor_v(NULL)
{
	
}

IMPLEMENT_DYNAMIC(KColorPickerButton, CButton)

KColorPickerButton::~KColorPickerButton(void)
{
	::SendMessage( GetColorPickerDialog()->GetSafeHwnd(), WM_CLOSE, NULL, NULL );
}

void KColorPickerButton::DrawItem(LPDRAWITEMSTRUCT lpDIS /*lpDrawItemStruct*/)
{
	if (m_tczColorPickerDialogCaption == std::tstring(_T("")))
	{
		CString cs;
		this->GetWindowText(cs);
		m_tczColorPickerDialogCaption = (LPCTSTR)cs;
	}
	// TODO:  Add your code to draw the specified item
	CDC* pDC = CDC::FromHandle( lpDIS->hDC );
	CRect rect( lpDIS->rcItem );

	COLORREF CR_bkColor;
	::Color2Ref( m_d3dc_bkColor, CR_bkColor );

	CBrush	bkBrush( CR_bkColor );
	CBrush	frBrush( RGB( 0, 0, 0 ) );

	int nSaveDC = pDC->SaveDC();
	if ( 0 == nSaveDC )
	{
		goto EXIT0;
	}
	pDC->FillRect( &rect, &bkBrush );
	pDC->FrameRect( &rect, &frBrush );//黑色边框

	pDC->SetBkColor(CR_bkColor);
	BOOL bDarkBackGround = (GETRED(m_d3dc_bkColor) + GETGREEN(m_d3dc_bkColor) + GETBLUE(m_d3dc_bkColor)) / 3 < 128;
	pDC->SetTextColor(bDarkBackGround ? 0x00ffffff : 0x00000000);
	pDC->DrawText(m_tczColorPickerDialogCaption.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE );

	pDC->RestoreDC( nSaveDC );
EXIT0:
	return;
}

LRESULT KColorPickerButton::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_COLORPOINTER_CHANGE)
	{
		//m_d3dc_bkColor = m_d3dc_ColorToChange;
		if ( NULL != m_pd3dc_BindedColor )
			*m_pd3dc_BindedColor = m_d3dc_bkColor;	//同步颜色
        if ( NULL != m_pd3dc_BindedColor_v )
        {
			*m_pd3dc_BindedColor_v = ::Color2Value(m_d3dc_bkColor);            
        }
           

		Invalidate(); //强制要求绘制
		//UpdateWindow();

		CWnd* pParent = this->GetParent();
		if (pParent)
		{
			pParent->SendMessage(WM_COLORPOINTER_CHANGE
				, static_cast<WPARAM>(m_d3dc_bkColor)
				, reinterpret_cast<LPARAM>(this->GetSafeHwnd()));
		}
	}
	return CButton::WindowProc(message, wParam, lParam);
}
BEGIN_MESSAGE_MAP(KColorPickerButton, CButton)
	ON_CONTROL_REFLECT(BN_CLICKED, &KColorPickerButton::OnBnClicked)
	//ON_NOTIFY_REFLECT_EX(BN_CLICKED, &KColorPickerButton::OnBnClicked)
//	ON_WM_CLOSE()
END_MESSAGE_MAP()

//BOOL KColorPickerButton::OnBnClicked( NMHDR * pNotifyStruct, LRESULT* result )
//{
//	GetColorPickerDialog()->UnInit();
//	GetColorPickerDialog()->Init( /*&m_d3dc_ColorToChange*/ &m_d3dc_bkColor
//		, m_tczColorPickerDialogCaption.c_str(), this, IsAlphaEnable());
//	GetColorPickerDialog()->EnableWindow( TRUE );//强制要求接受输入
//	GetColorPickerDialog()->SetFocus();
//	
//	return FALSE;
//}

void KColorPickerButton::OnBnClicked()
{
	// TODO: Add your control notification handler code here
	//m_d3dc_ColorToChange = m_d3dc_bkColor;
	
	///先把之前的ColorPickerDialog杀掉才关闭
	
	GetColorPickerDialog()->UnInit();

	if (! m_bitset.test(EM_IS_COLOR_PANEL_POPPING_DISABLE))
	{
		
		GetColorPickerDialog()->Init( /*&m_d3dc_ColorToChange*/ &m_d3dc_bkColor
			, m_tczColorPickerDialogCaption.c_str(), this, IsAlphaEnable());
		GetColorPickerDialog()->EnableWindow( TRUE );//强制要求接受输入
		GetColorPickerDialog()->SetFocus();
        //GetColorPickerDialog()->CenterWindow(GetDesktopWindow());
	}
	
	//向父窗口继续发送这个消息，不然消息反射会把这个东东截了，不往父窗口发送
	HWND hWndMine = this->GetSafeHwnd();
	HWND hWndParent = ::GetParent(hWndMine);
	if (hWndParent)
	{
		INT nMyID = ::GetDlgCtrlID(hWndMine);
		::PostMessage(hWndParent, WM_COMMAND, MAKELONG(nMyID, BN_DBLCLK), (LPARAM)hWndMine);//不能发BN_CLICK不然会又回到这里
	}

}

void KColorPickerButton::SetBKColor( D3DCOLOR CR_Color )
{
	m_d3dc_bkColor = CR_Color;
	if ( NULL != m_pd3dc_BindedColor )
		*m_pd3dc_BindedColor = m_d3dc_bkColor;
	Invalidate(); //强制要求绘制
	//UpdateWindow();
}

VOID KColorPickerButton::EnableColorPanelPopping( BOOL bEnable )
{
	m_bitset.set(EM_IS_COLOR_PANEL_POPPING_DISABLE, bEnable ? false : true);
}

void KColorPickerButton::SetColorPickerDialogCaption( LPCTSTR szCaption )
{
	m_tczColorPickerDialogCaption = szCaption ? szCaption :_T("");
}

void KColorPickerButton::SetAlphaEnable( BOOL bIsEnable )
{
	m_bitset.set(EM_IS_ALPHA_ENABLE, bIsEnable?true : false);

	GetColorPickerDialog()->SetAlphaEnable(bIsEnable);
}

HRESULT KColorPickerButton::BindColor( D3DCOLORVALUE* pColor )
{
	UnBindColor();

	m_pd3dc_BindedColor_v = pColor;
	m_d3dc_bkColor = D3DCOLOR_COLORVALUE(pColor->r, pColor->g, pColor->b, pColor->a);
	return S_OK;
}

HRESULT KColorPickerButton::BindColor( D3DCOLOR* ColorToBind )
{
	UnBindColor();

	m_pd3dc_BindedColor =  ColorToBind;
	m_d3dc_bkColor      = *ColorToBind;
	return S_OK;
}

HRESULT KColorPickerButton::UnBindColor( void )
{
	m_pd3dc_BindedColor = NULL;
	m_pd3dc_BindedColor_v = NULL;
	return S_OK;
}

void KColorPickerDialog::OnEnChangeEditH()
{
	CString cs;
	if(this->GetDlgItemText(IDC_EDIT_H, cs))
	{
		FLOAT fH = static_cast<FLOAT>(_tstof(cs));
		if (fH < 0 || fH > 360.f)
		{
			MessageBox(_T("值需要在0-360之间，自动设到最接近的值"));
			if (fH < 0)
			{
				fH = 0;
			}
			else if (fH > 360.f)
			{
				fH = 360.f;
			}
		}
		SetColor(fH, m_HSV[1], m_HSV[2]);
		UpdateBitmaps();
	}
}
void KColorPickerDialog::OnEnChangeEditS()
{
	CString cs;
	if(this->GetDlgItemText(IDC_EDIT_S, cs))
	{
		FLOAT fS = static_cast<FLOAT>(_tstof(cs));
		if (fS < 0 || fS > 100.f)
		{
			MessageBox(_T("值需要在0-100之间，自动设到最接近的值"));
			if (fS < 0)
			{
				fS = 0;
			}
			else if (fS > 100.f)
			{
				fS = 100.f;
			}
		}
		SetColor(m_HSV[0], fS / 100.f, m_HSV[2]);
		UpdateBitmaps();
	}
}
void KColorPickerDialog::OnEnChangeEditV()
{
	CString cs;
	if(this->GetDlgItemText(IDC_EDIT_B, cs))
	{
		FLOAT fV = static_cast<FLOAT>(_tstof(cs));
		if (fV < 0 || fV > 100.f)
		{
			MessageBox(_T("值需要在0-100之间，自动设到最接近的值"));
			if (fV < 0)
			{
				fV = 0;
			}
			else if (fV > 100.f)
			{
				fV = 100.f;
			}
		}
		SetColor(m_HSV[0], m_HSV[1], fV / 100.0f);
		UpdateBitmaps();
	}
}
void KColorPickerDialog::OnEnChangeEditR()
{
	CString cs;
	if(this->GetDlgItemText(IDC_EDIT_RED, cs))
	{
		INT nRed = static_cast<INT>(_tstoi(cs));
		if (nRed < 0 || nRed > 255)
		{
			MessageBox(_T("值需要在0-255之间，自动设到最接近的值"));
			if (nRed < 0)
			{
				nRed = 0;
			}
			else if (nRed > 255)
			{
				nRed = 255;
			}
		}
		DWORD dwCurColor = GetCurrentColor();
		DWORD dwNewColor = D3DCOLOR_ARGB(GETALPHA(dwCurColor), nRed, GETGREEN(dwCurColor), GETBLUE(dwCurColor));
		SetColor(dwNewColor, FALSE);
		UpdateBitmaps();
	}
}
void KColorPickerDialog::OnEnChangeEditG()
{
	CString cs;
	if(this->GetDlgItemText(IDC_EDIT_GREEN, cs))
	{
		INT nGreen = static_cast<INT>(_tstoi(cs));
		if (nGreen < 0 || nGreen > 255)
		{
			MessageBox(_T("值需要在0-255之间，自动设到最接近的值"));
			if (nGreen < 0)
			{
				nGreen = 0;
			}
			else if (nGreen > 255)
			{
				nGreen = 255;
			}
		}
		DWORD dwCurColor = GetCurrentColor();
		DWORD dwNewColor = D3DCOLOR_ARGB(GETALPHA(dwCurColor), GETRED(dwCurColor), nGreen, GETBLUE(dwCurColor));
		SetColor(dwNewColor, FALSE);
		UpdateBitmaps();
	}
}
void KColorPickerDialog::OnEnChangeEditB()
{
	CString cs;
	if(this->GetDlgItemText(IDC_EDIT_BLUE, cs))
	{
		INT nBlue = static_cast<INT>(_tstoi(cs));
		if (nBlue < 0 || nBlue > 255)
		{
			MessageBox(_T("值需要在0-255之间，自动设到最接近的值"));
			if (nBlue < 0)
			{
				nBlue = 0;
			}
			else if (nBlue > 255)
			{
				nBlue = 255;
			}
		}
		DWORD dwCurColor = GetCurrentColor();
		DWORD dwNewColor = D3DCOLOR_ARGB(GETALPHA(dwCurColor), GETRED(dwCurColor), GETGREEN(dwCurColor), nBlue);
		SetColor(dwNewColor, FALSE);
		UpdateBitmaps();
	}
}
void KColorPickerDialog::OnEnChangeEditA()
{
	if (! m_bAlphaEnable)
	{
		MessageBox(_T("这个颜色指定了禁止Alpha编辑"));
		UpdateData(FALSE);
		return;
	}

	CString cs;
	if(this->GetDlgItemText(IDC_EDIT_ALPHA, cs))
	{
		INT nAlpha = static_cast<INT>(_tstoi(cs));
		if (nAlpha < 0 || nAlpha > 255)
		{
			MessageBox(_T("值需要在0-255之间，自动设到最接近的值"));
			if (nAlpha < 0)
			{
				nAlpha = 0;
			}
			else if (nAlpha > 255)
			{
				nAlpha = 255;
			}
		}
		DWORD dwCurColor = GetCurrentColor();
		DWORD dwNewColor = D3DCOLOR_ARGB(nAlpha, GETRED(dwCurColor), GETGREEN(dwCurColor), GETBLUE(dwCurColor));
		SetColor(dwNewColor, TRUE);
		
	}
}

BOOL LPCTSTR2D3DCOLOR_Custom( LPCTSTR czSource, D3DCOLOR& ResultColor, BOOL bAlphaEnable )//如果不允许Alpha，那么解释3个就好了
{
	if(NULL == czSource)
		return FALSE;

	int ColorElements[4] = {0, 0, 0, 0};
	TCHAR LocalSource[128];
	int nRet = _tcscpy_s(LocalSource, _countof(LocalSource), czSource);
	
	if(nRet != 0)
		return FALSE;
	
	TCHAR tczSeps[] = _T(" ,\t\n");
	TCHAR* pToken1 = NULL;
	TCHAR* pNext_Token = NULL;
	pToken1 = _tcstok_s(LocalSource, tczSeps, &pNext_Token);

	int nElemCountNeedToBeParse = bAlphaEnable ? 4 : 3;

	for(int i = 0; i < nElemCountNeedToBeParse; i++ )
	{
		if(NULL == pToken1)///不能全部解释出来也是错误
			return FALSE;
		int nTemp = _ttoi(pToken1);
		
		if(nTemp < 0 || nTemp > 255)
			return FALSE;
		
		ColorElements[i] = nTemp;
		pToken1 = _tcstok_s(NULL, tczSeps, &pNext_Token);
	}
	if(bAlphaEnable)
		ResultColor = D3DCOLOR_ARGB(ColorElements[0], ColorElements[1], ColorElements[2], ColorElements[3]);
	else
		ResultColor = D3DCOLOR_ARGB(0, ColorElements[0], ColorElements[1], ColorElements[2]);
	return TRUE;
}

void KColorPickerDialog::OnEnChangeEditARGB()
{
	CString cs;
	if(this->GetDlgItemText(IDC_EDIT_ARGB, cs))
	{
		D3DCOLOR colorTemp;
		BOOL bRet = LPCTSTR2D3DCOLOR_Custom(cs, colorTemp, m_bAlphaEnable);
		if(bRet)
		{
			CWnd* p = this->GetDlgItem(IDC_EDIT_ARGB);
			_ASSERTE(NULL != p);
			CPoint pt = p->GetCaretPos();

			SetColor(colorTemp, m_bAlphaEnable);
			UpdateBitmaps();

			
		}
	}
}
void KColorPickerDialog::OnEnKillfocusEditArgb()
{
	this->UpdateData(FALSE);
}
void KColorPickerDialog::OnEnKillfocusEditArgbHEX()
{
	this->UpdateData(FALSE);
}
void KColorPickerDialog::OnEnChangeEditARGBHEX()
{
	CString cs;
	if(this->GetDlgItemText(IDC_EDIT_ARGB_HEX, cs))
	{
		D3DCOLOR colorTemp;
		int nRet = _stscanf_s(cs, _T("%x"), &colorTemp);
		if(nRet)
		{
			CWnd* p = this->GetDlgItem(IDC_EDIT_ARGB);
			_ASSERTE(NULL != p);
			CPoint pt = p->GetCaretPos();

			SetColor(colorTemp, m_bAlphaEnable);
			UpdateBitmaps();

			p->SetCaretPos(pt);//让光标复位，不然SetColor刷了之后，光标会跑前面
		}
	}
}
BOOL KColorPickerDialog::OnInitDialog()
{
	BOOL bRet = CDialog::OnInitDialog();

	CClientDC dc(this);
	m_bmpLeft.CreateCompatibleBitmap(&dc, em_color_pallete_size, em_color_pallete_size);
	m_bmpRight.CreateCompatibleBitmap(&dc, em_vernier_pallete_width, em_color_pallete_size);
	//m_bmpAlpha.SetBitmapBits(em_color_pallete_size * (m_rectAlpha.bottom - m_rectAlpha.top - 2) * sizeof(DWORD), m_pAlphaBuffer);

	return bRet;
}

VOID KColorPickerDialog::SetAlphaEnable( BOOL bRet )
{
	m_bAlphaEnable = bRet;
	if (this->GetSafeHwnd())
	{
		CWnd* p = this->GetDlgItem(IDC_EDIT_ALPHA);
		if (NULL != p)
		{
			p->EnableWindow(bRet);
		}
	}
}

VOID KColorPickerDialog::UpdateBitmaps()
{
	UpdateBitmapLeft();
	UpdateBitmapRight();
	Invalidate();
}

void KColorPickerDialog::UpdateBitmapRight()
{
	_ASSERTE(1 == em_vernier_pallete_width);
	if(m_bHSVModel)
	{
		{
			float fStep;
			int i;
			DWORD colorBuffer[em_color_pallete_size];
			switch(m_enumLockChannel)
			{
			case ENUM_CHANNEL_H:
				{
					fStep = 360.0f / 255.0f;
					for ( i = 0; i < em_color_pallete_size; i++ )
					{
						colorBuffer[em_color_pallete_size - 1 - i] = HSV2RGB(fStep * i, 1.0f, 1.0f);

					}
				}
				break;
			case ENUM_CHANNEL_S:
				{
					fStep = 1.0f / 255.0f;
					for ( i = 0; i < em_color_pallete_size; i++ )
					{
						colorBuffer[em_color_pallete_size - 1 - i] = HSV2RGB(m_HSV[0], i * fStep, m_HSV[2]);
					}
				}
				break;
			case ENUM_CHANNEL_B:
				{
					fStep = 1.0f / 255.0f;
					for ( i = 0; i < em_color_pallete_size; i++ )
					{
						colorBuffer[em_color_pallete_size - 1 - i] = HSV2RGB(m_HSV[0], m_HSV[1], i * fStep);
					}
				}
				break;
			}
			m_bmpRight.SetBitmapBits(em_color_pallete_size * 4, colorBuffer);
		}
	}
	else
	{
		BYTE byRedMask   = static_cast<BYTE>((m_enumLockChannel & 0x00ff0000 & 0x00010000) >> 16);
		BYTE byGreenMask = static_cast<BYTE>((m_enumLockChannel & 0x0000ff00 & 0x00000100) >> 8);
		BYTE byBlueMask  = static_cast<BYTE>((m_enumLockChannel & 0x000000ff & 0x00000001));
		DWORD dwBase = ~m_enumLockChannel & GetCurrentColor();
		DWORD dwStep = static_cast<DWORD>(byRedMask  << 16 ) |
			static_cast<DWORD>(byGreenMask <<  8 ) |
			static_cast<DWORD>(byBlueMask) & 0x00ffffff;

		DWORD colorBuffer[em_color_pallete_size];
		//if(bUpdateBmp)
		{
			for( int i = em_color_pallete_size - 1; i >= 0; i-- )
			{
				colorBuffer[i] = dwBase;
				dwBase += dwStep;
			}
			m_bmpRight.SetBitmapBits(em_color_pallete_size * 4, colorBuffer);
		}
	}
}

void KColorPickerDialog::UpdateBitmapLeft()
{
	if(m_bHSVModel)
	{
		//if(bUpdateBmp)
		{
			float fStepX, fStepY;
			int i, j;

			DWORD colorBuffer[em_color_pallete_size * em_color_pallete_size];
			switch(m_enumLockChannel)
			{
			case ENUM_CHANNEL_H:
				{
					fStepX = 1.0f / 255.0f;
					fStepY = fStepX;
					for ( j = 0; j < em_color_pallete_size; j++ )
					{
						float v1 = j * fStepY;
						for ( i = 0; i < em_color_pallete_size; i++ )
						{
							float s1 =  i * fStepX;
							DWORD dwRGB = HSV2RGB(m_HSV[0], s1, v1);
							colorBuffer[(em_color_pallete_size - 1 - j) * em_color_pallete_size + i] = dwRGB;
						}
					}
				}
				break;
			case ENUM_CHANNEL_S:
				{
					fStepX = 360.0f / 255.0f;
					fStepY = 1.0f / 255.0f;
					for ( j = 0; j < em_color_pallete_size; j++ )
					{
						float v1 = j * fStepY;
						for ( i = 0; i < em_color_pallete_size; i++ )
						{
							float h1 =  i * fStepX;
							DWORD dwRGB = HSV2RGB(h1, m_HSV[1], v1);
							colorBuffer[(em_color_pallete_size - 1 - j) * em_color_pallete_size + i] = dwRGB;
						}
					}
				}
				break;
			case ENUM_CHANNEL_B:
				{
					fStepX = 360.0f / 255.0f;
					fStepY = 1.0f / 255.0f;
					for ( j = 0; j < em_color_pallete_size ; j++ )
					{
						float s1 = j * fStepY;
						for ( i = 0; i < em_color_pallete_size; i++ )
						{
							float h1 =  i * fStepX;
							DWORD dwRGB = HSV2RGB(h1, s1, m_HSV[2]);
							colorBuffer[(em_color_pallete_size-1 - j) * em_color_pallete_size + i] = dwRGB;
						}
					}
				}
				break;
			}
			m_bmpLeft.SetBitmapBits(em_color_pallete_size << 10 , colorBuffer);
		}
	}
	else
	{
		DWORD dwBase = m_enumLockChannel & GetCurrentColor();
		DWORD dwStepY = (0x00000100 & ~m_enumLockChannel) ? 0x00000100 : 0x00000001;
		DWORD dwStepX = (0x00010000 & ~m_enumLockChannel) ? 0x00010000 : 0x00000001;
		if ( dwStepY == 0x100 && dwStepX == 0x1 )
		{
			DWORD dwTemp = dwStepY;
			dwStepY = dwStepX;
			dwStepX = dwTemp;
		}
		//if(bUpdateBmp)
		{
			DWORD colorBuffer[em_color_pallete_size * em_color_pallete_size];

			for ( DWORD y = 0; y < em_color_pallete_size; y++ )
			{
				for ( int x = em_color_pallete_size - 1; x >= 0; x-- )
				{
					colorBuffer[y * em_color_pallete_size + x] = dwBase;
					dwBase += dwStepX;
				}
				dwBase -= (dwStepX << 8 & 0x00ffffff);
				dwBase += dwStepY;
			}
			m_bmpLeft.SetBitmapBits(em_color_pallete_size << 10, colorBuffer);
		}
	}
}

void KColorPickerDialog::OnDestroy()
{
	m_pBindedColor = NULL;
	m_pColorValueBinded = NULL;
}

VOID KColorPickerDialog::UnBindAllColorPointers()
{
	m_pBindedColor = NULL;
	m_pColorValueBinded = NULL;
}

