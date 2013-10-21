////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGListCtrl.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-11-5 16:49:11 
//  Comment     : 2006-2-24 Update To V2.0 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KGLISTCRL_H_
#define _KGLISTCRL_H_

#include "AfxTempl.h"

// The User Define Item Data Max Size 
#define ITEM_DATA_SIZE 8000		
#define ITEM_TIP_SIZE  256

#define WM_USER_BACKUP_TAB_FILE  WM_USER + 200

// Define The List Item Text Struct
struct ITEMTEXT
{
	CString  szItemText;                 // Item Text
	COLORREF colorTextSel;				 // Selected Text Color
	COLORREF colorBackSel;				 // Selected Back Color
	COLORREF colorFrameSel;				 // Selected Frame Color
	COLORREF colorText;					 // Text Color
	COLORREF colorBack;					 // Back Color
	COLORREF colorFrame;				 // Frame Color
	COLORREF colorExpandRectFrameSel;	 // Selected Expand Rectangle Frame Color			  
	COLORREF colorExpandRectBackSel;	 // Selected Expand rectangle Back Color
	COLORREF colorExpandRectFrame;		 // Expand Rectangle Frame Color
	COLORREF colorExpandRectBack;		 // Expand Rectangle Back  Color

	ITEMTEXT()
	{
		szItemText	  = "";
		colorFrameSel = RGB(192, 192, 192);
		colorBackSel  = RGB(2, 255, 2);
		colorTextSel  = RGB(2, 2, 2);
		colorFrame	  = RGB(192, 192, 192);
		colorBack     = RGB(255, 255, 255);
		colorText     = RGB(2, 2, 2);
		colorExpandRectFrameSel = RGB(2, 255, 2);
		colorExpandRectBackSel  = RGB(68, 68, 68);  
		colorExpandRectFrame	= RGB(255, 255, 255);
		colorExpandRectBack		= RGB(68, 68, 68); 
	}
};

// Define The List Item Strcut
typedef struct _KGLISTITEM
{
	enum enuListItemType
	{
		EDITTEXT,
		SPINTEXT,
		COMBOX,
		BNBROWSE
	};

	int nItemType;
	CArray<ITEMTEXT, ITEMTEXT&> arryItemText;  // Store The Item Text
	int  nTitleLever;                          // The Title Lever
	int  nTitleFlag;                           // True : Is The Title
	int  nItemState;                           // True : Expand, False : Shrink
	BYTE byItemData[ITEM_DATA_SIZE];           // User Define Data  
	char szTipText[ITEM_TIP_SIZE];

	_KGLISTITEM* pNexSiblingItem;             
	_KGLISTITEM* pFirstChildItem;
	_KGLISTITEM* pPerantItem; 

	_KGLISTITEM()
	{
		pNexSiblingItem = NULL;
		pFirstChildItem = NULL;
		pPerantItem		= NULL;
		nTitleLever		= 0;
		nTitleFlag		= false;
		nItemState		= true;
		nItemType       = EDITTEXT;
		memset(byItemData, 0, sizeof(byItemData));
		*szTipText      = '\0';
	}


	template < typename T >
	int SetStructData( T* pBuffer)
	{
		return memcpy_s(
			byItemData, sizeof(byItemData), (LPVOID)pBuffer, sizeof(T)
			);
	}
	/*int SetStructData(LPVOID pBuffer)
	{
	return memcpy_s(
	byItemData, sizeof(byItemData), pBuffer, sizeof(byItemData)
	);
	}*/

	int GetStructData(LPVOID pBuffer, int nBufferSize)
	{
		return memcpy_s(pBuffer, nBufferSize, byItemData, nBufferSize);
	}

}KGLISTITEM, *LPKGLISTITEM;

// Define The List Item Link (Data Link)
class KGLISTITEMDATELINK
{
public :
	// Add Item  After Indecate Item In Link
	int AddAfter(LPKGLISTITEM  pBeforeItem, LPKGLISTITEM pAddItem);
	// Add Item  Depend Indecate Item In Link
	int AddDepend(LPKGLISTITEM pParentItem, LPKGLISTITEM pAddItem);

	// Dellete The Indicate Item 
	int DelItem(LPKGLISTITEM pDelItem);
	// Dellete The Indicate Item And Its Sibling 
	int DelSubset(LPKGLISTITEM pDelSub);

	// Find One Item Exist whether or not
	int FindItem(LPKGLISTITEM pFindItem);
	int FindItem(LPKGLISTITEM pBeginItem, LPKGLISTITEM pFindItem);

	LPKGLISTITEM FindLastVisualItem(LPKGLISTITEM pFirsttem);
	LPKGLISTITEM FindLastVisualChild(LPKGLISTITEM pParentItem);

	int IsVisual(LPKGLISTITEM pItem);
	int Release();
	int Refrush();
	int IsTitle(LPKGLISTITEM pItem);

	int FindItemByText(CString szItemText, LPKGLISTITEM pStartItem);
	LPKGLISTITEM FindNextItem();

	LPKGLISTITEM GetHead(){return pHead;}

	KGLISTITEMDATELINK();

private :
	LPKGLISTITEM pHead;
	CArray<LPKGLISTITEM, LPKGLISTITEM> arrayFindedItem;
	INT_PTR nCurrentIndex;
};

// Define The KGListCtrl
class KGListCtrl : public CListCtrl
{
public:
	// Draw The List Item
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	// The nFlag : true [+] false [-]
	int  DrawRect(
		CDC* dc, CRect rt, COLORREF colorFrame, COLORREF colorBack, int nFlag
		); 
	// Insert The Item Depend The Indicate Item
	int  InsertDepend(LPKGLISTITEM pParentItem, LPKGLISTITEM pInsertItem);
	// Insert The Item After  The Indicate Item
	int  InsertAfter(LPKGLISTITEM pBeforeItem, LPKGLISTITEM pInsertItme);
	// Expand The Indicate Item 
	int  Expand(LPKGLISTITEM pExpandItem);
	// Update The Indicate Item
	BOOL Update(LPKGLISTITEM pItem, int nRePaint = true);
	BOOL Update(int nItem, int nRePaint = true);
	// Retrieve The Item Data
	virtual int RetrievesItemData(LPKGLISTITEM pItem){return true;}
	// Get Item Text
	CString	GetItemText(int nItem, int nSubItem);
	// Find The Item By Indicate Text
	int  FindItemByText(CString szItemText, LPKGLISTITEM pStartItem = NULL);
	// Delete All Item And Date
	int  RemoveAll();
	LPKGLISTITEM FindNextItem();
	int  SetupToolTip(
		int nDelayTime = 100, COLORREF colorBack = RGB(255, 255, 255),
		COLORREF colorText = RGB(2, 2, 2)
		);
	int  ExpandAll();
	int  ShrinkAll();
	int  FindItemPos(LPKGLISTITEM pItem);

	// find 
	LPKGLISTITEM FindJunior(LPKGLISTITEM pItem);

public:
	KGLISTITEMDATELINK m_listDataTree;           // List Data Tree
	CToolTipCtrl       m_toolTipCtrl;
	int				   m_nToolTipFlag;

protected:
	int  ShrinkItem(LPKGLISTITEM pExpandItem);
	int  ExpandItem(LPKGLISTITEM pExpandItem, LPKGLISTITEM pPreItem);

public:
	KGListCtrl();
	~KGListCtrl();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	DECLARE_MESSAGE_MAP()
};

#endif // _KGLISTCRL_H_