#include "stdafx.h"
#include ".\meshlistbox.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace ModelUI
{

//KModelUIListBox::KModelUIListBox(void)
//{
//	m_dwState = 0;
//	m_dwStyle = 0;
//
//	m_dwNumShowItem = 0;
//	m_lpSlider = NULL;
//	m_dwItemStart = 0;
//
//	m_nCurSelectItem = -1;
//}
//
//KModelUIListBox::~KModelUIListBox(void)
//{
//}
//
//int KModelUIListBox::Init(IIniFile* pIniFile, const char* pSection)
//{
//	m_bShowTitle = FALSE;
//	KModelUIBase::Init(pIniFile,pSection);
//	m_bShowTitle = FALSE;
//
//	m_dwNumShowItem = (DWORD)(KWndWindow::m_Height / 16);
//	if (m_dwNumShowItem<=5) m_dwNumShowItem = 5;
//
//	Refresh();
//	return S_OK;
//}
//
//HRESULT KModelUIListBox::Create(LPFRECT pRect,LPSTR StyleFile,BOOL State,DWORD Style)
//{
//	KModelUIBase::Create(pRect,StyleFile,State,Style);
//
//	m_bShowTitle = FALSE;
//
//	m_dwStyle = Style;
//
//	m_dwNumShowItem = (DWORD)(pRect->Height / 0.03f);
//	if (m_dwNumShowItem<=5) m_dwNumShowItem = 5;
//	Refresh();
//	return S_OK;
//}
//
//HRESULT KModelUIListBox::CreateAbsolute(LPFRECT pRect,LPSTR StyleFile,BOOL State,DWORD Style)
//{
//	KModelUIBase::CreateAbsolute(pRect,StyleFile,State,Style);
//
//	m_bShowTitle = FALSE;
//
//	m_dwStyle = Style;
//
//	m_dwNumShowItem = (DWORD)(KWndWindow::m_Height / 16);
//	if (m_dwNumShowItem<=5) m_dwNumShowItem = 5;
//
//	Refresh();
//	return S_OK;
//}
//
//HRESULT KModelUIListBox::Render(void)
//{
//
//	KModelUIBase::Render();
//
//	D3DVIEWPORT9 ViewPort;
//	g_pd3dDevice->GetViewport(&ViewPort);
//
//	DWORD BordColor = 0xFF000000;
//	DWORD BackColor = 0xFFAAAAAA;
//	if (m_dwState)
//	{
//		BordColor = 0xFFFF0000;
//		BackColor = 0xFFFFFFFF;
//	}
//
//
//	for(DWORD i=0;i<m_lpItems.size();i++)
//	{
//		if (m_nCurSelectItem == i+m_dwItemStart)
//		{
//			m_lpItems[i]->m_bSelected = TRUE;
//		}
//		else
//		{
//			m_lpItems[i]->m_bSelected = FALSE;
//		}
//		if (i+m_dwItemStart<m_ItemList.size())
//			m_lpItems[i]->Render();
//	}
//
//	if ((m_lpSlider)&&(m_ItemList.size()>m_dwNumShowItem))
//	{
//		m_lpSlider->Render();
//	}
//	return E_NOTIMPL;
//}
//
//HRESULT KModelUIListBox::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	//return S_OK;
//
//	if ((m_lpSlider)&&(m_ItemList.size()>m_dwNumShowItem))
//	{
//		DWORD LastItemStart = m_dwItemStart;
//		DWORD MSG = m_lpSlider->ProcessMessage(message,wParam,lParam);
//		if(MSG==SLIDER_MOVE)
//		{
//			DWORD ItemCount = m_ItemList.size();
//			if (ItemCount<=m_dwNumShowItem)
//				ItemCount = 0;
//			else
//				ItemCount -= m_dwNumShowItem;
//
//			DWORD NewItemStart = (DWORD)(m_lpSlider->m_fPercent*0.01f*(ItemCount+1));
//			if (NewItemStart!=LastItemStart)
//			{
//				m_dwItemStart = NewItemStart;
//				Refresh();
//			}
//		}
//		if (MSG)
//			return S_OK;
//	}
//
//	for(DWORD i=0;i<m_lpItems.size();i++)
//	{
//		if (i+m_dwItemStart<m_ItemList.size())
//		{
//			DWORD Msg = m_lpItems[i]->ProcessMessage(message,wParam,lParam);
//			switch(Msg)
//			{
//			case ITEM_CLICK:
//				{
//					ClearSelection();
//					m_lpItems[i]->m_bSelected = TRUE;
//					m_nCurSelectItem = i+m_dwItemStart;
//					return LISTBOX_SELECTCHANGE;
//				}
//				break;
//			case ITEM_DBCLICK:
//				{
//					ClearSelection();
//					m_lpItems[i]->m_bSelected = TRUE;
//					m_nCurSelectItem = i+m_dwItemStart;
//					return LISTBOX_ITEMDBCLICK;
//				}
//			}
//		}
//	}
//
//	return S_OK;
//}
//
//HRESULT KModelUIListBox::Refresh(void)
//{
//	float K = 1.0f;
//	if (m_ItemList.size()>m_dwNumShowItem)
//	{
//		K = 0.9f;
//	}
//
//	for(DWORD j=0;j<m_dwNumShowItem;j++)
//	{
//		if (j>=m_lpItems.size())
//		{
//			LPMODEL pMesh = NULL;
//			g_cMeshTable.Get1NewXMesh(&pMesh,MODELTYPE_ITEM);
//			m_lpItems.push_back((KModelUIItem*)pMesh);
//		}
//		if(m_bAutoResize)
//		{
//			FRECT Rect;
//			Rect.Left   = m_RelativeRect.Left;
//			Rect.Top    = m_RelativeRect.Top + m_RelativeRect.Height/m_dwNumShowItem*j;
//			Rect.Width  = m_RelativeRect.Width * K;
//			Rect.Height = m_RelativeRect.Height/m_dwNumShowItem;
//
//			m_lpItems[j]->Create(&Rect,"",0,0);
//		}
//		else
//		{
//			FRECT Rect;
//			Rect.Left   = m_Rect.Left;
//			Rect.Top    = m_Rect.Top + m_Rect.Height/m_dwNumShowItem*j;
//			Rect.Width  = m_Rect.Width * K;
//			Rect.Height = m_Rect.Height/m_dwNumShowItem;
//
//			m_lpItems[j]->CreateAbsolute(&Rect,"",0,0);
//		}
//
//		m_lpItems[j]->SetParentWnd(this);
//	}
//
//	DWORD N =0;
//	list<LPLISTITEM>::iterator i = m_ItemList.begin();
//	while(i!=m_ItemList.end())
//	{
//		LPLISTITEM pItem = *i;
//		if ((N>=m_dwItemStart)&&(N<m_dwItemStart+m_dwNumShowItem))
//		{
//			int F = N - m_dwItemStart;
//			m_lpItems[F]->SetText(pItem->String);
//		}
//		i++;
//		N++;
//	}
//
//
//
//	float Yoffset = 0.01f;
//	if (!m_lpSlider)
//	{
//		g_cMeshTable.Get1NewXMesh((LPMODEL*)&(m_lpSlider),MODELTYPE_SLIDEBAR);
//	}
//
//	float Percent = 0;
//	DWORD ItemCount = m_ItemList.size();
//	if (ItemCount<=m_dwNumShowItem)
//	{
//		ItemCount = 0;
//	}
//	else
//	{
//		ItemCount -= m_dwNumShowItem;
//		Percent = m_dwItemStart*100.0f/(ItemCount);
//	}
//
//	if(m_bAutoResize)
//	{
//		FRECT Rect(m_RelativeRect.Left+m_RelativeRect.Width*K,m_RelativeRect.Top+Yoffset,
//			m_RelativeRect.Width*(1-K),m_RelativeRect.Height-2*Yoffset);
//
//		m_lpSlider->Create(&Rect,"",0,1);
//	}
//	else
//	{
//		FRECT Rect(m_Rect.Left+m_Rect.Width*K,m_Rect.Top+Yoffset,
//			m_Rect.Width*(1-K),m_Rect.Height-2*Yoffset);
//
//		m_lpSlider->CreateAbsolute(&Rect,"",0,1);
//	}
//
//	m_lpSlider->m_fPercent = Percent;
//
//	return E_NOTIMPL;
//}
//
//HRESULT KModelUIListBox::AddString(LPSTR Str)
//{
//	LPLISTITEM pItem = new LISTITEM;
//	wsprintf(pItem->String,"%s",Str);
//	m_ItemList.push_back(pItem);
//	Refresh();
//	return E_NOTIMPL;
//}
//
//HRESULT KModelUIListBox::AddString(LPSTR Str,DWORD Value)
//{
//	LPLISTITEM pItem = new LISTITEM;
//	wsprintf(pItem->String,"%s",Str);
//	pItem->dwValue = Value;
//	m_ItemList.push_back(pItem);
//	Refresh();
//	return E_NOTIMPL;
//}
//int KModelUIListBox::GetItemCount(void)
//{
//	return m_ItemList.size();
//}
//
//HRESULT KModelUIListBox::Clear(void)
//{
//	list<LPLISTITEM>::iterator i = m_ItemList.begin();
//	while(i!=m_ItemList.end())
//	{
//		LPLISTITEM pItem = *i;
//		SAFE_DELETE(pItem);
//		i++;
//	}
//	m_ItemList.clear();
//	return S_OK;
//}
//
//HRESULT KModelUIListBox::SetText(LPSTR Str)
//{
//	return E_NOTIMPL;
//}
//
//void KModelUIListBox::ClearSelection(void)
//{
//	m_nCurSelectItem = -1;
//	for(DWORD i=0;i<m_dwNumShowItem;i++)
//	{
//		m_lpItems[i]->m_bSelected = FALSE;
//	}
//}
//
//HRESULT KModelUIListBox::GetCurSelectedString(LPSTR pOutStr)
//{
//	if (m_nCurSelectItem==-1)
//		return E_FAIL;
//	if (m_nCurSelectItem>= (int) m_ItemList.size())
//		return E_FAIL;
//
//	int n = 0;
//	list<LPLISTITEM>::iterator i = m_ItemList.begin();
//	while(i!=m_ItemList.end())
//	{
//		LPLISTITEM pItem = *i;
//		if (n==m_nCurSelectItem)
//		{
//			wsprintf(pOutStr,"%s",pItem->String);
//			return S_OK;
//		}
//		i++;
//		n++;
//	}
//	return E_FAIL;
//}
//
//HRESULT KModelUIListBox::GetCurSelectedString(LPSTR pOutStr,DWORD* pValue)
//{
//	if (m_nCurSelectItem==-1)
//		return E_FAIL;
//	if (m_nCurSelectItem>= (int) m_ItemList.size())
//		return E_FAIL;
//
//	int n = 0;
//	list<LPLISTITEM>::iterator i = m_ItemList.begin();
//	while(i!=m_ItemList.end())
//	{
//		LPLISTITEM pItem = *i;
//		if (n==m_nCurSelectItem)
//		{
//			wsprintf(pOutStr,"%s",pItem->String);
//			(*pValue) = pItem->dwValue;
//			return S_OK;
//		}
//		i++;
//		n++;
//	}
//	return E_FAIL;
//}
//
//HRESULT KModelUIListBox::SetCurSelectedString(LPSTR pOutStr)
//{
//	if (m_nCurSelectItem==-1)
//		return E_FAIL;
//	if (m_nCurSelectItem>=(int)m_ItemList.size())
//		return E_FAIL;
//
//	int n = 0;
//	list<LPLISTITEM>::iterator i = m_ItemList.begin();
//	while(i!=m_ItemList.end())
//	{
//		LPLISTITEM pItem = *i;
//		if (n==m_nCurSelectItem)
//		{
//			wsprintf(pItem->String,"%s",pOutStr);
//			Refresh();
//			return S_OK;
//		}
//		i++;
//		n++;
//	}
//	return E_FAIL;
//}
//
//HRESULT KModelUIListBox::SelectByValue(DWORD Value)
//{
//	int n=0;
//	list<LPLISTITEM>::iterator i = m_ItemList.begin();
//	while(i!=m_ItemList.end())
//	{
//		LPLISTITEM pItem = *i;
//		if (Value==pItem->dwValue)
//		{
//			m_nCurSelectItem = n;
//			return S_OK;
//		}
//		i++;
//		n++;
//	}
//	return E_FAIL;
//}
//
//HRESULT KModelUIListBox::ComputeRect()
//{
//	KModelUIBase::ComputeRect();
//	Refresh();
//	return S_OK;
//}
//
//HRESULT KModelUIListBox::CleanUp()
//{
//	KModel::CleanUp();
//	Clear();
//	return S_OK;
//}
};
