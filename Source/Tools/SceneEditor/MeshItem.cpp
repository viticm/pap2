#include "stdafx.h"
#include ".\meshitem.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace ModelUI
{

//KModelUIItem::KModelUIItem(void)
//{
//	m_dwValue = 0;
//
//	m_dwStrLength = 0;
//
//	m_dwState = 0;
//
//	m_dwNumChar  = 0;
//	m_dwStyle    = ITEMSTYLE_TEXT;
//	m_IconTextID = 0;
//
//	wsprintf(m_szString,"");
//	wsprintf(m_lpShowStr,"");
//
//	m_dwLastClickTime = 0;
//	m_bSelected = FALSE;
//	m_bDrawBorder = TRUE;
//}
//
//KModelUIItem::~KModelUIItem(void)
//{
//}
//
//HRESULT KModelUIItem::Create(LPFRECT pRect,LPSTR StyleFile,BOOL State,DWORD Style)
//{
//	KModelUIBase::Create(pRect,StyleFile,State,Style);
//
//	m_bShowTitle = FALSE;
//
//	m_dwNumChar = (DWORD)(pRect->Width/pRect->Height*4);
//	if (m_dwNumChar<5)  m_dwNumChar = 5;
//	if (m_dwNumChar>30) m_dwNumChar = 30;
//
//	//m_lpShowStr = new TCHAR[m_dwNumChar+1];
//	for(DWORD i=0;i<m_dwNumChar;i++)
//	{
//		m_lpShowStr[i] = ' ';
//	}
//	m_lpShowStr[i] = '\0';
//	RefreshText();
//
//	SAFE_DELETE_ARRAY(m_lpMaterial);
//	m_dNumMaterial = 1;
//	m_lpMaterial = new MATERIAL[m_dNumMaterial];
//	ZeroMemory(m_lpMaterial,sizeof(MATERIAL)*m_dNumMaterial);
//
//	DWORD Def_Option = MATERIAL_OPTION_ZBUFFER_TRUE|
//		MATERIAL_OPTION_FILL_SOLID|
//		MATERIAL_OPTION_SHADE_GOURAUD|
//		MATERIAL_OPTION_CULL_CW|MATERIAL_OPTION_LIGHTING|
//		MATERIAL_OPTION_SPECULARENABLE;
//
//	for( i=0;i<m_dNumMaterial;i++)
//	{
//		m_lpMaterial[i].m_sMaterial9.Diffuse.r = 0.7f ;
//		m_lpMaterial[i].m_sMaterial9.Diffuse.g = 0.7f ;
//		m_lpMaterial[i].m_sMaterial9.Diffuse.b = 0.7f ;
//		m_lpMaterial[i].m_sMaterial9.Diffuse.a = 1.0f ;
//		m_lpMaterial[i].m_sMaterial9.Ambient = m_lpMaterial[i].m_sMaterial9.Diffuse;
//		m_lpMaterial[i].m_sMaterial9.Specular = m_lpMaterial[i].m_sMaterial9.Diffuse;
//		m_lpMaterial[i].m_sMaterial9.Power = 15;
//		m_lpMaterial[i].m_dOption = Def_Option;
//	}
//	return E_NOTIMPL;
//}
//
//HRESULT KModelUIItem::CreateAbsolute(LPFRECT pRect,LPSTR StyleFile,BOOL State,DWORD Style)
//{
//	KModelUIBase::CreateAbsolute(pRect,StyleFile,State,Style);
//
//	m_bShowTitle = FALSE;
//
//	m_dwNumChar = (DWORD)(pRect->Width/pRect->Height*4);
//	if (m_dwNumChar<5)  m_dwNumChar = 5;
//	if (m_dwNumChar>30) m_dwNumChar = 30;
//
//	//m_lpShowStr = new TCHAR[m_dwNumChar+1];
//	for(DWORD i=0;i<m_dwNumChar;i++)
//	{
//		m_lpShowStr[i] = ' ';
//	}
//	m_lpShowStr[i] = '\0';
//	RefreshText();
//
//	SAFE_DELETE_ARRAY(m_lpMaterial);
//	m_dNumMaterial = 1;
//	m_lpMaterial = new MATERIAL[m_dNumMaterial];
//	ZeroMemory(m_lpMaterial,sizeof(MATERIAL)*m_dNumMaterial);
//
//	DWORD Def_Option = MATERIAL_OPTION_ZBUFFER_TRUE|
//		MATERIAL_OPTION_FILL_SOLID|
//		MATERIAL_OPTION_SHADE_GOURAUD|
//		MATERIAL_OPTION_CULL_CW|MATERIAL_OPTION_LIGHTING|
//		MATERIAL_OPTION_SPECULARENABLE;
//
//	for( i=0;i<m_dNumMaterial;i++)
//	{
//		m_lpMaterial[i].m_sMaterial9.Diffuse.r = 0.7f ;
//		m_lpMaterial[i].m_sMaterial9.Diffuse.g = 0.7f ;
//		m_lpMaterial[i].m_sMaterial9.Diffuse.b = 0.7f ;
//		m_lpMaterial[i].m_sMaterial9.Diffuse.a = 1.0f ;
//		m_lpMaterial[i].m_sMaterial9.Ambient = m_lpMaterial[i].m_sMaterial9.Diffuse;
//		m_lpMaterial[i].m_sMaterial9.Specular = m_lpMaterial[i].m_sMaterial9.Diffuse;
//		m_lpMaterial[i].m_sMaterial9.Power = 15;
//		m_lpMaterial[i].m_dOption = Def_Option;
//	}
//	return E_NOTIMPL;
//}
//HRESULT KModelUIItem::Render(void)
//{
//	if (m_bSelected)
//	{
//		m_BorderColor = 0xFFFF0000;
//		m_BackColor   = 0xFFFFFFFF;
//	}
//	else
//	{
//		m_BorderColor = 0xFF101010;
//		m_BackColor   = 0xFFAAAAAA;
//	}
//
//	if(m_bDrawBorder)
//	{
//		KModelUIBase::Render();
//	}
//	else
//	{
//		KModelUIBase::ComputeRect();
//	}
//
//	D3DXVECTOR2 A,C;
//	m_ClientRect.ToPosition(&A,&C);
//
//
//	switch(m_dwStyle)
//	{
//	case ITEMSTYLE_TEXT:
//		g_cGraphicsTool.DrawTextInRect(&m_ClientRect,m_fZ,0xFF000000,12,"ËÎÌו",m_szString,0);
//		break;
//	case ITEMSTYLE_IMAGE:
//		g_cGraphicsTool.DrawScreenRect(&A,&C,m_fZ,0xFFFFFFFF,m_lpMaterial[0].m_dTextureIDs[0]);
//		break;
//	case ITEMSTYLE_TEXTIMAGE:
//		{
//			float L = m_Rect.Height;
//			D3DXVECTOR2 B = C;
//			B.x = A.x + L;
//			g_cGraphicsTool.DrawScreenRect(&A,&B,m_fZ,0xFFFFFFFF,m_lpMaterial[0].m_dTextureIDs[0]);
//
//			FRECT rect(A.x+L,A.y,m_Rect.Width - L,L);
//			g_cGraphicsTool.DrawTextInRect(&rect,m_fZ,0xFF000000,12,"ËÎÌו",m_lpShowStr,0);
//		}
//		break;
//	case ITEMSTYLE_MATERIAL:
//		{
//			if ((m_bSelected)||(m_dwState==1))
//				g_cGraphicsTool.DrawScreenRect(&A,&C,m_fZ,0xFFFFFFFF,m_lpMaterial[0].m_dTextureIDs[0]);
//			else
//				g_cGraphicsTool.DrawScreenRect(&A,&C,m_fZ,0xFFA0A0A0,m_lpMaterial[0].m_dTextureIDs[0]);
//		}
//		break;
//	case ITEMSTYLE_COLOR:
//		{
//			D3DCOLOR Color;
//			Color = ColorValue(m_Color);
//
//			g_cGraphicsTool.DrawScreenRectD(&A,&C,m_fZ,Color,Color,Color,Color);
//			g_cGraphicsTool.DrawTextInRect(&m_ClientRect,m_fZ,0xFF000000,12,"ËÎÌו",m_lpShowStr,0);
//		}
//		break;
//	}
//	return E_NOTIMPL;
//}
//
//HRESULT KModelUIItem::SetText(LPSTR pSrcText)
//{
//	wsprintf(m_szString,"%s",pSrcText);
//	m_dwStrLength = strlen(m_szString);
//	RefreshText();
//	return E_NOTIMPL;
//}
//HRESULT KModelUIItem::RefreshText(void)
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
//HRESULT KModelUIItem::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
//{
//
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
//			DWORD Time = timeGetTime();
//			if (Time - m_dwLastClickTime<1000)
//			{
//				m_dwLastClickTime = Time;
//				return ITEM_DBCLICK;
//			}
//			else
//			{
//				m_dwLastClickTime = Time;
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
//
//	return ITEM_NOTING;
//}
//
//HRESULT KModelUIItem::SetIcon(LPSTR pIconFile)
//{
//	DWORD TextureID = 0;
//	g_cTextureTable.LoadFromFile(&TextureID,pIconFile);
//	m_lpMaterial[0].m_dTextureIDs[0] = TextureID;
//	return S_OK;
//}
//
//HRESULT KModelUIItem::SetMaterial(KModel::LPMATERIAL pMat)
//{
//	if (!pMat)
//		return E_FAIL;
//	m_lpMaterial[0].m_sMaterial9 = pMat->m_sMaterial9 ;
//	m_lpMaterial[0].m_dOption = pMat->m_dOption;
//	for(int i=0;i<8;i++)
//	{
//
//		m_lpMaterial[0].m_dTextureIDs[i] = pMat->m_dTextureIDs[i];
//	}
//	return S_OK;
//}
//
//HRESULT KModelUIItem::SetColor(D3DCOLORVALUE Color)
//{
//	m_Color = Color;
//	return S_OK;
//}
//
//int KModelUIItem::Init(IIniFile* pIniFile, const char* pSection)
//{
//	m_bShowTitle = FALSE;
//	KModelUIBase::Init(pIniFile,pSection);
//	pIniFile->GetBool(pSection,"Border",&m_bDrawBorder);
//
//	m_dwNumChar = (DWORD)(KWndWindow::m_Width*1.0f/KWndWindow::m_Height*5);
//	if (m_dwNumChar<5)  m_dwNumChar = 5;
//	if (m_dwNumChar>30) m_dwNumChar = 30;
//
//	for(DWORD i=0;i<m_dwNumChar;i++)
//	{
//		m_lpShowStr[i] = ' ';
//	}
//	m_lpShowStr[i] = '\0';
//	RefreshText();
//
//	SetText((LPSTR)m_TitleCaption.c_str());
//	return S_OK;
//}
};
