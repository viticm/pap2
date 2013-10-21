// MeshListCtrl.h: interface for the KModelUIListCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHLISTCTRL_H__3EB3A68A_C684_4348_A3CA_2B0EDA9AB75C__INCLUDED_)
#define AFX_MESHLISTCTRL_H__3EB3A68A_C684_4348_A3CA_2B0EDA9AB75C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MeshWinBase.h"
namespace ModelUI
{
//class KModelUIListCtrl : public KModelUIBase  
//{
//public:
//	enum LISTITEMTYPE
//	{
//		ITEM_UNKNOWN,
//		ITEM_STRING,
//		ITEM_MATERIAL,
//		ITEM_TEXTURE,
//		ITEM_COLORREF,
//	};
//	struct LISTITEM
//	{
//		string        scItemText;
//		LISTITEMTYPE  emType;
//		DWORD         dwData;
//		DWORD         dwState;
//		LISTITEM()
//		{
//			emType = ITEM_UNKNOWN;
//			dwData = 0;
//			dwState = 0;
//			scItemText = "ListItem";
//		}
//	};
//	vector<LISTITEM>m_ListItems;
//
//	vector<KModelUIItem*>m_ShowItems;
//	float m_fItemWidth;
//	float m_fItemHeight;
//	int   m_nNumItem_X;
//	int   m_nNumItem_Y;
//	int   m_nItemStart;
//	int   m_nNumItem;
//	int   m_nNumItemOld;
//	BOOL  m_bMultiSelect;
//	
//public:
//
//	int     Init(IIniFile* pIniFile, const char* pSection);
//	HRESULT SelectInvert();
//	HRESULT SelectNone();
//	HRESULT SelectAll();
//	HRESULT Clear();
//	HRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);
//	HRESULT AddItemMaterial(KModel::LPMATERIAL pMat);
//	HRESULT Refresh();
//	HRESULT Render();
//	HRESULT FrameMove();
//	HRESULT ComputeRect();
//	HRESULT Create(LPFRECT pRect,LPSTR StyleFile,BOOL State,DWORD Style);
//	KModelUIListCtrl();
//	virtual ~KModelUIListCtrl();
//
//};
};

#endif // !defined(AFX_MESHLISTCTRL_H__3EB3A68A_C684_4348_A3CA_2B0EDA9AB75C__INCLUDED_)
