////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGTreeCtrl.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-2-9 14:59:28
//				  2006-3-16 Update By Chiyer
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KGTREECTRL_H_
#define _KGTREECTRL_H_

// Define The User Message 
#define WM_TR_COPY  WM_USER + 60	// Copy   Tree Item
#define WM_TR_CUT   WM_USER + 61	// Cut	  Tree Item
#define WM_TR_MOVE  WM_USER + 62	// Move	  Tree Item
#define WM_TR_STICK WM_USER + 63	// Stick  Tree Item
#define WM_TR_DEL   WM_USER + 64	// Delete Tree Item

// Copy Type
enum MOVE_TYPE
{
	MOVE_TYPE_COPY,					// Copy
	MOVE_TYPE_CUT					// Cut
};

// Define The KGTreeCtrl
class KGTreeCtrl : public CTreeCtrl
{
public:
	KGTreeCtrl();
	virtual ~KGTreeCtrl();

protected:
	UINT          m_TimerTicks;      // The Holding Time Of Scroll Timer 
	UINT_PTR      m_nScrollTimerID;  // The Scroll Timer Id
	CPoint        m_HoverPoint;      // Mouse Position
	UINT_PTR      m_nHoverTimerID;   // The Hover Timer Id
	DWORD         m_dwDragStart;     // The Start Drag Time
	int           m_bDragging;       // Dragging Flag
	CImageList*   m_pDragImage;      // Drag Image List
	HTREEITEM     m_hItemDragS;      // Source Item
	HTREEITEM     m_hItemDragD;      // Destination Item
    int           m_bDragable;

	UINT_PTR          m_nDrawLineTimerID;// The Move Line Draw Timer ID

	POINT         m_pointLeft;       // The Save Left  Point Of The Select Line
	POINT		  m_pointRigh;		 // The Save Right Point Of The Select Line
	POINT         m_curPointLeft;    // The Current Left  Point Of The Select Line
	POINT		  m_curPointRigh;    // The Current Right Point Of The Select Line

	MOVE_TYPE	  m_eMoveType;		 // Current Copy Type

	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	
	afx_msg LRESULT OnCopy(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStick(WPARAM wParam, LPARAM LPARAM);
	afx_msg LRESULT OnDel(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
	// Do Something Begin Stick Tree Item
	virtual int BeginStick(HTREEITEM hSour, HTREEITEM hDest, MOVE_TYPE eCopyType)
	{
		return true;
	}
	// Do Something After Stick Tree Item
	virtual int AfterStick(HTREEITEM hNewItem, MOVE_TYPE eCopyType){return true;}
	// Do Something Begin Delete Tree Item
	virtual int BeginDelItem(HTREEITEM hDelItem){return true;}
	// Do Something After Delete Tree Item
	virtual int AfterDelItem(HTREEITEM hAfterItem){return true;}
	// Do Something After Copy The Tree Item
	virtual int AfterCopyItem(
		HTREEITEM hCopyItem, HTREEITEM hNewItem, HTREEITEM hParentItem,
		HTREEITEM hAfterItem
	)
	{
		return true;
	}
	int ExpandItem(HTREEITEM hItem);
	int ShrinkItem(HTREEITEM hItem);
	int ExpendAll();
	int ShrinkAll();

    void EnableDrag(int bDrag) { m_bDragable = bDrag; }

	//
	// Tool Functions
	//

	HTREEITEM GetFirstSibingItem(HTREEITEM hTreeItem);
	HTREEITEM GetLastSibingItem(HTREEITEM hTreeItem);

	int GetSibingCount(HTREEITEM hItem);

	//(by huangjinshou 2007Äê7ÔÂ17ÈÕ
	HTREEITEM FindItembySubString(CString strText, HTREEITEM hTreeItem);
	HTREEITEM FindNextItembySubString(CString strText, HTREEITEM hTreeItem);

	void SortTree(HTREEITEM hTreeItem);
	//)
private:
	HTREEITEM CopyBranch(
		HTREEITEM htiBranch, HTREEITEM htiNewParent, HTREEITEM htiAfter
	);
	HTREEITEM CopyItem(
		HTREEITEM hItem, HTREEITEM htiNewParent, HTREEITEM htiAfter
	);

public:
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#endif // _KGTREECTRL_H_