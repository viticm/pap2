// KColorPickerDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "SceneEditor.h"
#include "KColorDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

D3DCOLORVALUE Color2Value(D3DCOLOR Color)
{
	D3DCOLORVALUE nColor;
	nColor.a = ((float)(Color>>24))/255.0f;
	nColor.r = ((float)((Color&0x00ff0000)>>16))/255.0f;
	nColor.g = ((float)((Color&0x0000ff00)>>8))/255.0f;
	nColor.b = ((float)((Color&0x000000ff)))/255.0f;
	return nColor;
}

VOID Color2Value( D3DCOLOR Color, D3DCOLORVALUE& Value )
{
	Value.a = ((float)(Color>>24))/255.0f;
	Value.r = ((float)((Color&0x00ff0000)>>16))/255.0f;
	Value.g = ((float)((Color&0x0000ff00)>>8))/255.0f;
	Value.b = ((float)((Color&0x000000ff)))/255.0f;
}
D3DCOLOR ColorValue(D3DCOLORVALUE co)
{
	return D3DCOLOR_ARGB((int)(co.a*255),(int)(co.r*255),
		(int)(co.g*255),(int)(co.b*255));
}
COLORREF ColorValue2ref(D3DCOLORVALUE co)
{
	return RGB(co.r*255,co.g*255,co.b*255);
}
D3DCOLORVALUE Colorref2value(COLORREF Color,float a)
{
	D3DCOLORVALUE nColor;
	nColor.r = 1.0f;
	nColor.g = 1.0f;
	nColor.b = 1.0f;
	nColor.a = a;
	nColor.b = ((float)(Color>>16))/255.0f;
	nColor.g = ((float)((Color&0x00ff00)>>8))/255.0f;
	nColor.r = ((float)((Color&0x0000ff)))/255.0f;
	return nColor;
}
D3DCOLOR Value2Color( const D3DCOLORVALUE& D3DColorValue )
{
	return D3DCOLOR_COLORVALUE( D3DColorValue.r, D3DColorValue.g, D3DColorValue.b, D3DColorValue.a );
}
void Color2Ref( const D3DCOLOR &D3DColor, COLORREF &CRColor )//CTH Added
{
	CRColor = 0x00000000 //COLORREF的高位衡为0
		| ((D3DColor&0x00ff0000)>>16) //R
		| (D3DColor&0x0000ff00) 
		| ((D3DColor&0x000000ff)<<16);//B
}
///////////////////////////////////////////////////////////////////////

KColorPickerDialog g_ColorPicker;

KColorPickerDialog* GetColorPickerDialog()
{
	return &g_ColorPicker;
}

IMPLEMENT_DYNAMIC(KColorPickerDialog, CDialog)


KColorPickerDialog::KColorPickerDialog()
{
	m_bCreated = FALSE;
	m_bUpdateBmp1 = TRUE;
	m_bUpdateBmp2 = TRUE;
	m_pbmpColor1 = NULL;
	m_pbmpColor2 = NULL;
	m_pBmpAlpha = NULL;
	m_pAlphaBuffer = NULL;
	m_bAutoUpdate = FALSE;
	m_pColorBuffer1 = new DWORD[256 * 256];
	m_pColorBuffer2 = new DWORD[256];
}

void KColorPickerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RED, m_byChannel[0]);
	DDX_Text(pDX, IDC_EDIT_GREEN, m_byChannel[1]);
	DDX_Text(pDX, IDC_EDIT_BLUE, m_byChannel[2]);
	DDX_Text(pDX, IDC_EDIT_ALPHA, m_byChannel[3]);
	DDX_Text(pDX, IDC_EDIT_H, m_HSV[0]);
	DDX_Text(pDX, IDC_EDIT_S, m_HSV[1]);
	DDX_Text(pDX, IDC_EDIT_B, m_HSV[2]);
}

const RECT KColorPickerDialog::m_rectColor1 = { 9,  9, 267, 267};
const RECT KColorPickerDialog::m_rectColor2 = { 286, 9, 312, 267};
int KColorPickerDialog::m_nWidth2;
const int KColorPickerDialog::m_nSpan = 2;
const POINT KColorPickerDialog::m_ptVernier = {10, 11};
const RECT KColorPickerDialog::m_rectVernierRgn = { m_rectColor2.left - m_ptVernier.x - m_nSpan,
m_rectColor2.top + 1,
m_rectColor2.right + m_ptVernier.x + m_nSpan,
m_rectColor2.bottom - 1 };

void KColorPickerDialog::Init(D3DCOLORVALUE* pColor , LPCSTR strCaption, CWnd* pNotify /*= NULL*/, BOOL bAlphaEnable /* = TRUE*/)
{
	TCHAR strWindowCaption[256];
	m_bAutoUpdate = TRUE;

	sprintf(strWindowCaption, "颜色选择 - %s", strCaption);
	m_dwColor = ColorValue(*pColor);
	m_pColor = &m_dwColor;
	//m_dwColor = pManager->ColorValue(*pColor);
	m_pColorValue = pColor;

	m_pNotify = pNotify;
	m_dwColorOrg = *m_pColor;
	m_nGridSize = 5;
	m_bUseColorValue = TRUE;
	m_bAlphaEnable = bAlphaEnable;

	m_nWidth2 = abs(m_rectColor2.right - m_rectColor2.left - 2);



	m_bVernierSelect = FALSE;
	m_bAlphaSelect = FALSE;
	m_bVernierMoved = FALSE;
	m_bAlphaMoved = FALSE;
	m_bColor1Select = FALSE;
	m_bHSVModel = TRUE;

	m_enumLockChannel = ENUM_CHANNEL_H;


	RECT rectSel = { 340, 9, 410, 49};
	m_rectSelectColor = rectSel;

	RECT rectOrg = { rectSel.left, rectSel.bottom, rectSel.right, rectSel.bottom + 39};
	m_rectOriginColor = rectOrg;

	RECT rectSelAndOrg = { rectSel.left, rectSel.top, rectOrg.right, rectOrg.bottom };
	m_rectSelAndOrg = rectSelAndOrg;

	RECT rectClear0 = { m_rectColor2.left - m_nSpan - m_ptVernier.x - 1,
		max(m_rectColor2.top - 6, 0),
		m_rectColor2.left - m_nSpan + 1,
		m_rectColor2.bottom + 6};
	m_rectClearRgn[0] = rectClear0;

	RECT rectClear1 = { m_rectColor2.right + m_nSpan - 1,
		m_rectClearRgn[0].top,
		m_rectColor2.right + m_nSpan - 1 + m_ptVernier.x + 2,
		m_rectClearRgn[0].bottom };
	m_rectClearRgn[1] = rectClear1;

	m_byChannel[0] = static_cast<BYTE>(GETRED(*m_pColor));
	m_byChannel[1] = static_cast<BYTE>(GETGREEN(*m_pColor));
	m_byChannel[2] = static_cast<BYTE>(GETBLUE(*m_pColor));
	m_byChannel[3] = static_cast<BYTE>(GETALPHA(*m_pColor));
	RGB2HSV(*m_pColor, m_HSV[0], m_HSV[1], m_HSV[2]);

	//if ( m_bAlphaEnable )
	{
		//Alpha 滑动条
		RECT rectAlpha = {m_rectColor1.left, m_rectColor1.bottom + 2, m_rectColor1.right, rectAlpha.top + 17};
		m_rectAlpha = rectAlpha;
		if(!m_pAlphaBuffer)
			m_pAlphaBuffer = new DWORD[256 * abs(m_rectAlpha.bottom - m_rectAlpha.top - 2)];
		RECT rectAlphaRgn = { rectAlpha.left, rectAlpha.top, rectAlpha.right, rectAlpha.bottom + m_ptVernier.y + m_nSpan + 1 };
		m_rectAlphaRgn = rectAlphaRgn;

		RECT rectClear2 = { max(m_rectAlpha.left - m_ptVernier.x/2 - 1, 0),
			m_rectAlpha.bottom + 1,
			m_rectAlpha.right + m_ptVernier.x /2 + 1,
			m_rectAlpha.bottom + m_ptVernier.y + m_nSpan + 1};
		m_rectClearRgn[2] = rectClear2;
	}
	static BOOL bCreate = FALSE;
	if(!m_bCreated)
	{
		Create(IDD);
		m_bCreated = TRUE;
	}
	ShowWindow(SW_SHOW);
	((CButton*)(GetDlgItem(IDC_RADIO_H)))->SetCheck(BST_CHECKED);
	SetWindowText(strWindowCaption);
	UpdateData(FALSE);
	Invalidate();
	BringWindowToTop();
}

