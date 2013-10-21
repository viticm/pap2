#include "stdafx.h"
#include ".\meshslidebar.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace ModelUI
{

//KModelUISlideBar::KModelUISlideBar(void)
//{
//	m_fPercent =  0.0f;
//	m_bEnable = TRUE;
//	m_dwStyle = 0;
//	m_dwSliderTextureID = 0;
//	m_dwSliderState = 0;
//	m_bMoving = FALSE;
//
//	m_MovingStartPos = D3DXVECTOR2(0,0);
//
//}
//
//KModelUISlideBar::~KModelUISlideBar(void)
//{
//}
//
//HRESULT KModelUISlideBar::CreateAbsolute(LPFRECT pRect,LPSTR StyleFile,BOOL State,DWORD Style)
//{
//	KModelUIBase::CreateAbsolute(pRect,StyleFile,State,Style);
//	m_bShowTitle = FALSE;
//	m_bEnable  = State;
//	m_dwStyle  = Style;
//
//	TCHAR Name[256];
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"ToolBar\\Slider.tga");
//	g_cTextureTable.LoadFromFile(&m_dwSliderTextureID,Name);
//
//	return S_OK;
//}
//HRESULT KModelUISlideBar::Create(LPFRECT pRect,LPSTR StyleFile,BOOL State,DWORD Style)
//{
//	KModelUIBase::Create(pRect,StyleFile,State,Style);
//
//	m_bShowTitle = FALSE;
//	m_bEnable  = State;
//	m_dwStyle  = Style;
//
//	TCHAR Name[256];
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"ToolBar\\Slider.tga");
//	g_cTextureTable.LoadFromFile(&m_dwSliderTextureID,Name);
//
//	return E_NOTIMPL;
//}
//
//HRESULT KModelUISlideBar::Render(void)
//{
//	KModelUIBase::Render();
//
//	D3DXVECTOR2 A,C,A1,C1,A2,C2;
//	m_Rect.ToPosition(&A,&C);
//
//	A1 = A;A2 = A;
//	C1 = C;C2 = C;
//
//	if (m_dwStyle==0)
//	{
//		A1.y = A.y + m_Rect.Height * 0.35f;
//		C1.y = A.y + m_Rect.Height * 0.65f;
//		A2.x = A.x + m_Rect.Width * m_fPercent*0.01f - m_Rect.Width * 0.04f;
//		C2.x = A.x + m_Rect.Width * m_fPercent*0.01f + m_Rect.Width * 0.04f;
//	}
//	else
//	{
//		A1.x = A.x + m_Rect.Width * 0.35f;
//		C1.x = A.x + m_Rect.Width * 0.65f;
//		A2.y = A.y + m_Rect.Height * m_fPercent*0.01f - m_Rect.Height * 0.04f;
//		C2.y = A.y + m_Rect.Height * m_fPercent*0.01f + m_Rect.Height * 0.04f;
//	}
//
//	g_cGraphicsTool.DrawScreenRect(&A1,&C1,0,1,1,0xFF353535,0);
//
//	if (m_dwSliderState==0)
//		g_cGraphicsTool.DrawScreenRect(&A2,&C2,0,1,1,0xFFAAAAAA,m_dwSliderTextureID);
//	else
//		g_cGraphicsTool.DrawScreenRect(&A2,&C2,0,1,1,0xFFFFFFFF,m_dwSliderTextureID);
//
//	/*D3DVIEWPORT9 ViewPort;
//	g_pd3dDevice->GetViewport(&ViewPort);
//
//	D3DXVECTOR2 A,C,A1,C1,A2,C2;
//	A.x = ViewPort.X + ViewPort.Width  * m_fLeft;
//	A.y = ViewPort.Y + ViewPort.Height * m_fTop;
//	float R_W = ViewPort.Width  * TERRIANCELLWIDTH;
//	float R_H = ViewPort.Height * m_fHeight;
//	C.x = A.x + R_W;
//	C.y = A.y + R_H;
//
//	//m_fPercent = (sinf(timeGetTime()*0.001f)+1)*50;
//
//	A1 = A;A2 = A;
//	C1 = C;C2 = C;
//
//	if (m_dwStyle==0)
//	{
//		A1.y = A.y + R_H * 0.35f;
//		C1.y = A.y + R_H * 0.65f;
//		A2.x = A.x + R_W * m_fPercent*0.01f - R_W * 0.04f;
//		C2.x = A.x + R_W * m_fPercent*0.01f + R_W * 0.04f;
//	}
//	else
//	{
//		A1.x = A.x + R_W * 0.35f;
//		C1.x = A.x + R_W * 0.65f;
//		A2.y = A.y + R_H * m_fPercent*0.01f - R_H * 0.04f;
//		C2.y = A.y + R_H * m_fPercent*0.01f + R_H * 0.04f;
//	}
//
//
//	//g_cGraphicsTool.DrawScreenRectD(&A,&C,0,0xFFAAAAAA,0xFFAAAAAA,0xFFAAAAAA,0xFFAAAAAA);
//
//	g_cGraphicsTool.DrawScreenRect(&A1,&C1,0,1,1,0xFF353535,0);
//	if (m_dwSliderState==0)
//		g_cGraphicsTool.DrawScreenRect(&A2,&C2,0,1,1,0xFFAAAAAA,m_dwSliderTextureID);
//	else
//		g_cGraphicsTool.DrawScreenRect(&A2,&C2,0,1,1,0xFFFFFFFF,m_dwSliderTextureID);
//		*/
//	return S_OK;
//}
//
//HRESULT KModelUISlideBar::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	POINT ptCursor;
//	::GetCursorPos( &ptCursor );
//
//	D3DXVECTOR2 Pos;
//	Pos.x = (float)ptCursor.x;
//	Pos.y = (float)ptCursor.y;
//
//	D3DXVECTOR2 A,C,A1,C1,A2,C2;
//	m_Rect.ToPosition(&A,&C);
//
//	A1 = A;A2 = A;
//	C1 = C;C2 = C;
//
//	if (m_dwStyle==0)
//	{
//		A1.y = A.y + m_Rect.Height * 0.35f;
//		C1.y = A.y + m_Rect.Height * 0.65f;
//		A2.x = A.x + m_Rect.Width * m_fPercent*0.01f - m_Rect.Width * 0.04f;
//		C2.x = A.x + m_Rect.Width * m_fPercent*0.01f + m_Rect.Width * 0.04f;
//	}
//	else
//	{
//		A1.x = A.x + m_Rect.Width * 0.35f;
//		C1.x = A.x + m_Rect.Width * 0.65f;
//		A2.y = A.y + m_Rect.Height * m_fPercent*0.01f - m_Rect.Height * 0.04f;
//		C2.y = A.y + m_Rect.Height * m_fPercent*0.01f + m_Rect.Height * 0.04f;
//	}
//
//	if ((Pos.x >= A2.x)&&(Pos.x <= C2.x)&&
//		(Pos.y >= A2.y)&&(Pos.y <= C2.y))
//	{
//		m_dwSliderState = 1;
//		if (message==WM_LBUTTONDOWN)
//		{
//			m_bMoving = TRUE;
//			m_MovingStartPos = Pos;
//			return SLIDER_CLICK;
//		}
//	}
//	else
//	{
//		m_dwSliderState = 0;
//	}
//
//	if (message==WM_LBUTTONUP)
//	{
//		m_bMoving = FALSE;
//	}
//
//	if ((message==WM_MOUSEMOVE)&&(wParam&MK_LBUTTON)&&m_bMoving)
//	{
//		m_dwSliderState = 1;
//		D3DXVECTOR2 Dis = Pos/* - m_MovingStartPos*/;
//
//		if (m_dwStyle==0)
//		{
//			float XX  = (Dis.x - A.x)*100/m_Rect.Width;
//			m_fPercent = XX;
//			if (m_fPercent<=0)
//				m_fPercent = 0;
//			if (m_fPercent>=100)
//				m_fPercent = 100;
//		}
//		else
//		{
//			float YY  = (Dis.y - A.y)*100/m_Rect.Height;
//			m_fPercent = YY;
//			if (m_fPercent<=0)
//				m_fPercent = 0;
//			if (m_fPercent>=100)
//				m_fPercent = 100;
//		}
//		return SLIDER_MOVE;
//	}
//	return S_OK;
//}
};
