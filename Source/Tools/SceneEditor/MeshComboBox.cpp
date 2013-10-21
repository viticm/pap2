// MeshComboBox.cpp: implementation of the KModelUIComboBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "MeshComboBox.h"

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

//KModelUIComboBox::KModelUIComboBox()
//{
//	m_bDroped = FALSE;
//	m_dwCurItemValue = 0;
//}
//
//KModelUIComboBox::~KModelUIComboBox()
//{
//
//}
//
//HRESULT KModelUIComboBox::Clear()
//{
//	m_lpListBox->Clear();
//	m_bDroped = FALSE;
//	return S_OK;
//}
//
//int KModelUIComboBox::Init(IIniFile* pIniFile, const char* pSection)
//{
//	KModelUIBase::Init(pIniFile,pSection);
//	m_bShowTitle = FALSE;
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpCurItem,
//		MODELTYPE_ITEM);
//
//	m_lpCurItem->CreateAbsolute(&m_Rect,"",0,0);
//	m_lpCurItem->SetParentWnd(this);
//	m_lpCurItem->m_fZ = 0.0001f;
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpListBox,
//		MODELTYPE_LISTBOX);
//
//	FRECT Rect1(m_Rect.Left,m_Rect.Top,
//		m_Rect.Width,m_Rect.Height*8);
//
//	m_lpListBox->CreateAbsolute(&Rect1,"",0,0);
//	m_lpListBox->SetParentWnd(this);
//	m_lpListBox->m_fZ = 0.0001f;
//
//	TCHAR Name[256];
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,
//		"ToolBar\\Slider.tga");
//
//	return S_OK;
//}
//
//HRESULT KModelUIComboBox::Create(LPFRECT pRect,LPSTR StyleFile,BOOL State,DWORD Style)
//{
//	KModelUIBase::Create(pRect,StyleFile,State,Style);
//	m_bShowTitle = FALSE;
//	
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpCurItem,
//		MODELTYPE_ITEM);
//	
//	m_lpCurItem->Create(&m_RelativeRect,"",State,Style);
//	m_lpCurItem->SetParentWnd(this);
//	m_lpCurItem->m_fZ = 0.0001f;
//
//	g_cMeshTable.Get1NewXMesh((LPMODEL*)&m_lpListBox,
//		MODELTYPE_LISTBOX);
//	
//	FRECT Rect1(m_RelativeRect.Left,m_RelativeRect.Top,
//		m_RelativeRect.Width,m_RelativeRect.Height*8);
//	
//	m_lpListBox->Create(&Rect1,"",0,0);
//	m_lpListBox->SetParentWnd(this);
//	m_lpListBox->m_fZ = 0.0001f;
//
//	TCHAR Name[256];
//	wsprintf(Name,"%s\\%s",g_Def_AppDirectory,
//		"ToolBar\\Slider.tga");
//	
//	/*m_CmdDrop.Create(Left+Width-Height*0.7f,Top,
//		Height*0.65f,Height,
//		0,0,Name,0);
//	g_cGraphicsTool.LoadTexture4Command(&m_CmdDrop);*/
//
//
//	return S_OK;
//}
//
//HRESULT KModelUIComboBox::Render()
//{
//	KModelUIBase::Render();
//
//	if (!m_bDroped)
//	{
//		SAFE_RENDER(m_lpCurItem);
//		//g_cGraphicsTool.DrawCommand(&m_CmdDrop);
//	}
//	else
//	{
//		SAFE_RENDER(m_lpListBox);
//	}
//	return S_OK;
//}
//HRESULT KModelUIComboBox::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	if ((!m_bDroped)&&(m_lpListBox->GetItemCount()>0))
//	{
//		if (ITEM_CLICK == m_lpCurItem->ProcessMessage(message,wParam,lParam))
//		{
//			m_bDroped = TRUE;
//			return COMBOBOX_DROP;
//		}
//	}
//	else
//	{
//		DWORD MSG = m_lpListBox->ProcessMessage(message,wParam,lParam);
//
//		if (MSG==LISTBOX_SELECTCHANGE)
//		{
//			m_bDroped = FALSE;
//			m_CmdDrop.State = 0;
//			TCHAR str[256];
//			m_lpListBox->GetCurSelectedString(str,&m_dwCurItemValue);
//			m_lpCurItem->SetText(str);
//			return COMBOBOX_ITEMSELECT;
//		}
//	}
//	return S_OK;
//}
//
//
//HRESULT KModelUIComboBox::AddString(LPSTR Str, DWORD Value)
//{
//	m_lpListBox->AddString(Str,Value);
//	return S_OK;
//}
//
//HRESULT KModelUIComboBox::SetCurItembyValue(DWORD Value)
//{
//	m_lpListBox->SelectByValue(Value);
//	TCHAR str[256];
//	m_lpListBox->GetCurSelectedString(str,&m_dwCurItemValue);
//	m_lpCurItem->SetText(str);
//	return S_OK;
//
//}
//
//DWORD KModelUIComboBox::GetCuritemValue()
//{
//	TCHAR str[256];
//	m_lpListBox->GetCurSelectedString(str,&m_dwCurItemValue);
//	return m_dwCurItemValue;
//}
};