void KColorPickerDialog::Init(DWORD* pColor, LPCSTR strCaption, CWnd* pNotify/* = NULL */, BOOL bAlphaEnable /* = TRUE */)
{
	TCHAR strWindowCaption[256];
	sprintf(strWindowCaption, "颜色选择 - %s", strCaption);
	m_pColor = pColor;
	m_bAutoUpdate = TRUE;
	m_pNotify = pNotify;
	m_dwColorOrg = *m_pColor;
	m_nGridSize = 5;
	m_bUseColorValue = FALSE;
	m_bAlphaEnable = bAlphaEnable;

	m_nWidth2 = abs(m_rectColor2.right - m_rectColor2.left - 2);

	m_bVernierSelect = FALSE;
	m_bAlphaSelect = FALSE;
	m_bVernierMoved = FALSE;
	m_bAlphaMoved = FALSE;
	m_bColor1Select = FALSE;
	m_bHSVModel = TRUE;

	m_enumLockChannel = ENUM_CHANNEL_H;


	RECT rectSel = { 340, 9, 410, 49};
	m_rectSelectColor = rectSel;

	RECT rectOrg = { rectSel.left, rectSel.bottom, rectSel.right, rectSel.bottom + 39};
	m_rectOriginColor = rectOrg;

	RECT rectSelAndOrg = { rectSel.left, rectSel.top, rectOrg.right, rectOrg.bottom };
	m_rectSelAndOrg = rectSelAndOrg;

	RECT rectClear0 = { m_rectColor2.left - m_nSpan - m_ptVernier.x - 1,
		max(m_rectColor2.top - 6, 0),
		m_rectColor2.left - m_nSpan + 1,
		m_rectColor2.bottom + 6};
	m_rectClearRgn[0] = rectClear0;

	RECT rectClear1 = { m_rectColor2.right + m_nSpan - 1,
		m_rectClearRgn[0].top,
		m_rectColor2.right + m_nSpan - 1 + m_ptVernier.x + 2,
		m_rectClearRgn[0].bottom };
	m_rectClearRgn[1] = rectClear1;

	m_byChannel[0] = static_cast<BYTE>(GETRED(*pColor));
	m_byChannel[1] = static_cast<BYTE>(GETGREEN(*pColor));
	m_byChannel[2] = static_cast<BYTE>(GETBLUE(*pColor));
	m_byChannel[3] = static_cast<BYTE>(GETALPHA(*pColor));
	RGB2HSV(*pColor, m_HSV[0], m_HSV[1], m_HSV[2]);

	//if ( m_bAlphaEnable )
	{
		RECT rectAlpha = {m_rectColor1.left, m_rectColor1.bottom + 2, m_rectColor1.right, rectAlpha.top + 17};
		m_rectAlpha = rectAlpha;
		if(!m_pAlphaBuffer)
			m_pAlphaBuffer = new DWORD[256 * abs(m_rectAlpha.bottom - m_rectAlpha.top - 2)];
		RECT rectAlphaRgn = { rectAlpha.left, rectAlpha.top, rectAlpha.right, rectAlpha.bottom + m_ptVernier.y + m_nSpan + 1 };
		m_rectAlphaRgn = rectAlphaRgn;

		RECT rectClear2 = { max(m_rectAlpha.left - m_ptVernier.x/2 - 1, 0),
			m_rectAlpha.bottom + 1,
			m_rectAlpha.right + m_ptVernier.x /2 + 1,
			m_rectAlpha.bottom + m_ptVernier.y + m_nSpan + 1};
		m_rectClearRgn[2] = rectClear2;
	}
	if(!m_bCreated)
	{
		Create(IDD);
		m_bCreated = TRUE;
	}
	ShowWindow(SW_SHOW);
	((CButton*)(GetDlgItem(IDC_RADIO_H)))->SetCheck(BST_CHECKED);
	SetWindowText(strWindowCaption);
	Invalidate();
	BringWindowToTop();
}

KColorPickerDialog::~KColorPickerDialog(void)
{
	SAFE_DELETE_ARRAY(m_pColorBuffer1);
	SAFE_DELETE_ARRAY(m_pColorBuffer2);
	SAFE_DELETE_ARRAY(m_pAlphaBuffer);
	//m_pbmpColor2->DeleteObject();
	//m_pbmpColor1->DeleteObject();
	//m_pBmpAlpha->DeleteObject();
	SAFE_DELETE(m_pbmpColor2);
	SAFE_DELETE(m_pbmpColor1);
	SAFE_DELETE(m_pBmpAlpha);

}


