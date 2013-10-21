// MeshButton.cpp: implementation of the KModelUIButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "MeshButton.h"

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

//KModelUIButton::KModelUIButton()
//{
//	m_TextureID = 0;
//}
//
//KModelUIButton::~KModelUIButton()
//{
//
//}
//
//HRESULT KModelUIButton::Create(LPFRECT pRect, LPSTR StyleFile, BOOL State, DWORD Style)
//{
//	KModelUIBase::Create(pRect,StyleFile,State,Style);
//
//	m_bShowTitle = FALSE;
//
//	return S_OK;
//}
//
//HRESULT KModelUIButton::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
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
//	if ((Pos.x >= A.x)&&(Pos.x <= C.x)&&
//		(Pos.y >= A.y)&&(Pos.y <= C.y))
//	{
//		if (message==WM_LBUTTONDOWN)
//		{
//			/*DWORD Time = timeGetTime();
//			if (Time - m_dwLastClickTime<1000)
//			{
//				m_dwLastClickTime = Time;
//				return ITEM_DBCLICK;
//			}
//			else*/
//			if (m_dwState!=2)
//			{
//				//m_dwLastClickTime = Time;
//				m_dwState = 2;
//				return ITEM_CLICK;
//			}
//			else
//			{
//				m_dwState = 1;
//				return ITEM_CLICK;
//			}
//		}
//		if (m_dwState==0)
//		{
//			m_dwState = 1;
//			return ITEM_MOUSEON;
//		}
//	}
//	else
//	{
//		if (m_dwState==1)
//		{
//			m_dwState = 0;
//			return ITEM_MOUSELEAVE;
//		}
//	}
//	return S_OK;
//}
//
//HRESULT KModelUIButton::Render()
//{
//	ComputeRect();
//
//	D3DXVECTOR2 A,B,C,D;
//	m_Rect.ToPosition(&A,&C);
//	B.x = C.x;B.y = A.y;
//	D.x = A.x;D.y = C.y;
//
//	g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF,0x20);
//	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE ,TRUE);
//	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
//	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
//	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
//	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
//
//	//DrawScreenRect(&A1,&C1,0.0f,0xFF888888,pCmd->TextureID);
//	/*if (m_dwState==1)
//	{
//		g_cGraphicsTool.DrawScreenRect(&A,&C,m_fZ,0xFFAAAAAA,m_TextureID);
//
//		g_cGraphicsTool.DrawScreenLine(&A,&B,m_fZ,0xFFFFFFFF,0xFFFFFFFF);
//		g_cGraphicsTool.DrawScreenLine(&A,&D,m_fZ,0xFFFFFFFF,0xFFFFFFFF);
//		g_cGraphicsTool.DrawScreenLine(&C,&B,m_fZ,0xFF444444,0xFF444444);
//		g_cGraphicsTool.DrawScreenLine(&C,&D,m_fZ,0xFF444444,0xFF444444);
//	}
//	else*/ 
//	if (m_dwState==2)
//	{
//		g_cGraphicsTool.DrawScreenRect(&A,&C,m_fZ,0xFFAAAAAA,m_TextureID);
//	
//		g_cGraphicsTool.DrawScreenLine(&A,&B,m_fZ,0xFF444444,0xFF444444);
//		g_cGraphicsTool.DrawScreenLine(&A,&D,m_fZ,0xFF444444,0xFF444444);
//		g_cGraphicsTool.DrawScreenLine(&C,&B,m_fZ,0xFFFFFFFF,0xFFFFFFFF);
//		g_cGraphicsTool.DrawScreenLine(&C,&D,m_fZ,0xFFFFFFFF,0xFFFFFFFF);
//	}
//	else
//	{
//		g_cGraphicsTool.DrawScreenRect(&A,&C,m_fZ,0xFFAAAAAA,m_TextureID);
//		
//		g_cGraphicsTool.DrawScreenLine(&A,&B,m_fZ,0xFFFFFFFF,0xFFFFFFFF);
//		g_cGraphicsTool.DrawScreenLine(&A,&D,m_fZ,0xFFFFFFFF,0xFFFFFFFF);
//		g_cGraphicsTool.DrawScreenLine(&C,&B,m_fZ,0xFF444444,0xFF444444);
//		g_cGraphicsTool.DrawScreenLine(&C,&D,m_fZ,0xFF444444,0xFF444444);
//	}
//	return S_OK;
//}
//
//HRESULT KModelUIButton::SetIconTexture(LPSTR pIcon)
//{
//	TCHAR Name[256];
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,pIcon);
//	g_cTextureTable.LoadFromFile(&m_TextureID,Name);
//	return S_OK;
//}
};
