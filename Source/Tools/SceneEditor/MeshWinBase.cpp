// MeshWinBase.cpp: implementation of the KModelUIBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "MeshWinBase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace ModelUI
{
//KModelUIBase::KModelUIBase()
//{
//	m_bEnbaleScroll = FALSE;
//	m_bAutoResize = TRUE;
//	m_bScrolling = FALSE;
//	
//	m_fZ = 0.001f;
//	m_dwStyle = 0;
//	m_dwState = 0;
//	m_lpParentWnd = NULL;
//	
//	m_TitleCaption = "  标题";
//
//	m_bShowTitle = TRUE;
//	m_bFocused = FALSE;
//	m_fTitleHeight = 16.0f;
//    m_BorderColor = 0xFF101010;
//	m_BackColor   = 0xFFAAAAAA;
//	m_TitleColor  = 0xFFA0A0A0;
//	m_TitleColorFocus  = 0xFF0000FF;
//
//	m_fBorderWidth = 1.0f;
//	m_bMoving = FALSE;
//}
//
//KModelUIBase::~KModelUIBase()
//{
//
//}
//
//HRESULT KModelUIBase::Create(LPFRECT pRect,LPSTR StyleFile,BOOL State,DWORD Style)
//{
//	m_RelativeRect = *pRect;
//	m_dwState = State;
//	m_dwStyle = Style;
//	ComputeRect();
//	return S_OK;
//}
//
//HRESULT KModelUIBase::CreateAbsolute(LPFRECT pRect,LPSTR StyleFile,BOOL State,DWORD Style)
//{
//	m_bAutoResize = FALSE;
//	m_Rect = *pRect;
//	m_dwState = State;
//	m_dwStyle = Style;
//	ComputeRect();
//	return S_OK;
//}
//
//HRESULT KModelUIBase::ComputeRect()
//{
//	if(m_bAutoResize)
//	{
//		if (m_lpParentWnd)
//		{
//			m_RelativeRect = m_RelativeParentRect + m_lpParentWnd->m_RelativeRect; 
//		}
//
//
//		D3DVIEWPORT9 ViewPort;
//		g_pd3dDevice->GetViewport(&ViewPort);
//
//		m_Rect.Left = ViewPort.X + ViewPort.Width  * m_RelativeRect.Left;
//		m_Rect.Top  = ViewPort.Y + ViewPort.Height * m_RelativeRect.Top;
//
//		m_Rect.Width  = ViewPort.Width  * m_RelativeRect.Width;
//		m_Rect.Height = ViewPort.Height * m_RelativeRect.Height;
//
//	}
//	else
//	{
//		if (m_lpParentWnd)
//		{
//			m_Rect = m_ParentRect + m_lpParentWnd->m_Rect; 
//		}
//	}
//
//	m_BorderRect.Left   = m_Rect.Left  + m_fBorderWidth;
//	m_BorderRect.Top    = m_Rect.Top   + m_fBorderWidth;
//	m_BorderRect.Width  = m_Rect.Width - 2*m_fBorderWidth;
//	m_BorderRect.Height = m_Rect.Height  - 2*m_fBorderWidth;
//
//	m_TitleRect.Left   = m_BorderRect.Left;
//	m_TitleRect.Width  = m_BorderRect.Width;
//	m_TitleRect.Top    = m_BorderRect.Top;
//	m_TitleRect.Height = m_fTitleHeight;
//
//	m_ClientRect.Left  = m_BorderRect.Left;
//	m_ClientRect.Width = m_BorderRect.Width;
//
//	if (m_bShowTitle)
//	{
//		m_ClientRect.Top    = m_BorderRect.Top + m_fTitleHeight;
//		m_ClientRect.Height = m_BorderRect.Height - m_fTitleHeight;
//	}
//	else
//	{
//		m_ClientRect.Top    = m_BorderRect.Top ;
//		m_ClientRect.Height = m_BorderRect.Height ;
//	}
//	return S_OK;
//}
//
//HRESULT KModelUIBase::CleanUp()
//{
//    return KModel::CleanUp();
//}
//
//HRESULT KModelUIBase::Render()
//{	
//	ComputeRect();
//
//	D3DXVECTOR2 A,B,C,D;
//
//	m_Rect.ToPosition(&A,&C);
//	g_pd3dDevice->SetTexture(0,NULL);
//
//	g_cGraphicsTool.SetUIDefaultRenderState();
//
//	g_cGraphicsTool.DrawScreenRectD(&A,&C,m_fZ,m_BorderColor,m_BorderColor,m_BorderColor,m_BorderColor);
//		
//	{
//		B.x = C.x;B.y = A.y;
//		D.x = A.x;D.y = C.y;
//		
//		g_cGraphicsTool.DrawScreenLine(&A,&B,m_fZ,0xFFFFFFFF,0xFFFFFFFF);
//		g_cGraphicsTool.DrawScreenLine(&A,&D,m_fZ,0xFFFFFFFF,0xFFFFFFFF);
//		g_cGraphicsTool.DrawScreenLine(&C,&B,m_fZ,0xFF444444,0xFF444444);
//		g_cGraphicsTool.DrawScreenLine(&C,&D,m_fZ,0xFF444444,0xFF444444);
//	}
//
//	m_BorderRect.ToPosition(&A,&C);
//	g_cGraphicsTool.DrawScreenRectD(&A,&C,m_fZ,m_BackColor,m_BackColor,m_BackColor,m_BackColor);
//
//	if (m_bShowTitle)
//	{
//		m_TitleRect.ToPosition(&A,&C);
//		if (m_bFocused)
//			g_cGraphicsTool.DrawScreenRectD(&A,&C,m_fZ,
//			m_TitleColorFocus,m_TitleColorFocus,
//			m_TitleColorFocus,m_TitleColorFocus);
//		else
//			g_cGraphicsTool.DrawScreenRectD(&A,&C,m_fZ,
//			m_TitleColor,m_TitleColor,
//			m_TitleColor,m_TitleColor);
//		float X = A.x + 4;
//		float Y = A.y + 1;
//		g_cGraphicsTool.DrawTextInRect(&m_TitleRect,m_fZ,0xFF000000,10,"宋体",(LPSTR)m_TitleCaption.c_str(),0);
//	}
//	
//	g_pd3dDevice->SetTexture(0,NULL);
//	m_ClientRect.ToPosition(&A,&C);
//	
//	/*g_cGraphicsTool.DrawScreenRectD(&A,&C,0.0f,0xFF770077,0xFF770077,0xFF770077,0xFF770077);
//	{
//		g_cGraphicsTool.DrawTextInRect(&m_ClientRect,0.0f,0xFF000000,18,"宋体","基督教分列式地方啊",0);
//	}*/
//	return S_OK;
//}
//
//HRESULT KModelUIBase::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	D3DXVECTOR2 A,C;
//	m_Rect.ToPosition(&A,&C);
//
//	POINT ptCursor;
//	::GetCursorPos( &ptCursor );
//
//	D3DXVECTOR2 Pos;
//	Pos.x = (float)ptCursor.x;
//	Pos.y = (float)ptCursor.y;
//
//	BOOL MsgOnMe = FALSE;
//	if (m_Rect.IsPositionInRect(Pos))
//	{
//		if (message==WM_LBUTTONDOWN)
//		{
//			m_bFocused = TRUE;
//		}
//
//		if (m_TitleRect.IsPositionInRect(Pos))
//		{	
//			HCURSOR hCursorMove = LoadCursor(NULL,IDC_SIZEALL);
//			::SetCursor(hCursorMove);
//			if (message==WM_LBUTTONDOWN)
//			{
//				m_bMoving = TRUE;
//				m_MousePos = Pos - A;	
//			}
//		}
//		
//		if(m_bEnbaleScroll)
//		{
//			FRECT RectScroll(m_Rect.Left+m_Rect.Width*0.8f,m_Rect.Top,m_Rect.Width*0.2f,m_Rect.Height);
//			if(RectScroll.IsPositionInRect(Pos))
//			{
//				HCURSOR hCursorMove = LoadCursor(NULL,IDC_SIZEALL);
//				::SetCursor(hCursorMove);
//				if (message==WM_LBUTTONDOWN)
//				{
//					m_bScrolling = TRUE;
//					m_MousePos = Pos - A;	
//				}
//			}
//		}
//
//		MsgOnMe = TRUE;
//	}
//	else
//	{
//		if (message==WM_LBUTTONDOWN)
//		{
//			m_bFocused = FALSE;
//		}
//	}
//
//	if (message==WM_MOUSEMOVE)
//	{
//		int fwKeys = wParam;
//		if ((m_bMoving)&&(fwKeys&MK_LBUTTON))
//		{
//			D3DXVECTOR2 PosLeftTop = Pos - m_MousePos;
//			if(m_bAutoResize)
//			{
//				D3DVIEWPORT9 ViewPort;
//				g_pd3dDevice->GetViewport(&ViewPort);
//				m_RelativeRect.Left  = (PosLeftTop.x - ViewPort.X)/ViewPort.Width;
//				m_RelativeRect.Top   = (PosLeftTop.y - ViewPort.Y)/ViewPort.Height;
//			}
//			else
//			{
//				m_Rect.Left  = PosLeftTop.x;
//				m_Rect.Top   = PosLeftTop.y;
//			}
//
//			ComputeRect();
//			MsgOnMe = TRUE;
//		}
//		if(m_bEnbaleScroll)
//		{
//			if ((m_bScrolling)&&(fwKeys&MK_LBUTTON))
//			{
//				D3DXVECTOR2 PosLeftTop = Pos - m_MousePos;
//				D3DVIEWPORT9 ViewPort;
//				g_pd3dDevice->GetViewport(&ViewPort);
//				//m_RelativeRect.Left  = (PosLeftTop.x - ViewPort.X)/ViewPort.Width;
//				m_RelativeRect.Top   = (PosLeftTop.y - ViewPort.Y)/ViewPort.Height;
//				ComputeRect();
//				MsgOnMe = TRUE;
//			}
//		}
//
//	}
//	else if (message==WM_LBUTTONUP)
//	{
//		m_bScrolling = FALSE;
//		m_bMoving = FALSE;
//		HCURSOR hCursorStd = LoadCursor(NULL,IDC_ARROW);
//		::SetCursor(hCursorStd);
//	}
//
//	if(MsgOnMe)
//		return MSG_PROCESSED;
//	else
//		return S_OK;
//}
//
//HRESULT KModelUIBase::SetParentWnd(KModelUIBase *lpParentWnd)
//{
//	m_lpParentWnd = lpParentWnd;
//	if (m_lpParentWnd)
//	{
//		m_fZ = m_lpParentWnd->m_fZ - 0.0001f;
//		if(m_fZ<=0)
//			m_fZ =0;
//		m_RelativeParentRect = m_RelativeRect - m_lpParentWnd->m_RelativeRect; 
//		m_ParentRect = m_Rect - m_lpParentWnd->m_Rect; 
//	}
//	return S_OK;
//}
//
//HRESULT KModelUIBase::SetTitleCaption(LPSTR pText)
//{
//	m_TitleCaption = pText;
//	return S_OK;
//}
//
//int KModelUIBase::Init(IIniFile* pIniFile, const char* pSection)
//{
//	if(KWndWindow::Init(pIniFile,pSection))
//	{
//		m_Rect.Left = (float)KWndWindow::m_Left;
//		m_Rect.Top = (float)KWndWindow::m_Top;
//		m_Rect.Width = (float)KWndWindow::m_Width;
//		m_Rect.Height = (float)KWndWindow::m_Height;
//		
//		TCHAR Name[256];
//		pIniFile->GetString(pSection,"AssignString","Title",Name,256);
//
//		SetTitleCaption(Name);
//		
//		m_bAutoResize = FALSE;
//		ComputeRect();
//
//		Wnd_AddWindow(this);
//	}
//	return TRUE;
//}
//
////--------------------------------------------------------------------------
////	功能：窗体绘制
////--------------------------------------------------------------------------
//void KModelUIBase::PaintWindow()
//{
//	KWndWindow::PaintWindow();
//	Render();
//}
//
////--------------------------------------------------------------------------
////	功能：窗口函数（处理消息）
////--------------------------------------------------------------------------
//int KModelUIBase::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
//{
//	ProcessMessage(uMsg,uParam,nParam);
//
//	//KWndWindow::WndProc(uMsg,uParam,nParam);
//	//m_bFocused = KWndWindow::m_bMoving;
//
//	return TRUE;
//}

};