/*
*@brief 现实左边的颜色选择框
*@param pDc     窗口的DC
*@param byRed   红色通道的值
*@param byGreen 绿色通道的值
*@param byBlue  蓝色通道的值
*@param rect    没什么用, 本来想作为重画的区域,不过没有做
*/
void KColorPickerDialog::DrawColor1(CDC* pDc,BYTE byRed, BYTE byGreen, BYTE byBlue, const RECT& rect, BOOL bUpdateBmp/* = TRUE */)
{
	if( !m_pbmpColor1)
	{
		m_pbmpColor1 = new CBitmap;
		m_pbmpColor1->CreateCompatibleBitmap(pDc, 256, 256);
	}
	DWORD dwInputColor = (static_cast<DWORD>(byRed)<<16 | static_cast<DWORD>(byGreen)<<8 | static_cast<DWORD>(byBlue));
	DWORD dwBase = m_enumLockChannel & dwInputColor;
	DWORD dwStepY = (0x00000100 & ~m_enumLockChannel) ? 0x00000100 : 0x00000001;
	DWORD dwStepX = (0x00010000 & ~m_enumLockChannel) ? 0x00010000 : 0x00000001;
	if ( dwStepY == 0x100 && dwStepX == 0x1 )
	{
		DWORD dwTemp = dwStepY;
		dwStepY = dwStepX;
		dwStepX = dwTemp;
	}
	if(bUpdateBmp)
	{
		for ( DWORD y = 0; y < 256; y++ )
		{
			for ( int x = 255; x >= 0; x-- )
			{
				m_pColorBuffer1[y * 256 + x] = dwBase;
				dwBase += dwStepX;
			}
			dwBase -= (dwStepX << 8 & 0x00ffffff);
			dwBase += dwStepY;
		}
		m_pbmpColor1->SetBitmapBits(256 << 10, m_pColorBuffer1);
	}
	CBrush Brush(m_pbmpColor1);
	CBrush* pOldBrush = pDc->SelectObject(&Brush);
	pDc->SetBrushOrg(m_rectColor1.left + 1, m_rectColor1.top + 1);
	pDc->Rectangle(&m_rectColor1);

	POINT pt;
	DWORD dwMask = ~m_enumLockChannel & *m_pColor;
	pt.x = GETRED(~m_enumLockChannel) ? GETRED(dwMask) : GETGREEN(dwMask);
	pt.y = GETBLUE(~m_enumLockChannel) ? GETBLUE(*m_pColor) : GETGREEN(dwMask);

	RECT rectClip;
	CPen Pen(PS_SOLID, 1, RGB((255 - pt.x), (255 - pt.y), (255 - GETBLUE(*m_pColor))));
	//dc.SetDCPenColor(RGB((255 - pt.x), (255 - pt.y), (255 - GETBLUE(*m_pColor))));
	CPen* pOldPen = pDc->SelectObject(&Pen);
	pDc->GetClipBox(&rectClip);
	CRgn Rgn;
	CRgn Rgn1;
	Rgn.CreateRectRgnIndirect(&m_rectColor1);
	pDc->SelectClipRgn(&Rgn);
	pDc->SelectObject(GetStockObject(HOLLOW_BRUSH));
	pDc->Ellipse( m_rectColor1.right - 1 - pt.x - 3,
		pt.y + m_rectColor1.top + 1 - 3,
		m_rectColor1.right - 1 - pt.x + 3,
		pt.y + m_rectColor1.top + 1 + 3);
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
void KColorPickerDialog::DrawColor2(CDC* pDc, BYTE byRed, BYTE byGreen , BYTE byBlue, BOOL bUpdateBmp)
{
	if( !m_pbmpColor2 )
	{
		m_pbmpColor2 = new CBitmap;
		m_pbmpColor2->CreateCompatibleBitmap(pDc, 1, 256);
	}

	BYTE byRedMask   = static_cast<BYTE>((m_enumLockChannel & 0x00ff0000 & 0x00010000) >> 16);
	BYTE byGreenMask = static_cast<BYTE>((m_enumLockChannel & 0x0000ff00 & 0x00000100) >> 8);
	BYTE byBlueMask  = static_cast<BYTE>((m_enumLockChannel & 0x000000ff & 0x00000001));
	DWORD dwBase = ~m_enumLockChannel & (static_cast<DWORD>(byRed) << 16 | static_cast<DWORD>(byGreen) << 8 | static_cast<DWORD>(byBlue));
	DWORD dwStep = static_cast<DWORD>(byRedMask  << 16 ) |
		static_cast<DWORD>(byGreenMask <<  8 ) |
		static_cast<DWORD>(byBlueMask) & 0x00ffffff;
	if(bUpdateBmp)
	{
		for( int i = 255; i >= 0; i-- )
		{
			m_pColorBuffer2[i] = dwBase;
			dwBase += dwStep;
		}
		m_pbmpColor2->SetBitmapBits(256 * 4, m_pColorBuffer2);
	}
	CBrush Brush(m_pbmpColor2);
	CBrush* pOldBrush = pDc->SelectObject(&Brush);
	pDc->SetBrushOrg(m_rectColor2.left + 1, m_rectColor2.top + 1);
	pDc->Rectangle(&m_rectColor2);
	pDc->SelectObject(pOldBrush);
}

void KColorPickerDialog::DrawColor1HSV(CDC* pDc, float h, float s, float v, BOOL bUpdateBmp)
{
	float fStepX, fStepY;
	int i, j;
	if( !m_pbmpColor1 )
	{
		m_pbmpColor1 = new CBitmap;
		m_pbmpColor1->CreateCompatibleBitmap(pDc, 256, 256);
	}
	if(bUpdateBmp)
	{
		switch(m_enumLockChannel)
		{
		case ENUM_CHANNEL_H:
			{
				fStepX = 1.0f / 255.0f;
				fStepY = fStepX;
				for ( j = 0; j < 256; j++ )
				{
					float v1 = j * fStepY;
					for ( i = 0; i < 256; i++ )
					{
						float s1 =  i * fStepX;
						DWORD dwRGB = HSV2RGB(h, s1, v1) & 0x00ffffff;
						//m_pColorBuffer1[j * 256 + 255 - i] = dwRGB;
						m_pColorBuffer1[(255 - j) * 256 + i] = dwRGB;
					}
				}
			}
			break;
		case ENUM_CHANNEL_S:
			{
				fStepX = 360.0f / 255.0f;
				fStepY = 1.0f / 255.0f;
				for ( j = 0; j < 256; j++ )
				{
					float v1 = j * fStepY;
					for ( i = 0; i < 256; i++ )
					{
						float h1 =  i * fStepX;
						DWORD dwRGB = HSV2RGB(h1, s, v1);
						m_pColorBuffer1[(255 - j) * 256 + i] = dwRGB;
					}
				}
			}
			break;
		case ENUM_CHANNEL_B:
			{
				fStepX = 360.0f / 255.0f;
				fStepY = 1.0f / 255.0f;
				for ( j = 0; j < 256 ; j++ )
				{
					float s1 = j * fStepY;
					for ( i = 0; i < 256; i++ )
					{
						float h1 =  i * fStepX;
						DWORD dwRGB = HSV2RGB(h1, s1, v) & 0x00ffffff;
						m_pColorBuffer1[(255 - j) * 256 + i] = dwRGB;
					}
				}
			}
			break;
		}
		m_pbmpColor1->SetBitmapBits(256 << 10 , m_pColorBuffer1);
	}
	CBrush Brush(m_pbmpColor1);
	CBrush* pOldBrush = pDc->SelectObject(&Brush);
	pDc->SetBrushOrg(m_rectColor1.left + 1, m_rectColor1.top + 1);
	pDc->Rectangle(&m_rectColor1);

	POINT pt = { 0, 0 };
	switch(m_enumLockChannel)
	{
	case ENUM_CHANNEL_B:
		{
			//pt.x = static_cast<int>((1.0f - m_HSV[0] / 360.0f) * 255.0f);
			//pt.y = static_cast<int>((1.0f - m_HSV[1])  * 255.0f);
			pt.x = static_cast<int>(m_HSV[0] * 255.0f);
			pt.y = static_cast<int>(m_HSV[1] * 255.0f);
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
	CPen Pen(PS_SOLID, 1, RGB((255 - pt.x), (255 - pt.y), (255 - GETBLUE(*m_pColor))));
	//dc.SetDCPenColor(RGB((255 - pt.x), (255 - pt.y), (255 - GETBLUE(*m_pColor))));
	CPen* pOldPen = pDc->SelectObject(&Pen);
	pDc->GetClipBox(&rectClip);
	CRgn Rgn;
	CRgn Rgn1;
	Rgn.CreateRectRgnIndirect(&m_rectColor1);
	pDc->SelectClipRgn(&Rgn);
	pDc->SelectObject(GetStockObject(HOLLOW_BRUSH));
	pDc->Ellipse( m_rectColor1.right - 1 - pt.x - 3,
		pt.y + m_rectColor1.top + 1 - 3,
		m_rectColor1.right - 1 - pt.x + 3,
		pt.y + m_rectColor1.top + 1 + 3);
	Rgn1.CreateRectRgnIndirect(&rectClip);
	pDc->SelectClipRgn(&Rgn1);
	pDc->SelectObject(pOldPen);
	pDc->SelectObject(pOldBrush);
}

void KColorPickerDialog::DrawColor2HSV(CDC* pDc, float h, float s, float v, BOOL bUpdateBmp)
{
	float fStep;
	int i;
	if( !m_pbmpColor2 )
	{
		m_pbmpColor2 = new CBitmap;
		m_pbmpColor2->CreateCompatibleBitmap(pDc, 1, 256);
	}
	if(bUpdateBmp)
	{
		switch(m_enumLockChannel)
		{
		case ENUM_CHANNEL_H:
			{
				fStep = 360.0f / 255.0f;
				for ( i = 0; i < 256; i++ )
				{
					m_pColorBuffer2[255 - i] = HSV2RGB(fStep * i, 1.0f, 1.0f);

				}
			}
			break;
		case ENUM_CHANNEL_S:
			{
				fStep = 1.0f / 255.0f;
				for ( i = 0; i < 256; i++ )
				{
					m_pColorBuffer2[255 - i] = HSV2RGB(h, i * fStep, v);
				}
			}
			break;
		case ENUM_CHANNEL_B:
			{
				fStep = 1.0f / 255.0f;
				for ( i = 0; i < 256; i++ )
				{
					m_pColorBuffer2[255 - i] = HSV2RGB(h, s, i * fStep);
				}
			}
			break;
		}
		m_pbmpColor2->SetBitmapBits(256 << 2, m_pColorBuffer2);
	}
	CBrush Brush(m_pbmpColor2);
	CBrush* pOldBrush = pDc->SelectObject(&Brush);
	pDc->SetBrushOrg(m_rectColor2.left + 1, m_rectColor2.top + 1);
	pDc->Rectangle(&m_rectColor2);
	pDc->SelectObject(pOldBrush);

}



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
END_MESSAGE_MAP()


void KColorPickerDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	m_byChannel[0] = static_cast<BYTE>(GETRED(*m_pColor));
	m_byChannel[1] = static_cast<BYTE>(GETGREEN(*m_pColor));
	m_byChannel[2] = static_cast<BYTE>(GETBLUE(*m_pColor));
	m_byChannel[3] = static_cast<BYTE>(GETALPHA(*m_pColor));
	if ( m_bHSVModel )
	{
		DrawColor2HSV(&dc, m_HSV[0], m_HSV[1], m_HSV[2]);
		DrawColor1HSV(&dc, m_HSV[0], m_HSV[1], m_HSV[2]);
	}
	else
	{
		DrawColor2(&dc, m_byChannel[0], m_byChannel[1], m_byChannel[2]);
		DrawColor1(&dc, m_byChannel[0], m_byChannel[1], m_byChannel[2], m_rectColor1);
	}
	ClearVernier(&dc);
	DrawVernier(&dc);
	DrawSelectColor(&dc);
	DrawOriginColor(&dc);
	int nChannel = 3;
	if ( m_bAlphaEnable )
	{
		nChannel = 4;
		DrawAlpha(&dc);
	}
	//TCHAR strValue[4];
	//for(int i = 0; i < nChannel; i++ )
	//{
	//	sprintf(strValue, "%d", m_byChannel[i]);
	//	//m_ctlEditChannel[i].SetWindowText(strValue);
	//}
}

/*
*@brief 显示下方的Alpha条
*@param pDC    窗口的DC
*/
void KColorPickerDialog::DrawAlpha(CDC* pDc)
{
	if ( !m_pBmpAlpha )
	{
		m_pBmpAlpha = new CBitmap;
		m_pBmpAlpha->CreateCompatibleBitmap(pDc, 256, 10);
	}
	DWORD dwColor = (*m_pColor) & 0x00ffffff;
	DWORD dwAlpha = (*m_pColor) & 0xff000000;
	int nFlag = 0;
	for ( int i = 0; i < 256; i++ )
	{
		if (( i / m_nGridSize ) % 2 )
			nFlag = 1;
		else
			nFlag = 0;
		float fAlpha = i / 255.0f;
		BYTE byGrey = static_cast<BYTE>(192 * (1.0f - fAlpha));
		BYTE byWhite = static_cast<BYTE>(255 * (1.0f - fAlpha));
		DWORD dwColorBKGrey = (static_cast<DWORD>(GETRED(dwColor)   * fAlpha + byGrey) << 16) |
			(static_cast<DWORD>(GETGREEN(dwColor) * fAlpha + byGrey )<< 8 ) |
			(static_cast<DWORD>(GETBLUE(dwColor)  * fAlpha + byGrey)      ) & 0x00ffffff;
		DWORD dwColorBKWhite = (static_cast<DWORD>(GETRED(dwColor)   * fAlpha + byWhite )<< 16) |
			(static_cast<DWORD>(GETGREEN(dwColor) * fAlpha + byWhite )<< 8 ) |
			(static_cast<DWORD>(GETBLUE(dwColor)  * fAlpha + byWhite)      ) & 0x00ffffff;
		for( int j = 0; j < 10; j++ )
		{
			if (( nFlag + j / m_nGridSize ) % 2)
				*(m_pAlphaBuffer + i + j * 256) = dwColorBKGrey;
			else
				*(m_pAlphaBuffer + i + j * 256) = dwColorBKWhite;
		}
	}
	m_pBmpAlpha->SetBitmapBits(256 * (m_rectAlpha.bottom - m_rectAlpha.top - 2) * sizeof(DWORD), m_pAlphaBuffer);
	CBrush Brush(m_pBmpAlpha);
	CBrush* pOldBrush = pDc->SelectObject(&Brush);
	pDc->Rectangle(&m_rectAlpha);
	pDc->SelectObject(pOldBrush);
}

/*
*@brief 清除游标区域
*@param pDC    窗口的DC
*/
void KColorPickerDialog::ClearVernier(CDC* pDc)
{
	COLORREF crBK =RGB(0xEC,0xE9, 0xD8);
	CBrush Brush(crBK);
	CPen Pen(PS_SOLID, 1, crBK);
	CBrush* pOldBrush = pDc->SelectObject(&Brush);
	CPen* pOldPen = pDc->SelectObject(&Pen);
	if( m_bVernierMoved )
	{
		pDc->Rectangle(&m_rectClearRgn[0]);
		pDc->Rectangle(&m_rectClearRgn[1]);
	}
	if ( m_bAlphaEnable && m_bAlphaMoved)
	{
		pDc->Rectangle(&m_rectClearRgn[2]);
		m_bAlphaMoved = FALSE;
	}
	m_bVernierMoved = FALSE;
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

	POINT ptPos = { m_rectVernierRgn.left, m_rectColor2.bottom - 1 - (((*m_pColor)&m_enumLockChannel)>>nOffset) - m_ptVernier.y / 2};
	if ( m_bHSVModel )
	{
		switch(m_enumLockChannel)
		{
		case ENUM_CHANNEL_B:
			{
				ptPos.y = m_rectColor2.bottom - 1 - static_cast<int>(m_HSV[2] * 255.0f) - m_ptVernier.y / 2;
			}
			break;
		case ENUM_CHANNEL_H:
			{
				ptPos.y = m_rectColor2.bottom - 1 - static_cast<int>(m_HSV[0] / 360.0f * 255.0f) - m_ptVernier.y / 2;
			}
			break;
		case ENUM_CHANNEL_S:
			{
				ptPos.y = m_rectColor2.bottom - 1 - static_cast<int>(m_HSV[1] * 255.0f) - m_ptVernier.y / 2;
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
				pDc->LineTo(m_rectVernierRgn.left + m_ptVernier.x, m_rectColor2.bottom - 1 - static_cast<int>(m_HSV[2] * 255.0f));
			}
			break;
		case ENUM_CHANNEL_H:
			{
				pDc->LineTo(m_rectVernierRgn.left + m_ptVernier.x, m_rectColor2.bottom - 1 - static_cast<int>(m_HSV[0] / 360.0f  * 255.0f));
			}
			break;
		case ENUM_CHANNEL_S:
			{
				pDc->LineTo(m_rectVernierRgn.left + m_ptVernier.x, m_rectColor2.bottom - 1 - static_cast<int>(m_HSV[1] * 255.0f));
			}
			break;
		}
		//pDc->LineTo(m_rectVernierRgn.left + m_ptVernier.x, m_rectColor2.bottom - 1 - (((*m_pColor)&m_enumLockChannel)>>nOffset));
	}
	else
	{
		pDc->LineTo(m_rectVernierRgn.left + m_ptVernier.x, m_rectColor2.bottom - 1 - (((*m_pColor)&m_enumLockChannel)>>nOffset));
	}
	pDc->LineTo(ptPos.x, ptPos.y + m_ptVernier.y);
	pDc->LineTo(ptPos);
	pDc->FloodFill(ptPos.x + 1 , ptPos.y + m_ptVernier.y / 2, RGB(0, 0, 0));

	//Right vernier
	ptPos.x = m_rectVernierRgn.right;
	pDc->MoveTo(ptPos);
	if ( m_bHSVModel )
	{
		switch(m_enumLockChannel)
		{
		case ENUM_CHANNEL_B:
			{
				pDc->LineTo(m_rectVernierRgn.right - m_ptVernier.x, m_rectColor2.bottom - 1 - static_cast<int>(m_HSV[2] * 255.0f));
			}
			break;
		case ENUM_CHANNEL_H:
			{
				pDc->LineTo(m_rectVernierRgn.right - m_ptVernier.x, m_rectColor2.bottom - 1 - static_cast<int>(m_HSV[0] / 360.0f * 255.0f));
			}
			break;
		case ENUM_CHANNEL_S:
			{
				pDc->LineTo(m_rectVernierRgn.right - m_ptVernier.x, m_rectColor2.bottom - 1 - static_cast<int>(m_HSV[1] * 255.0f));
			}
			break;
		}
	}
	else
	{
		pDc->LineTo(m_rectVernierRgn.right - m_ptVernier.x, m_rectColor2.bottom - 1 - (((*m_pColor)&m_enumLockChannel)>>nOffset));
	}
	pDc->LineTo(ptPos.x, ptPos.y + m_ptVernier.y);
	pDc->LineTo(ptPos);
	pDc->FloodFill(ptPos.x - 1, ptPos.y + m_ptVernier.y / 2, RGB(0, 0, 0));

	if ( m_bAlphaEnable)//Draw alpha vernier
	{
		ptPos.x = GETALPHA(*m_pColor) + m_rectAlpha.left - 1;
		ptPos.y = m_rectAlpha.bottom + m_nSpan;
		pDc->MoveTo(ptPos.x, ptPos.y);
		pDc->LineTo(ptPos.x + m_ptVernier.x/2, ptPos.y + m_ptVernier.y);
		pDc->LineTo(ptPos.x - m_ptVernier.x/2, ptPos.y + m_ptVernier.y);
		pDc->LineTo(ptPos.x, ptPos.y);
		pDc->FloodFill(ptPos.x, ptPos.y + 1, RGB(0, 0, 0));
	}
	pDc->SelectObject(pOldPen);
	pDc->SelectObject(pOldBrush);
}

void KColorPickerDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	CClientDC dc(this);
	SetCapture();
	//RGB2HSV(*m_pColor, m_HSV[0], m_HSV[1], m_HSV[2]);
	//m_HSV[1] *= 100.0f;
	//m_HSV[2] *= 100.0f;
	if ( ::PtInRect(&m_rectVernierRgn, point) )//If in color selection box2(right)
	{
		m_bVernierSelect = TRUE;
		m_bVernierMoved = TRUE;
		m_bUpdateBmp1 = TRUE;
		m_bUpdateBmp2 = FALSE;
		DWORD dwPos = max(min(m_rectColor2.bottom - 1 - point.y, 255), 0);
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
			*m_pColor = HSV2RGB(m_HSV[0], m_HSV[1], m_HSV[2]) | (static_cast<DWORD>(m_byChannel[3])<<24);
		}
		else
		{
			dwPos = dwPos<<16 | dwPos<<8 | dwPos;
			*m_pColor = (*m_pColor)&(~m_enumLockChannel) | (dwPos & m_enumLockChannel) | (static_cast<DWORD>(m_byChannel[3])<<24);
		}
		m_byChannel[0] = static_cast<BYTE>(GETRED(*m_pColor));
		m_byChannel[1] = static_cast<BYTE>(GETGREEN(*m_pColor));
		m_byChannel[2] = static_cast<BYTE>(GETBLUE(*m_pColor));
		ClearVernier(&dc);
		DrawVernier(&dc);
		if ( m_bHSVModel )
		{
			DrawColor2HSV(&dc, m_HSV[0], m_HSV[1], m_HSV[2]);
			DrawColor1HSV(&dc, m_HSV[0], m_HSV[1], m_HSV[2]);
		}
		else
		{
			DrawColor2(&dc, m_byChannel[0], m_byChannel[1], m_byChannel[2]);
			DrawColor1(&dc, m_byChannel[0], m_byChannel[1], m_byChannel[2], m_rectColor1);
		}
		DrawSelectColor(&dc);
		if (m_bAlphaEnable)
			DrawAlpha(&dc);

		UpdateData(FALSE);

		if (m_bUseColorValue)
		{
			*m_pColorValue = Color2Value(*m_pColor);
		}

		if (m_pNotify)
		{
			DWORD dwColor = *m_pColor;
			m_pNotify->SendMessage(WM_COLORPOINTER_CHANGE, dwColor, reinterpret_cast<LPARAM>(this->GetSafeHwnd()));
		}
	}
	else if ( ::PtInRect(&m_rectColor1, point) )//If in color selection box1(left)
	{
		m_bColor1Select = TRUE;
		m_bUpdateBmp1 = FALSE;
		m_bUpdateBmp2 = TRUE;
		if ( m_bHSVModel )//HSV mode
		{
			int i1 = max(min(m_rectColor1.right - point.x, 0xff), 0);
			int i2 = max(min(point.y - m_rectColor1.top + 1, 0xff), 0);
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
			*m_pColor = HSV2RGB(m_HSV[0], m_HSV[1], m_HSV[2]) | (static_cast<DWORD>(m_byChannel[3])<<24);
		}
		else
		{
			DWORD dwChannel1 = DWORD(max(min(m_rectColor1.right - point.x, 0xff), 0));
			DWORD dwChannel2 = DWORD(max(min(point.y - m_rectColor1.top + 1, 0xff), 0));
			*m_pColor &= m_enumLockChannel;
			*m_pColor |= (~m_enumLockChannel & (dwChannel1<<16)) ? (dwChannel1<<16) : (dwChannel1<<8);
			*m_pColor |= (~m_enumLockChannel & dwChannel2) ? dwChannel2 : (dwChannel2<<8);
			*m_pColor |= static_cast<DWORD>(m_byChannel[3])<<24;
		}

		m_byChannel[0] = static_cast<BYTE>(GETRED(*m_pColor));
		m_byChannel[1] = static_cast<BYTE>(GETGREEN(*m_pColor));
		m_byChannel[2] = static_cast<BYTE>(GETBLUE(*m_pColor));
		ClearVernier(&dc);
		DrawVernier(&dc);
		if ( m_bHSVModel )
		{
			DrawColor2HSV(&dc, m_HSV[0], m_HSV[1], m_HSV[2], m_bUpdateBmp2);
			DrawColor1HSV(&dc, m_HSV[0], m_HSV[1], m_HSV[2], m_bUpdateBmp1);
		}
		else
		{
			DrawColor2(&dc, m_byChannel[0], m_byChannel[1], m_byChannel[2], m_bUpdateBmp2);
			DrawColor1(&dc, m_byChannel[0], m_byChannel[1], m_byChannel[2], m_rectColor1, m_bUpdateBmp2);
		}
		DrawSelectColor(&dc);
		if ( m_bAlphaEnable )
			DrawAlpha(&dc);
		UpdateData(FALSE);
		if ( m_bUseColorValue )
		{
			*m_pColorValue = Color2Value(*m_pColor);
		}

		if (m_pNotify)
		{
			DWORD dwColor = *m_pColor;
			m_pNotify->SendMessage(WM_COLORPOINTER_CHANGE, dwColor, reinterpret_cast<LPARAM>(this->GetSafeHwnd()));
		}
	}
	else if( ::PtInRect(&m_rectOriginColor, point) )//Select original color
	{
		m_bVernierMoved = TRUE;
		m_bAlphaMoved = TRUE;
		m_bUpdateBmp1 = TRUE;
		m_bUpdateBmp2 = TRUE;
		*m_pColor = m_dwColorOrg;
		m_byChannel[3] = static_cast<BYTE>(GETALPHA(*m_pColor));
		m_byChannel[0] = static_cast<BYTE>(GETRED(*m_pColor));
		m_byChannel[1] = static_cast<BYTE>(GETGREEN(*m_pColor));
		m_byChannel[2] = static_cast<BYTE>(GETBLUE(*m_pColor));
		ClearVernier(&dc);
		DrawVernier(&dc);
		if ( m_bHSVModel )
		{
			DrawColor2HSV(&dc, m_HSV[0], m_HSV[1], m_HSV[2], m_bUpdateBmp2);
			DrawColor1HSV(&dc, m_HSV[0], m_HSV[1], m_HSV[2], m_bUpdateBmp1);
		}
		else
		{
			DrawColor2(&dc, m_byChannel[0], m_byChannel[1], m_byChannel[2], m_bUpdateBmp2);
			DrawColor1(&dc, m_byChannel[0], m_byChannel[1], m_byChannel[2], m_rectColor1, m_bUpdateBmp1);
		}
		DrawSelectColor(&dc);
		if (m_bAlphaEnable)
		{
			DrawAlpha(&dc);
		}

		UpdateData(FALSE);
		if ( m_bUseColorValue )
		{
			*m_pColorValue = Color2Value(*m_pColor);
		}

		if (m_pNotify)
		{
			DWORD dwColor = *m_pColor;
			m_pNotify->SendMessage(WM_COLORPOINTER_CHANGE, dwColor, reinterpret_cast<LPARAM>(this->GetSafeHwnd()));
		}
	}
	else if ( ::PtInRect(&m_rectAlphaRgn, point) )//If in alpha selection region
	{
		m_bAlphaSelect = TRUE;
		m_bAlphaMoved = TRUE;
		m_bUpdateBmp1 = FALSE;
		m_bUpdateBmp2 = FALSE;
		m_byChannel[3] = static_cast<BYTE>(max(min(point.x - m_rectAlpha.left + 1, 0xff), 0));
		DWORD dwAlpha = static_cast<DWORD>(m_byChannel[3])<<24 & 0xff000000;
		*m_pColor &= 0x00ffffff;
		*m_pColor |= dwAlpha;
		m_byChannel[0] = static_cast<BYTE>(GETRED(*m_pColor));
		m_byChannel[1] = static_cast<BYTE>(GETGREEN(*m_pColor));
		m_byChannel[2] = static_cast<BYTE>(GETBLUE(*m_pColor));
		m_byChannel[3] = static_cast<BYTE>(GETALPHA(*m_pColor));
		ClearVernier(&dc);
		DrawVernier(&dc);
		if (m_bHSVModel)
		{

		}
		else
		{
			DrawColor2(&dc, m_byChannel[0], m_byChannel[1], m_byChannel[2], m_bUpdateBmp2);
			DrawColor1(&dc, m_byChannel[0], m_byChannel[1], m_byChannel[2], m_rectColor1, m_bUpdateBmp1);
		}
		//DrawSelectColor(&dc);
		//if ( m_bAlphaEnable )
		//	DrawAlpha(&dc);
		UpdateData(FALSE);
		if (m_bUseColorValue)
		{
			*m_pColorValue = Color2Value(*m_pColor);
		}

		if (m_pNotify)
		{
			DWORD dwColor = *m_pColor;
			m_pNotify->SendMessage(WM_COLORPOINTER_CHANGE, dwColor, reinterpret_cast<LPARAM>(this->GetSafeHwnd()));
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
	CBrush Brush(RGB(GETRED(*m_pColor), GETGREEN(*m_pColor), GETBLUE(*m_pColor)));
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
	m_bVernierSelect = FALSE;
	m_bColor1Select = FALSE;
	m_bAlphaSelect = FALSE;
	m_bUpdateBmp1 = FALSE;
	m_bUpdateBmp2 = FALSE;
	*m_pColor |= static_cast<DWORD>(m_byChannel[3])<<24;
	ReleaseCapture();
	CDialog::OnLButtonUp(nFlags, point);
}

BOOL KColorPickerDialog::OnInit()
{
	//Called in Create, after CWnd::Create() is called
	//Create controls in the dialog
	BOOL bReturn = TRUE;
	RECT rectRadio = { 340, 140, 380, 160 };
	RECT rectEdit = { 380, 140, 410, 160};

	bReturn &= m_ctlChannel[0].Create("R:", BS_AUTORADIOBUTTON|WS_GROUP|WS_VISIBLE|WS_CHILD, rectRadio, this, IDC_CHANNEL_RED);
	bReturn &= m_ctlEditChannel[0].Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_RIGHT|WS_CHILD|WS_DISABLED, rectEdit, this, IDC_CHANNEL_RED + 1);

	rectRadio.top += 22;
	rectRadio.bottom += 22;
	rectEdit.top += 22;
	rectEdit.bottom += 22;
	bReturn &= m_ctlChannel[1].Create("G:", BS_AUTORADIOBUTTON|WS_VISIBLE|WS_CHILD, rectRadio, this, IDC_CHANNEL_GREEN);
	bReturn &= m_ctlEditChannel[1].Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_RIGHT|WS_CHILD|WS_DISABLED, rectEdit, this, IDC_CHANNEL_GREEN + 1);

	rectRadio.top += 22;
	rectRadio.bottom += 22;
	rectEdit.top += 22;
	rectEdit.bottom += 22;

	bReturn &= m_ctlChannel[2].Create("B:", BS_AUTORADIOBUTTON|WS_VISIBLE|WS_CHILD, rectRadio, this, IDC_CHANNEL_BLUE);
	bReturn &= m_ctlEditChannel[2].Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_RIGHT|WS_CHILD|WS_DISABLED, rectEdit, this, IDC_CHANNEL_BLUE + 1);

	int nChannel = 3;
	if ( m_bAlphaEnable )
	{
		/*rectRadio.top += 22;
		rectRadio.bottom += 22;
		rectEdit.top += 22;
		rectEdit.bottom += 22;

		bReturn &= m_ctlAlphaStatic.Create("Alpha:", WS_CHILD | WS_VISIBLE,  rectRadio, this);
		bReturn &= m_ctlEditChannel[3].Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_RIGHT|WS_CHILD|WS_DISABLED, rectEdit, this, IDC_CHANNEL_ALPHA + 1);*/

		nChannel = 4;
	}

	//TCHAR strValue[4];
	//for(int i = 0; i < nChannel; i++ )
	//{
	//	sprintf(strValue, "%d", m_byChannel[i]);
	//	//m_ctlEditChannel[i].SetWindowText(strValue);
	//	//m_ctlEditChannel[i].RedrawWindow();
	//}

	m_ctlChannel[2].SetCheck(BST_CHECKED);

	return bReturn;
}

void KColorPickerDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	CClientDC dc(this);
	//static RECT rectAjustColor1 = { m_rectColor1.left - 5, m_rectColor1.top - 5, m_rectColor1.right + 5, m_rectColor1.bottom + 5};
	//TCHAR strValue[255];
	//RGB2HSV(*m_pColor, m_HSV[0], m_HSV[1], m_HSV[2]);
	//m_HSV[1] *= 100.0f;
	//m_HSV[2] *= 100.0f;
	if (m_bColor1Select)
	{
		if ( m_bHSVModel )
		{
			int i1 = m_rectColor1.right - point.x - 1;
			int i2 = point.y - m_rectColor1.top - 1;
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
			*m_pColor = HSV2RGB(m_HSV[0], m_HSV[1], m_HSV[2]) | (static_cast<DWORD>(m_byChannel[3])<<24);
		}
		else
		{
			//If left button down and point is in the color1 rectangle, change color as well
			int i1 = m_rectColor1.right - point.x - 1;
			int i2 = point.y - m_rectColor1.top - 1;

			//Make sure the value is in the range of ( 0 - 255 )
			DWORD dwChannel1 = DWORD(max(min(i1, 0xff), 0));
			DWORD dwChannel2 = DWORD(max(min(i2, 0xff), 0));

			*m_pColor &= m_enumLockChannel;
			*m_pColor |= (~m_enumLockChannel & (dwChannel1<<16)) ? (dwChannel1<<16) : (dwChannel1<<8);
			*m_pColor |= (~m_enumLockChannel & dwChannel2) ? dwChannel2 : (dwChannel2<<8);
			*m_pColor |= static_cast<DWORD>(m_byChannel[3])<<24;//加上Alpha
			m_byChannel[0] = static_cast<BYTE>(GETRED(*m_pColor));
			m_byChannel[1] = static_cast<BYTE>(GETGREEN(*m_pColor));
			m_byChannel[2] = static_cast<BYTE>(GETBLUE(*m_pColor));

			//for(int i = 0; i < 3; i++ )
			//{
			//	sprintf(strValue, "%d", m_byChannel[i]);
			//	//m_ctlEditChannel[i].SetWindowText(strValue);
			//	//m_ctlEditChannel[i].RedrawWindow();
			//}

		}

		m_byChannel[0] = static_cast<BYTE>(GETRED(*m_pColor));
		m_byChannel[1] = static_cast<BYTE>(GETGREEN(*m_pColor));
		m_byChannel[2] = static_cast<BYTE>(GETBLUE(*m_pColor));
		ClearVernier(&dc);
		DrawVernier(&dc);
		if ( m_bHSVModel )
		{
			DrawColor2HSV(&dc, m_HSV[0], m_HSV[1], m_HSV[2]);
			DrawColor1HSV(&dc, m_HSV[0], m_HSV[1], m_HSV[2]);
		}
		else
		{
			DrawColor2(&dc, m_byChannel[0], m_byChannel[1], m_byChannel[2]);
			DrawColor1(&dc, m_byChannel[0], m_byChannel[1], m_byChannel[2], m_rectColor1);
		}
		DrawSelectColor(&dc);
		if ( m_bAlphaEnable )
			DrawAlpha(&dc);
		UpdateData(FALSE);
		if ( m_bUseColorValue )
		{
			*m_pColorValue = Color2Value(*m_pColor);
		}

		if (m_pNotify)
		{
			DWORD dwColor = *m_pColor;
			m_pNotify->SendMessage(WM_COLORPOINTER_CHANGE, dwColor, reinterpret_cast<LPARAM>(this->GetSafeHwnd()));
		}
	}
	else if ( m_bVernierSelect)
	{
		//If the vernier is selected, move the vernier
		m_bVernierMoved = TRUE;
		m_bAlphaMoved = TRUE;
		DWORD dwPos = max(min(m_rectColor2.bottom - 1 - point.y, 255), 0);
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
			*m_pColor = HSV2RGB(m_HSV[0], m_HSV[1], m_HSV[2]) | (static_cast<DWORD>(m_byChannel[3])<<24);
		}
		else
		{
			dwPos = dwPos<<16 | dwPos<<8 | dwPos;
			*m_pColor = (*m_pColor)&(~m_enumLockChannel) | (dwPos & m_enumLockChannel) | (static_cast<DWORD>(m_byChannel[3])<<24);
		}
		//for(int i = 0; i < 3; i++ )
		//{
		//	sprintf(strValue, "%d", m_byChannel[i]);
		//	//m_ctlEditChannel[i].SetWindowText(strValue);
		//	//m_ctlEditChannel[i].RedrawWindow();
		//}
		m_byChannel[0] = static_cast<BYTE>(GETRED(*m_pColor));
		m_byChannel[1] = static_cast<BYTE>(GETGREEN(*m_pColor));
		m_byChannel[2] = static_cast<BYTE>(GETBLUE(*m_pColor));
		ClearVernier(&dc);
		DrawVernier(&dc);
		if ( m_bHSVModel )
		{
			DrawColor2HSV(&dc, m_HSV[0], m_HSV[1], m_HSV[2]);
			DrawColor1HSV(&dc, m_HSV[0], m_HSV[1], m_HSV[2]);
		}
		else
		{
			DrawColor2(&dc, m_byChannel[0], m_byChannel[1], m_byChannel[2]);
			DrawColor1(&dc, m_byChannel[0], m_byChannel[1], m_byChannel[2], m_rectColor1);
		}
		DrawSelectColor(&dc);
		if ( m_bAlphaEnable )
			DrawAlpha(&dc);

		UpdateData(FALSE);
		if ( m_bUseColorValue )
		{
			*m_pColorValue = Color2Value(*m_pColor);
		}

		if (m_pNotify)
		{
			DWORD dwColor = *m_pColor;
			m_pNotify->SendMessage(WM_COLORPOINTER_CHANGE, dwColor, reinterpret_cast<LPARAM>(this->GetSafeHwnd()));
		}
	}
	else if ( m_bAlphaSelect )//Alpha vernier is selected
	{
		m_bAlphaMoved = TRUE;
		m_byChannel[3] = static_cast<BYTE>(max(min(point.x - m_rectAlpha.left + 1, 0xff), 0));
		*m_pColor &= 0x00ffffff;
		*m_pColor |= static_cast<DWORD>(m_byChannel[3]) << 24;
		m_byChannel[0] = static_cast<BYTE>(GETRED(*m_pColor));
		m_byChannel[1] = static_cast<BYTE>(GETGREEN(*m_pColor));
		m_byChannel[2] = static_cast<BYTE>(GETBLUE(*m_pColor));
		ClearVernier(&dc);
		DrawVernier(&dc);
		UpdateData(FALSE);
		if ( m_bUseColorValue )
		{
			*m_pColorValue = Color2Value(*m_pColor);
		}

		if (m_pNotify)
		{
			DWORD dwColor = *m_pColor;
			m_pNotify->SendMessage(WM_COLORPOINTER_CHANGE, dwColor, reinterpret_cast<LPARAM>(this->GetSafeHwnd()));
		}
	}
	CDialog::OnMouseMove(nFlags, point);
}



