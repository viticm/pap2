#include "stdafx.h"
#include ".\meshcolorpick.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace ModelTools
{
//KModelToolColorPick::KModelToolColorPick(void)
//{
//	m_bHide = FALSE;
//	m_bMoving = FALSE;
//	m_MousePos = D3DXVECTOR2(0,0);
//	m_lpScene = NULL;
//}
//
//KModelToolColorPick::~KModelToolColorPick(void)
//{
//
//}
//
//HRESULT KModelToolColorPick::LoadMesh(LPSTR pFileName)
//{
//	m_CurColor = g_cGraphicsTool.Color(1,1,0,0);
//	m_LastColor = g_cGraphicsTool.Color(1,1,1,1);
//	m_ObjectColor = g_cGraphicsTool.Color(1,1,0,0);
//
//	TCHAR Name[256];
//
//	ModelUI::FRECT Rect(0.30f,0.3f,0.40f,0.40f);
//	KModelUIBase::Create(&Rect,"",0,0);
//	KModelUIBase::SetTitleCaption("Ñ¡ÔñÑÕÉ«");
//
//	//m_CmdTitle.Create(0.30f,0.3f,0.40f,0.03f,0,0,"",1);
//
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,"Textures\\Close.tga");
//	m_CmdClose.Create(0.67f,0.3f,0.03f,0.03f,0,0,Name,1);
//	g_cGraphicsTool.LoadTexture4Command(&m_CmdClose);
//
//	m_CmdColor.Create(0.31f,0.34f,0.24f,0.34f,0,0,"",1);
//
//	m_CmdCurColor. Create(0.62f,0.34f,0.07f,0.06f,0,0,"",1);
//	m_CmdLastColor.Create(0.62f,0.41f,0.07f,0.06f,0,0,"",1);
//
//	float height = 0.34f/6.0f;
//	for(int i=0;i<6;i++)
//	{
//		m_CmdRainBow[i].Create(0.57f,0.34f+i*height,0.03f,height,0,0,"",1);
//	}
//
//	return S_OK;
//}
//
//HRESULT KModelToolColorPick::Render(void)
//{
//	if (m_bHide)
//		return S_OK;
//
//	KModelUIBase::Render();
//	g_cGraphicsTool.DrawCommand(&m_CmdClose);
//
//	D3DCOLORVALUE A,B,C,D;
//	A.r=1;A.g=1;A.b=1;A.a=1;
//	B.r=1;B.g=0;B.b=0;B.a=1;
//	C.r=0;C.g=0;C.b=0;C.a=1;
//	D.r=0;D.g=0;D.b=0;D.a=1;
//
//	g_cGraphicsTool.DrawCommand(&m_CmdColor,&A,&m_ObjectColor,&C,&D);
//
//	g_cGraphicsTool.DrawCommand(&m_CmdCurColor,&m_CurColor);
//	g_cGraphicsTool.DrawCommand(&m_CmdLastColor,&m_LastColor);
//
//	//////////////////////////////////////////////////////////////////////////
//	//draw the fuck color rainbow
//	A = g_cGraphicsTool.Color(1,1,0,0);
//	C = g_cGraphicsTool.Color(1,1,0,1);
//	g_cGraphicsTool.DrawCommand(&m_CmdRainBow[0],&A,&A,&C,&C);
//	A = g_cGraphicsTool.Color(1,1,0,1);
//	C = g_cGraphicsTool.Color(1,0,0,1);
//	g_cGraphicsTool.DrawCommand(&m_CmdRainBow[1],&A,&A,&C,&C);
//	A = g_cGraphicsTool.Color(1,0,0,1);
//	C = g_cGraphicsTool.Color(1,0,1,1);
//	g_cGraphicsTool.DrawCommand(&m_CmdRainBow[2],&A,&A,&C,&C);
//	A = g_cGraphicsTool.Color(1,0,1,1);
//	C = g_cGraphicsTool.Color(1,0,1,0);
//	g_cGraphicsTool.DrawCommand(&m_CmdRainBow[3],&A,&A,&C,&C);
//	A = g_cGraphicsTool.Color(1,0,1,0);
//	C = g_cGraphicsTool.Color(1,1,1,0);
//	g_cGraphicsTool.DrawCommand(&m_CmdRainBow[4],&A,&A,&C,&C);
//	A = g_cGraphicsTool.Color(1,1,1,0);
//	C = g_cGraphicsTool.Color(1,1,0,0);
//	g_cGraphicsTool.DrawCommand(&m_CmdRainBow[5],&A,&A,&C,&C);
//
//	{
//		D3DXVECTOR2 PA,PC;
//		m_Rect.ToPosition(&PA,&PC);
//		g_cGraphicsTool.DrawScreenLine(&(m_PosRainBowA+PA),&(m_PosRainBowB+PA),0,0xFFFFFFFF,0xFFFFFFFF);
//
//		D3DXVECTOR2 Left(5,0);
//		D3DXVECTOR2 UP(0,5);
//		D3DXVECTOR2 ColorPos = m_PosColor + PA;
//
//		g_cGraphicsTool.DrawScreenLine(&(ColorPos-Left),&(ColorPos+Left),0,0xFFFFFFFF,0xFFFFFFFF);
//		g_cGraphicsTool.DrawScreenLine(&(ColorPos-UP),&(ColorPos+UP),0,0xFFFFFFFF,0xFFFFFFFF);
//	}
//
//	return S_OK;
//}
//HRESULT KModelToolColorPick::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	if (m_bHide)
//		return S_OK;
//	KSceneModelEditor* pMeshScene = (KSceneModelEditor*) m_lpScene;
//
//	POINT ptCursor;
//	::GetCursorPos( &ptCursor );
//
//	D3DXVECTOR2 Pos;
//	Pos.x = (float)ptCursor.x;
//	Pos.y = (float)ptCursor.y;
//
//	D3DXVECTOR2 PosLeftTop,PosC;
//	D3DVIEWPORT9 ViewPort;
//	g_pd3dDevice->GetViewport(&ViewPort);
//
//	m_Rect.ToPosition(&PosLeftTop,&PosC);
//
//	if (g_cGraphicsTool.IsPositionInCommand(&Pos,&m_CmdClose))
//	{
//		m_CmdClose.State = 1;
//		if (message==WM_LBUTTONDOWN)
//		{
//			m_bHide = TRUE;
//		}
//		return S_OK;
//	}
//	else
//	{
//		m_CmdClose.State = 0;
//	}
//
//	if (((message==WM_MOUSEMOVE)&&(wParam&MK_LBUTTON))||(message==WM_LBUTTONDOWN))
//	{
//		D3DCOLORVALUE Color = m_CurColor;
//		if(SUCCEEDED(GetColor(Pos,Color)))
//		{
//			m_CurColor = Color;
//			pMeshScene->OnChangeMaterialColor();
//		}
//	}
//
//	//////////////////////////////////////////////////////////////////////////
//	//get rainbow color
//	if (((message==WM_MOUSEMOVE)&&(wParam&MK_LBUTTON))||(message==WM_LBUTTONDOWN))
//	{
//		D3DCOLORVALUE Color = m_ObjectColor;
//		if(SUCCEEDED(GetRainBowColor(Pos,Color)))
//		{
//			m_ObjectColor = Color;
//			GetColor(PosLeftTop+m_PosColor,m_CurColor);
//			pMeshScene->OnChangeMaterialColor();
//		}
//	}
//
//
//	return KModelUIBase::ProcessMessage(message,wParam,lParam);
//}
//
//HRESULT KModelToolColorPick::SetMeshEditScene(PVOID pScene)
//{
//	m_lpScene = pScene;
//	return E_NOTIMPL;
//}
//HRESULT KModelToolColorPick::ComputeRect(void)
//{
//	KModelUIBase::ComputeRect();
//
//	m_CmdClose.X = m_RelativeRect.Left+0.37f;
//	m_CmdClose.Y = m_RelativeRect.Top;
//
//	m_CmdColor.X = m_RelativeRect.Left+0.01f;
//	m_CmdColor.Y = m_RelativeRect.Top+0.04f;
//
//	m_CmdCurColor.X = m_RelativeRect.Left+0.32f;
//	m_CmdCurColor.Y = m_RelativeRect.Top+0.04f;
//
//	m_CmdLastColor.X = m_RelativeRect.Left+0.32f;
//	m_CmdLastColor.Y = m_RelativeRect.Top+0.11f;
//
//
//
//	float height = 0.34f/6.0f;
//	for(int i=0;i<6;i++)
//	{
//		m_CmdRainBow[i].X = m_RelativeRect.Left+0.27f;
//		m_CmdRainBow[i].Y = m_RelativeRect.Top+0.04f+i*height;
//	}
//	return E_NOTIMPL;
//}
//
//HRESULT KModelToolColorPick::GetColor(D3DXVECTOR2 Pos,D3DCOLORVALUE& Color)
//{
//	D3DVIEWPORT9 ViewPort;
//	g_pd3dDevice->GetViewport(&ViewPort);
//	D3DXVECTOR2 A,C;
//	m_Rect.ToPosition(&A,&C);
//
//	if (g_cGraphicsTool.IsPositionInCommand(&Pos,&m_CmdColor))
//	{
//		m_CmdColor.State = 1;
//		D3DXVECTOR2 ColorVec;
//		D3DXVECTOR2 ColorStart,ColorSize;
//
//		ColorStart.x = ViewPort.X + ViewPort.Width  * m_CmdColor.X;
//		ColorStart.y = ViewPort.Y + ViewPort.Height * m_CmdColor.Y;
//
//		ColorSize.x = ViewPort.Width  * m_CmdColor.XS;
//		ColorSize.y = ViewPort.Height * m_CmdColor.YS;
//		ColorVec = Pos - ColorStart;
//		ColorVec.x /= ColorSize.x;
//		ColorVec.y /= ColorSize.y;
//
//		D3DXVECTOR3 ColorLeft,ColorRight;
//		ColorLeft.x = 1-ColorVec.y * 1.0f;
//		ColorLeft.y = 1-ColorVec.y * 1.0f;
//		ColorLeft.z = 1-ColorVec.y * 1.0f;
//
//		ColorRight.x = (1-ColorVec.y) * m_ObjectColor.r;
//		ColorRight.y = (1-ColorVec.y) * m_ObjectColor.g;
//		ColorRight.z = (1-ColorVec.y) * m_ObjectColor.b;
//
//		Color.r = ColorLeft.x*(1-ColorVec.x)+ColorRight.x*(ColorVec.x);
//		Color.g = ColorLeft.y*(1-ColorVec.x)+ColorRight.y*(ColorVec.x);
//		Color.b = ColorLeft.z*(1-ColorVec.x)+ColorRight.z*(ColorVec.x);
//
//		m_PosColor = Pos;
//		m_PosColor -= A;
//		return S_OK;
//	}
//	else
//		return E_FAIL;
//}
//HRESULT KModelToolColorPick::GetRainBowColor(D3DXVECTOR2 Pos,D3DCOLORVALUE& Color)
//{
//	D3DVIEWPORT9 ViewPort;
//	g_pd3dDevice->GetViewport(&ViewPort);
//	BOOL bInRainBow = FALSE;
//	D3DXVECTOR2 A,C;
//	m_Rect.ToPosition(&A,&C);
//	if (g_cGraphicsTool.IsPositionInCommand(&Pos,&m_CmdRainBow[0]))
//	{
//		D3DXVECTOR2 ColorVec;
//		D3DXVECTOR2 ColorStart,ColorSize;
//
//		ColorStart.x = ViewPort.X + ViewPort.Width  * m_CmdRainBow[0].X;
//		ColorStart.y = ViewPort.Y + ViewPort.Height * m_CmdRainBow[0].Y;
//
//		ColorSize.x = ViewPort.Width  * m_CmdRainBow[0].XS;
//		ColorSize.y = ViewPort.Height * m_CmdRainBow[0].YS;
//		ColorVec = Pos - ColorStart;
//		ColorVec.x /= ColorSize.x;
//		ColorVec.y /= ColorSize.y;
//
//		D3DXVECTOR3 Up(1,0,0);
//		D3DXVECTOR3 Dn(1,0,1);
//
//		Color.r = Up.x*(1-ColorVec.y)+Dn.x*ColorVec.y;
//		Color.g = Up.y*(1-ColorVec.y)+Dn.y*ColorVec.y;
//		Color.b = Up.z*(1-ColorVec.y)+Dn.z*ColorVec.y;
//
//		m_PosRainBowA.x = ColorStart.x - 10;
//		m_PosRainBowA.y = Pos.y;
//		m_PosRainBowB.x = ColorStart.x + ColorSize.x + 10;
//		m_PosRainBowB.y = Pos.y;
//		m_PosRainBowA -= A;
//		m_PosRainBowB -= A;
//
//		bInRainBow = TRUE;
//	}
//	else if (g_cGraphicsTool.IsPositionInCommand(&Pos,&m_CmdRainBow[1]))
//	{
//		D3DXVECTOR2 ColorVec;
//		D3DXVECTOR2 ColorStart,ColorSize;
//
//		ColorStart.x = ViewPort.X + ViewPort.Width  * m_CmdRainBow[1].X;
//		ColorStart.y = ViewPort.Y + ViewPort.Height * m_CmdRainBow[1].Y;
//
//		ColorSize.x = ViewPort.Width  * m_CmdRainBow[1].XS;
//		ColorSize.y = ViewPort.Height * m_CmdRainBow[1].YS;
//		ColorVec = Pos - ColorStart;
//		ColorVec.x /= ColorSize.x;
//		ColorVec.y /= ColorSize.y;
//
//		D3DXVECTOR3 Up(1,0,1);
//		D3DXVECTOR3 Dn(0,0,1);
//
//		Color.r = Up.x*(1-ColorVec.y)+Dn.x*ColorVec.y;
//		Color.g = Up.y*(1-ColorVec.y)+Dn.y*ColorVec.y;
//		Color.b = Up.z*(1-ColorVec.y)+Dn.z*ColorVec.y;
//		m_PosRainBowA.x = ColorStart.x - 10;
//		m_PosRainBowA.y = Pos.y;
//		m_PosRainBowB.x = ColorStart.x + ColorSize.x + 10;
//		m_PosRainBowB.y = Pos.y;
//		m_PosRainBowA -= A;
//		m_PosRainBowB -= A;
//
//		bInRainBow = TRUE;
//	}
//	else if (g_cGraphicsTool.IsPositionInCommand(&Pos,&m_CmdRainBow[2]))
//	{
//		D3DXVECTOR2 ColorVec;
//		D3DXVECTOR2 ColorStart,ColorSize;
//
//		ColorStart.x = ViewPort.X + ViewPort.Width  * m_CmdRainBow[2].X;
//		ColorStart.y = ViewPort.Y + ViewPort.Height * m_CmdRainBow[2].Y;
//
//		ColorSize.x = ViewPort.Width  * m_CmdRainBow[2].XS;
//		ColorSize.y = ViewPort.Height * m_CmdRainBow[2].YS;
//		ColorVec = Pos - ColorStart;
//		ColorVec.x /= ColorSize.x;
//		ColorVec.y /= ColorSize.y;
//
//		D3DXVECTOR3 Up(0,0,1);
//		D3DXVECTOR3 Dn(0,1,1);
//
//		Color.r = Up.x*(1-ColorVec.y)+Dn.x*ColorVec.y;
//		Color.g = Up.y*(1-ColorVec.y)+Dn.y*ColorVec.y;
//		Color.b = Up.z*(1-ColorVec.y)+Dn.z*ColorVec.y;
//		m_PosRainBowA.x = ColorStart.x - 10;
//		m_PosRainBowA.y = Pos.y;
//		m_PosRainBowB.x = ColorStart.x + ColorSize.x + 10;
//		m_PosRainBowB.y = Pos.y;
//		m_PosRainBowA -= A;
//		m_PosRainBowB -= A;
//
//		bInRainBow = TRUE;
//	}
//	else if (g_cGraphicsTool.IsPositionInCommand(&Pos,&m_CmdRainBow[3]))
//	{
//		D3DXVECTOR2 ColorVec;
//		D3DXVECTOR2 ColorStart,ColorSize;
//
//		ColorStart.x = ViewPort.X + ViewPort.Width  * m_CmdRainBow[3].X;
//		ColorStart.y = ViewPort.Y + ViewPort.Height * m_CmdRainBow[3].Y;
//
//		ColorSize.x = ViewPort.Width  * m_CmdRainBow[3].XS;
//		ColorSize.y = ViewPort.Height * m_CmdRainBow[3].YS;
//		ColorVec = Pos - ColorStart;
//		ColorVec.x /= ColorSize.x;
//		ColorVec.y /= ColorSize.y;
//
//		D3DXVECTOR3 Up(0,1,1);
//		D3DXVECTOR3 Dn(0,1,0);
//
//		Color.r = Up.x*(1-ColorVec.y)+Dn.x*ColorVec.y;
//		Color.g = Up.y*(1-ColorVec.y)+Dn.y*ColorVec.y;
//		Color.b = Up.z*(1-ColorVec.y)+Dn.z*ColorVec.y;
//		m_PosRainBowA.x = ColorStart.x - 10;
//		m_PosRainBowA.y = Pos.y;
//		m_PosRainBowB.x = ColorStart.x + ColorSize.x + 10;
//		m_PosRainBowB.y = Pos.y;
//		m_PosRainBowA -= A;
//		m_PosRainBowB -= A;
//
//		bInRainBow = TRUE;
//	}
//	else if (g_cGraphicsTool.IsPositionInCommand(&Pos,&m_CmdRainBow[4]))
//	{
//		D3DXVECTOR2 ColorVec;
//		D3DXVECTOR2 ColorStart,ColorSize;
//
//		ColorStart.x = ViewPort.X + ViewPort.Width  * m_CmdRainBow[4].X;
//		ColorStart.y = ViewPort.Y + ViewPort.Height * m_CmdRainBow[4].Y;
//
//		ColorSize.x = ViewPort.Width  * m_CmdRainBow[4].XS;
//		ColorSize.y = ViewPort.Height * m_CmdRainBow[4].YS;
//		ColorVec = Pos - ColorStart;
//		ColorVec.x /= ColorSize.x;
//		ColorVec.y /= ColorSize.y;
//
//		D3DXVECTOR3 Up(0,1,0);
//		D3DXVECTOR3 Dn(1,1,0);
//
//		Color.r = Up.x*(1-ColorVec.y)+Dn.x*ColorVec.y;
//		Color.g = Up.y*(1-ColorVec.y)+Dn.y*ColorVec.y;
//		Color.b = Up.z*(1-ColorVec.y)+Dn.z*ColorVec.y;
//		m_PosRainBowA.x = ColorStart.x - 10;
//		m_PosRainBowA.y = Pos.y;
//		m_PosRainBowB.x = ColorStart.x + ColorSize.x + 10;
//		m_PosRainBowB.y = Pos.y;
//		m_PosRainBowA -= A;
//		m_PosRainBowB -= A;
//
//		bInRainBow = TRUE;
//	}
//	else if (g_cGraphicsTool.IsPositionInCommand(&Pos,&m_CmdRainBow[5]))
//	{
//		D3DXVECTOR2 ColorVec;
//		D3DXVECTOR2 ColorStart,ColorSize;
//
//		ColorStart.x = ViewPort.X + ViewPort.Width  * m_CmdRainBow[5].X;
//		ColorStart.y = ViewPort.Y + ViewPort.Height * m_CmdRainBow[5].Y;
//
//		ColorSize.x = ViewPort.Width  * m_CmdRainBow[5].XS;
//		ColorSize.y = ViewPort.Height * m_CmdRainBow[5].YS;
//		ColorVec = Pos - ColorStart;
//		ColorVec.x /= ColorSize.x;
//		ColorVec.y /= ColorSize.y;
//
//		D3DXVECTOR3 Up(1,1,0);
//		D3DXVECTOR3 Dn(1,0,0);
//
//		Color.r = Up.x*(1-ColorVec.y)+Dn.x*ColorVec.y;
//		Color.g = Up.y*(1-ColorVec.y)+Dn.y*ColorVec.y;
//		Color.b = Up.z*(1-ColorVec.y)+Dn.z*ColorVec.y;
//		m_PosRainBowA.x = ColorStart.x - 10;
//		m_PosRainBowA.y = Pos.y;
//		m_PosRainBowB.x = ColorStart.x + ColorSize.x + 10;
//		m_PosRainBowB.y = Pos.y;
//		m_PosRainBowA -= A;
//		m_PosRainBowB -= A;
//
//		bInRainBow = TRUE;
//	}
//	if(bInRainBow)
//		return S_OK;
//	else
//		return E_FAIL;
//}
};
