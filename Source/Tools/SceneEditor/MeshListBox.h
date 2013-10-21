#pragma once
//#include "xmesh.h"
#include "./MeshSlideBar.h"

namespace ModelUI
{
#define  LISTBOX_NOTHING        0
#define  LISTBOX_SELECTCHANGE   1
#define  LISTBOX_ITEMDBCLICK    2

//class KModelUIListBox :
//	public KModelUIBase
//{
//public:
//	typedef struct _ListItem
//	{
//		TCHAR String[256];
//		DWORD dwValue;
//	
//		_ListItem()
//		{
//			dwValue = 0;
//			wsprintf(String,"");
//		}
//
//	}LISTITEM,*LPLISTITEM;
//
//	DWORD    m_dwState;
//	DWORD    m_dwStyle;
//	DWORD    m_dwItemStart;
//
//	int            m_nCurSelectItem;
//	DWORD          m_dwNumShowItem;
//	KModelUISlideBar* m_lpSlider;
//
//	list<LPLISTITEM>   m_ItemList;
//	vector<KModelUIItem*> m_lpItems;
//
//public:
//	HRESULT CreateAbsolute(LPFRECT pRect,LPSTR StyleFile,BOOL State,DWORD Style);
//	int Init(IIniFile* pIniFile, const char* pSection);
//
//	KModelUIListBox(void);
//	virtual ~KModelUIListBox(void);
//
//	HRESULT Create(LPFRECT pRect,LPSTR StyleFile,BOOL State,DWORD Style);
//	
//	HRESULT Render(void);
//	HRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);
//	HRESULT Refresh(void);
//	HRESULT AddString(LPSTR Str);
//	HRESULT AddString(LPSTR Str,DWORD Value);
//
//	int GetItemCount(void);
//	HRESULT Clear(void);
//	HRESULT SetText(LPSTR Str);
//protected:
//	void ClearSelection(void);
//public:
//	virtual HRESULT CleanUp();
//	HRESULT ComputeRect();
//	HRESULT SelectByValue(DWORD Value);
//	HRESULT GetCurSelectedString(LPSTR pOutStr,DWORD* pValue);
//	HRESULT GetCurSelectedString(LPSTR pOutStr);
//	HRESULT SetCurSelectedString(LPSTR pOutStr);
//};
};