void KColorPickerDialog::PostNcDestroy()
{
	//	delete this;
}

void KColorPickerDialog::OnOK()
{
	UpdateData();
}

void KColorPickerDialog::RGB2HSV(DWORD rgb, float& h, float& s, float& v)
{
	float ColorMin;
	float ColorMax;
	float r = GETRED(rgb) / 255.0f;
	float g = GETGREEN(rgb)/ 255.0f;
	float b = GETBLUE(rgb) / 255.0f;
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
}

DWORD KColorPickerDialog::HSV2RGB(float h, float s, float v)
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
		i = floor(h);
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
	return dwRGB;
}


void KColorPickerDialog::OnBnClickedRadioH()
{
	m_bHSVModel = TRUE;
	m_enumLockChannel = ENUM_CHANNEL_H;
	m_bVernierMoved = TRUE;
	Invalidate();
}

void KColorPickerDialog::OnBnClickedRadioS()
{
	m_bHSVModel = TRUE;
	m_enumLockChannel = ENUM_CHANNEL_S;
	m_bVernierMoved = TRUE;
	Invalidate();
}

void KColorPickerDialog::OnBnClickedRadioB()
{
	m_bHSVModel = TRUE;
	m_enumLockChannel = ENUM_CHANNEL_B;
	m_bVernierMoved = TRUE;
	Invalidate();
}

