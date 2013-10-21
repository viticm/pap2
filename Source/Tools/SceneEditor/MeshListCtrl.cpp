// MeshListCtrl.cpp: implementation of the KModelUIListCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "MeshListCtrl.h"

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

//KModelUIListCtrl::KModelUIListCtrl()
//{
//	m_bMultiSelect = TRUE; 
//	m_fItemWidth  = 35;
//	m_fItemHeight = 35;
//	m_nNumItem_X = 0;
//	m_nNumItem_Y = 0;
//	m_nItemStart = 0;
//	m_nNumItem = 0;
//	m_nNumItemOld = 0;
//}
//
//KModelUIListCtrl::~KModelUIListCtrl()
//{
//
//}
//
//HRESULT KModelUIListCtrl::Create(LPFRECT pRect, LPSTR StyleFile, BOOL State, DWORD Style)
//{
//	KModelUIBase::Create(pRect,StyleFile,State,Style);
//	m_bShowTitle = FALSE;
//	
//	return S_OK;
//}
//
//HRESULT KModelUIListCtrl::ComputeRect()
//{
//	KModelUIBase::ComputeRect();
//	m_nNumItem_X = (int)(m_Rect.Width  / m_fItemWidth);
//	m_nNumItem_Y = (int)(m_Rect.Height / m_fItemHeight);
//	m_nNumItem   = m_nNumItem_X * m_nNumItem_Y;
//	return S_OK;
//}
//
//HRESULT KModelUIListCtrl::FrameMove()
//{
//	return S_OK;
//}
//
//HRESULT KModelUIListCtrl::Render()
//{
//	if ( m_nNumItemOld != m_nNumItem)
//		Refresh();
//
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
//	for(int i = m_nItemStart;i<(int)m_ListItems.size();i++)
//	{
//		int ShowItemIndex = i - m_nItemStart;
//		
//		KModelUIItem* pItem = m_ShowItems[ShowItemIndex];
//		
//		SAFE_RENDER(pItem);
//	}
//	return S_OK;
//}
//
//HRESULT KModelUIListCtrl::Refresh()
//{
//	int X = 0;
//	int Y = 0;
//	for(int i = m_nItemStart;i<(int)m_ListItems.size();i++)
//	{
//		int ShowItemIndex = i - m_nItemStart;
//		
//		if (ShowItemIndex>=(int)m_ShowItems.size())
//		{
//			KModelUIItem* pItem = NULL;
//			g_cMeshTable.Get1NewXMesh((LPMODEL*)(&pItem),MODELTYPE_ITEM);
//			m_ShowItems.push_back(pItem);
//		}
//		KModelUIItem* pItem = m_ShowItems[ShowItemIndex];
//		
//		if(m_bAutoResize)
//		{
//			float Item_W = m_RelativeRect.Width  / m_nNumItem_X;
//			float Item_H = m_RelativeRect.Height / m_nNumItem_Y;
//			float Item_X = m_RelativeRect.Left + X * Item_W + Item_W*0.05f;
//			float Item_Y = m_RelativeRect.Top  + Y * Item_H + Item_H*0.05f;
//
//			FRECT Rect(Item_X,Item_Y,Item_W*0.9f,Item_H*0.9f);
//			pItem->SetParentWnd(NULL);
//			pItem->ComputeRect();
//			pItem->Create(&Rect,"",0,ITEMSTYLE_MATERIAL);
//		}
//		else
//		{
//			float Item_W = m_Rect.Width  / m_nNumItem_X;
//			float Item_H = m_Rect.Height / m_nNumItem_Y;
//			float Item_X = m_Rect.Left + X * Item_W + Item_W*0.05f;
//			float Item_Y = m_Rect.Top  + Y * Item_H + Item_H*0.05f;
//
//			FRECT Rect(Item_X,Item_Y,Item_W*0.9f,Item_H*0.9f);
//			pItem->SetParentWnd(NULL);
//			pItem->ComputeRect();
//			pItem->CreateAbsolute(&Rect,"",0,ITEMSTYLE_MATERIAL);
//		}
//
//		TCHAR str[256];
//		wsprintf(str,"Item%d",ShowItemIndex);
//		pItem->SetText(str);
//		pItem->SetParentWnd(this);
//		pItem->m_bSelected = m_ListItems[i].dwState;
//
//
//		LISTITEM ListItem= m_ListItems[i];
//		KModel::LPMATERIAL pMat = (KModel::LPMATERIAL) ListItem.dwData;
//		pItem->SetMaterial(pMat);
//
//		X++;
//		if (X>=m_nNumItem_X)
//		{
//			X = 0;
//			Y ++;
//		}
//	}
//	m_nNumItemOld = m_nNumItem;
//	return S_OK;
//}
//
//HRESULT KModelUIListCtrl::AddItemMaterial(KModel::LPMATERIAL pMat)
//{
//	LISTITEM NewItem;
//	NewItem.dwData = (DWORD) pMat;
//	m_ListItems.push_back(NewItem);
//	Refresh();
//	return S_OK;
//}
//
//HRESULT KModelUIListCtrl::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	for(int i = m_nItemStart;i<(int)m_ListItems.size();i++)
//	{
//		int ShowItemIndex = i - m_nItemStart;
//		KModelUIItem* pItem = m_ShowItems[ShowItemIndex];
//		DWORD MSG = pItem->ProcessMessage(message,wParam,lParam);
//		if ((MSG == ITEM_CLICK)||(MSG == ITEM_DBCLICK))
//		{
//			if(!m_bMultiSelect)
//			{
//				SelectNone();
//			}
//			pItem->m_bSelected = !pItem->m_bSelected;
//			m_ListItems[i].dwState = pItem->m_bSelected;
//			return ITEM_CLICK;
//		}
//	}
//	return S_OK;
//}
//
//HRESULT KModelUIListCtrl::Clear()
//{
//	m_ListItems.clear();
//	m_nItemStart = 0;
//	return S_OK;
//}
//
//HRESULT KModelUIListCtrl::SelectAll()
//{
//	for(int i = m_nItemStart;i<(int)m_ListItems.size();i++)
//	{
//		m_ListItems[i].dwState = TRUE;
//	}
//	Refresh();
//	return S_OK;
//}
//
//HRESULT KModelUIListCtrl::SelectNone()
//{
//	for(int i = m_nItemStart;i<(int)m_ListItems.size();i++)
//	{
//		m_ListItems[i].dwState = FALSE;
//	}
//	Refresh();
//	return S_OK;
//}
//
//HRESULT KModelUIListCtrl::SelectInvert()
//{
//	for(int i = m_nItemStart;i<(int)m_ListItems.size();i++)
//	{
//		m_ListItems[i].dwState = !m_ListItems[i].dwState;
//	}
//	Refresh();
//	return S_OK;
//}
//
//int KModelUIListCtrl::Init(IIniFile* pIniFile, const char* pSection)
//{
//	m_bShowTitle = FALSE;
//	int nI_Width = 0;
//	int nI_Heigth = 0;
//	pIniFile->GetInteger(pSection,"ItemWidth",16 ,&nI_Width);
//	pIniFile->GetInteger(pSection,"ItemHeight",16,&nI_Heigth);
//
//	m_fItemWidth  = (float)nI_Width;
//	m_fItemHeight = (float)nI_Heigth;
//
//	KModelUIBase::Init(pIniFile,pSection);
//
//	return S_OK;}
};
