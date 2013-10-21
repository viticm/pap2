#include "stdafx.h"
#include ".\mesheditctrl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace ModelUI
{
//
//KModelUIEditCtrl::KModelUIEditCtrl(void)
//{
//	m_dwValue = 0;
//
//	m_dwNumChar = 0;
//
//	m_bEnable = TRUE;
//	m_dwStyle = 0;
//	m_dwCurPos = 0;
//	wsprintf(m_szString,"");
//	m_lpShowStr = NULL;
//	m_bFocus = FALSE;
//}
//
//KModelUIEditCtrl::~KModelUIEditCtrl(void)
//{
//	SAFE_DELETE_ARRAY(m_lpShowStr);
//}
//
//HRESULT KModelUIEditCtrl::Create(LPFRECT pRect,LPSTR StyleFile,BOOL State,DWORD Style)
//{
//	KModelUIBase::Create(pRect,StyleFile,State,Style);
//	m_bEnable = State;
//	m_dwStyle = Style;
//	m_bShowTitle = FALSE;
//
//	m_dwNumChar = (DWORD)(pRect->Width/pRect->Height*4);
//	if (m_dwNumChar<5)  m_dwNumChar = 5;
//	if (m_dwNumChar>30) m_dwNumChar = 30;
//
//	m_lpShowStr = new TCHAR[m_dwNumChar+1];
//	for(DWORD i=0;i<m_dwNumChar;i++)
//	{
//		m_lpShowStr[i] = ' ';
//	}
//	m_lpShowStr[i] = '\0';
//	return E_NOTIMPL;
//}
//
//HRESULT KModelUIEditCtrl::Render(void)
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
//		g_cGraphicsTool.DrawScreenLine(&A,&B,m_fZ,0xFF444444,0xFF444444);
//		g_cGraphicsTool.DrawScreenLine(&A,&D,m_fZ,0xFF444444,0xFF444444);
//		g_cGraphicsTool.DrawScreenLine(&C,&B,m_fZ,0xFFFFFFFF,0xFFFFFFFF);
//		g_cGraphicsTool.DrawScreenLine(&C,&D,m_fZ,0xFFFFFFFF,0xFFFFFFFF);
//	}
//
//	m_BorderRect.ToPosition(&A,&C);
//	g_cGraphicsTool.DrawScreenRectD(&A,&C,m_fZ,m_BackColor,m_BackColor,m_BackColor,m_BackColor);
//
//	m_ClientRect.ToPosition(&A,&C);
//	g_cGraphicsTool.DrawTextInRect(&m_ClientRect,0.0f,0xFF000000,12,"ËÎÌו",m_lpShowStr,0);
//
//	if (m_bFocus)
//	{
//		float CurPos  = 12.0f * m_dwCurPos * 0.68f;
//		D3DXVECTOR2 CurA(A.x+CurPos,A.y +2);
//		D3DXVECTOR2 CurB(A.x+CurPos+2,C.y-2);
//		float t = sinf(timeGetTime()*0.01f);
//		if ( t > 0)
//		{
//			g_pd3dDevice->SetTexture(0,NULL);
//			g_cGraphicsTool.DrawScreenRectD(&CurA,&CurB,0,0xFF101010,0xFF101010,0xFF101010,0xFF101010);
//		}
//	}
//
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
//	A1 = A;
//	C1 = C;
//
//	float Len = R_H * 0.05f;
//	A1.x = A.x + Len;
//	C1.x = A.x + R_W - Len;
//	A1.y = A.y + Len;
//	C1.y = A.y + R_H - Len;
//	A2 = A1;
//	C2 = C1;
//
//	if (m_bFocus)
//	{
//		g_cGraphicsTool.DrawScreenRect(&A ,&C ,0,1,1,0xFFFF0000,0);
//		g_cGraphicsTool.DrawScreenRect(&A1,&C1,0,1,1,0xFFFFFFFF,0);
//	}
//	else
//	{
//		g_cGraphicsTool.DrawScreenRect(&A ,&C ,0,1,1,0xFF000000,0);
//		g_cGraphicsTool.DrawScreenRect(&A1,&C1,0,1,1,0xFFAAAAAA,0);
//	}
//
//
//	g_cGraphicsTool.DrawTextScale(&A1,&C1,
//		0xFF000000,24,"ËÎÌו",m_lpShowStr,0);
//
//	float Vox = R_W/m_dwNumChar;
//	//for(DWORD i=0;i<m_dwNumChar;i++)
//	if (m_bFocus)
//	{
//		DWORD i = m_dwCurPos;
//		{
//			A2.x = A1.x + Vox * i ;
//			C2.x = A1.x + Vox * i + Len;
//			float t = sinf(timeGetTime()*0.01f);
//			if ( t > 0)
//				g_cGraphicsTool.DrawScreenRect(&A2,&C2,0,1,1,0xFFFF0000,0);
//			else
//				g_cGraphicsTool.DrawScreenRect(&A2,&C2,0,1,1,0x00FFFFFF,0);
//		}
//	}
//
//	/*g_cGraphicsTool.DrawScreenRect(&A1,&C1,0,1,1,0xFF353535,0);
//	if (m_dwSliderState==0)
//		g_cGraphicsTool.DrawScreenRect(&A2,&C2,0,1,1,0xFFAAAAAA,m_dwSliderTextureID);
//	else
//		g_cGraphicsTool.DrawScreenRect(&A2,&C2,0,1,1,0xFFFFFFFF,m_dwSliderTextureID);
//	*/
//
//	return E_NOTIMPL;
//}
//
//HRESULT KModelUIEditCtrl::RefreshText(void)
//{
//	BOOL end = FALSE;
//	for(DWORD i=0;i<m_dwNumChar;i++)
//	{
//		if (!end)
//		{
//			char chr = m_szString[i];
//			if (chr=='\0')
//			{
//				end = TRUE;
//				m_lpShowStr[i] = ' ';
//			}
//			else
//			{
//				m_lpShowStr[i] = chr;
//			}
//		}
//		else
//		{
//			m_lpShowStr[i] = ' ';
//		}
//	}
//	m_lpShowStr[i] = '\0';
//	return E_NOTIMPL;
//}
//
//HRESULT KModelUIEditCtrl::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	if (message==WM_LBUTTONDOWN)
//	{
//		D3DXVECTOR2 A,C,A1,C1;
//		m_Rect.ToPosition(&A,&C);
//
//		POINT ptCursor;
//		::GetCursorPos( &ptCursor );
//
//		D3DXVECTOR2 Pos;
//		Pos.x = (float)ptCursor.x;
//		Pos.y = (float)ptCursor.y;
//
//		if ((Pos.x >= A.x)&&(Pos.x <= C.x)&&
//			(Pos.y >= A.y)&&(Pos.y <= C.y))
//		{
//			m_bFocus = TRUE;
//
//			A1 = A;
//			C1 = C;
//
//			float Len = m_Rect.Height * 0.05f;
//			A1.x = A.x + Len;
//			C1.x = A.x + m_Rect.Width - Len;
//			A1.y = A.y + Len;
//			C1.y = A.y + m_Rect.Height - Len;
//			float XN = (Pos.x - A1.x)/m_Rect.Width*m_dwNumChar;
//			DWORD Dest_Pos = (DWORD)(XN+0.5f);
//			if (Dest_Pos<m_dwCurPos)
//			{
//				m_dwCurPos = Dest_Pos;
//			}
//			else if (Dest_Pos>m_dwCurPos)
//			{
//				for(DWORD i=m_dwCurPos;i<Dest_Pos;i++)
//				{
//					if (m_szString[m_dwCurPos]!='\0')
//					{
//						m_dwCurPos++;
//					}
//					else
//					{
//						break;
//					}
//				}
//			}
//		}
//		else
//		{
//			m_bFocus = FALSE;
//		}
//	}
//
//	if (m_bFocus)
//	{
//		if (message==WM_CHAR)
//		{
//			TCHAR Key = (TCHAR)wParam;
//			if (Key==VK_BACK)
//			{
//				if (m_dwCurPos>0)
//				{
//					for(DWORD i = m_dwCurPos;i<255;i++)
//					{
//						TCHAR Str = m_szString[i];
//						m_szString[i-1] = Str;
//						if (Str=='\0')
//							break;
//					}
//					m_dwCurPos--;
//
//					RefreshText();
//				}
//			}
//			else if(Key==VK_RETURN)
//			{
//				return EDIT_RETURN;
//			}
//			else
//			{
//				if (m_dwCurPos<m_dwNumChar)
//				{
//					for(DWORD EndPos = m_dwCurPos;EndPos<255;EndPos++)
//					{
//						TCHAR Str = m_szString[EndPos];
//						if (Str=='\0')
//							break;
//					}
//					for(int i = EndPos ; i>=(int)m_dwCurPos;i--)
//					{
//						TCHAR Str = m_szString[i];
//						m_szString[i+1] = Str;
//					}
//
//					m_szString[m_dwCurPos] = Key;
//					m_dwCurPos++;
//
//					RefreshText();
//				}
//			}
//		}
//		else if (message==WM_KEYDOWN)
//		{
//			TCHAR Key = (TCHAR)wParam;
//			if (Key=='\n')
//			{
//				;
//			}
//			else if (Key==VK_LEFT)
//			{
//				if (m_dwCurPos>0)
//				{
//					m_dwCurPos--;
//				}
//			}
//			else if (Key==VK_RIGHT)
//			{
//				if (m_szString[m_dwCurPos]!='\0')
//				{
//					m_dwCurPos++;
//				}
//			}
//			else if (Key==VK_DELETE)
//			{
//				if (m_dwCurPos<m_dwNumChar)
//				{
//					if (m_szString[m_dwCurPos]!='\0')
//					{
//						for(DWORD i = m_dwCurPos+1;i<255;i++)
//						{
//							TCHAR Str = m_szString[i];
//							m_szString[i-1] = Str;
//							if (Str=='\0')
//								break;
//						}
//					}
//
//					RefreshText();
//				}
//			}
//			return EDIT_TEXTCHANGE;
//		}
//	}
//
//	return S_OK;
//}
//
//HRESULT KModelUIEditCtrl::SetText(LPSTR String)
//{
//	wsprintf(m_szString,"%s",String);
//	RefreshText();
//	return E_NOTIMPL;
//}
//
//int KModelUIEditCtrl::Init(IIniFile* pIniFile, const char* pSection)
//{
//	m_bShowTitle = FALSE;
//	KModelUIBase::Init(pIniFile,pSection);
//
//
//	m_dwNumChar = (DWORD)(KWndWindow::m_Width*1.0f/KWndWindow::m_Height*5);
//	if (m_dwNumChar<5)  m_dwNumChar = 5;
//	if (m_dwNumChar>30) m_dwNumChar = 30;
//
//	m_lpShowStr = new TCHAR[m_dwNumChar+1];
//	for(DWORD i=0;i<m_dwNumChar;i++)
//	{
//		m_lpShowStr[i] = ' ';
//	}
//	m_lpShowStr[i] = '\0';
//	return E_NOTIMPL;
//
//}

};