void KColorPickerDialog::OnBnClickedRadioRed()
{
	m_bHSVModel = FALSE;
	m_enumLockChannel = ENUM_CHANNEL_RED;
	m_bVernierMoved = TRUE;
	Invalidate();
}

void KColorPickerDialog::OnBnClickedRadioGreen()
{
	m_bHSVModel = FALSE;
	m_enumLockChannel = ENUM_CHANNEL_GREEN;
	m_bVernierMoved = TRUE;
	Invalidate();
}

void KColorPickerDialog::OnBnClickedRadioBlue()
{
	m_bHSVModel = FALSE;
	m_enumLockChannel = ENUM_CHANNEL_BLUE;
	m_bVernierMoved = TRUE;
	Invalidate();
}


LRESULT KColorPickerDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{
	case WM_COLORPOINTER_CHANGE:
		{
			//通知颜色的指针发生了变化
			m_pColor = reinterpret_cast<DWORD*>(wParam);
			return 0;
		}
		break;
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
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void KColorPickerDialog::OnCancel()
{
	UnInit();
	//DestroyWindow();

	CDialog::OnCancel();
}
VOID KColorPickerDialog::UnInit(void)
{
	if ( m_pNotify )
	{
		WPARAM pWnd = reinterpret_cast<WPARAM>(this);
		::SendMessage(m_pNotify->GetSafeHwnd(), WM_COLORDIALOG_CLOSE, pWnd, NULL);
		m_pNotify = NULL;
	}
	m_bAutoUpdate = FALSE;
}

//void KColorPickerDialog::SetColorPointer(D3DCOLORVALUE* pNewValue, LPCSTR strCaption, CWnd* pNotity /*= NULL*/, BOOL bAlpha /*= TRUE*/)
/*{
IEKG3DEngineManager *pManager = NULL;
TCHAR strWindowCaption[256];

Get3DEngineInterface((void **)(&pManager));
sprintf(strWindowCaption, "颜色选择 - %s", strCaption);
m_dwColor = pManager->ColorValue(*pNewValue);
m_pColor = &m_dwColor;
m_bUseColorValue = TRUE;
m_pNotify = pNotify;
m_bAlpha = bAlpha;
m_pNotify = pNotify;
m_dwColorOrg = *m_pColor;
m_pColor = pNewValue;
SetWindowText(strWindowCaption);
Invalidate();
}*/

BOOL KColorPickerDialog::PreTranslateMessage(MSG* pMsg)
{
	int i = 0;
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
			dwColor |= static_cast<DWORD>(m_byChannel[3]) << 24;
		}
		else
		{
			dwColor = static_cast<DWORD>(
				((m_byChannel[3] & 0xff) << 24) |
				((m_byChannel[0] & 0xff) << 16) |
				((m_byChannel[1] & 0xff) <<  8) | 
				((m_byChannel[2] & 0xff) <<  0)
				);
		}


		*m_pColor = dwColor;
		if(m_bUseColorValue)
			*m_pColorValue = Color2Value(dwColor);


		Invalidate();
		m_pNotify->SendMessage(WM_COLORPOINTER_CHANGE, dwColor, reinterpret_cast<LPARAM>(this->GetSafeHwnd()));
		return TRUE;
	}
	else return CDialog::PreTranslateMessage(pMsg);
}


//////////////////////////////////////////////////////////////////////////
//KColorPickerButton方法定义开始
//////////////////////////////////////////////////////////////////////////
KColorPickerButton::KColorPickerButton(void)
:m_d3dc_bkColor( D3DCOLOR_XRGB( 255, 0, 0 ) )
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
	// TODO:  Add your code to draw the specified item
	CDC* pDC = CDC::FromHandle( lpDIS->hDC );
	CRect rect( lpDIS->rcItem );

	COLORREF CR_bkColor;
	Color2Ref( m_d3dc_bkColor, CR_bkColor );

	CBrush	bkBrush( CR_bkColor );
	CBrush	frBrush( RGB( 0, 0, 0 ) );

	int nSaveDC = pDC->SaveDC();
	if ( 0 == nSaveDC )
	{
		goto EXIT0;
	}
	pDC->FillRect( &rect, &bkBrush );
	pDC->FrameRect( &rect, &frBrush );//黑色边框
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
			Color2Value(m_d3dc_bkColor, *m_pd3dc_BindedColor_v);            
		}


		Invalidate(); //强制要求绘制
		UpdateWindow();

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
	//	ON_WM_CLOSE()
END_MESSAGE_MAP()

void KColorPickerButton::OnBnClicked()
{
	// TODO: Add your control notification handler code here
	//m_d3dc_ColorToChange = m_d3dc_bkColor;

	///先把之前的ColorPickerDialog杀掉才关闭

	GetColorPickerDialog()->UnInit();
	GetColorPickerDialog()->Init( /*&m_d3dc_ColorToChange*/ &m_d3dc_bkColor
		, m_tczColorPickerDialogCaption.c_str(), this, IsAlphaEnable());
	GetColorPickerDialog()->EnableWindow( TRUE );//强制要求接受输入
	GetColorPickerDialog()->SetFocus();
}

void KColorPickerButton::SetBKColor( D3DCOLOR CR_Color )
{
	m_d3dc_bkColor = CR_Color;
	if ( NULL != m_pd3dc_BindedColor )
		*m_pd3dc_BindedColor = m_d3dc_bkColor;
	Invalidate(); //强制要求绘制
	//UpdateWindow();
}

