////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGTreeCtrl.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-2-13 10:44:01
//				  2006-3-16 Update By Chiyer
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KGTreeCtrl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define DRAG_DELAY 60

KGTreeCtrl::KGTreeCtrl()
{
	m_bDragging  = false;
    m_bDragable  = false;
	m_hItemDragS = NULL;
	m_hItemDragD = NULL;
	m_eMoveType  = MOVE_TYPE_COPY;
}

KGTreeCtrl::~KGTreeCtrl()
{
}

BEGIN_MESSAGE_MAP(KGTreeCtrl, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_MESSAGE(WM_TR_COPY,  OnCopy)
	ON_MESSAGE(WM_TR_STICK, OnStick)
	ON_MESSAGE(WM_TR_DEL,   OnDel)
	ON_WM_KEYUP()
END_MESSAGE_MAP()

LRESULT KGTreeCtrl::OnDel(WPARAM wParam, LPARAM lParam)
{
	int nResult  = false;
	int nRetCode = false;

	HTREEITEM hDelItem  = NULL;
	HTREEITEM hNextItem = NULL;

	int nItemDate = 0;

	hDelItem = (HTREEITEM)wParam;;
	KG_PROCESS_ERROR(hDelItem);

	hNextItem = GetNextSiblingItem(hDelItem);

	nRetCode = BeginDelItem(hDelItem);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = DeleteItem(hDelItem);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = AfterDelItem(hNextItem);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

LRESULT KGTreeCtrl::OnCopy(WPARAM wParam, LPARAM lParam)
{
	int nResult  = false;
	int nRetCode = false;

	m_hItemDragS = (HTREEITEM)wParam;
	m_eMoveType  = (MOVE_TYPE)lParam;

	nResult = true;
//Exit0:
	return nResult;
}

int KGTreeCtrl::ExpandItem(HTREEITEM hTreeItem)
{
	if (hTreeItem)
	{
		Expand(hTreeItem, TVE_EXPAND);
		ExpandItem(GetChildItem(hTreeItem));
		ExpandItem(GetNextSiblingItem(hTreeItem));
	}

	return true;
}

int KGTreeCtrl::ExpendAll()
{
	return ExpandItem(GetRootItem());
}


int KGTreeCtrl::ShrinkItem(HTREEITEM hTreeItem)
{
	if (hTreeItem)
	{
		Expand(hTreeItem, TVE_TOGGLE);
		ShrinkItem(GetChildItem(hTreeItem));
		ShrinkItem(GetNextSiblingItem(hTreeItem));
	}

	return true;
}

int KGTreeCtrl::ShrinkAll()
{
	return ShrinkItem(GetRootItem());
}

LRESULT KGTreeCtrl::OnStick(WPARAM wParam, LPARAM lParam)
{
	int nResult  = false;
	int nRetCode = false;

	HTREEITEM  hParentItem = NULL;
	HTREEITEM  hTempItem   = NULL;
	HTREEITEM  hNewItem    = NULL;

	m_hItemDragD = (HTREEITEM)wParam;

	KG_PROCESS_ERROR(m_hItemDragS);
	KG_PROCESS_ERROR(m_hItemDragD);

	nRetCode = BeginStick(m_hItemDragS, m_hItemDragD, m_eMoveType);
	KG_PROCESS_ERROR(nRetCode);

    hParentItem = GetParentItem(m_hItemDragD);
	hTempItem   = m_hItemDragD;

	while ((hTempItem = GetParentItem(hTempItem)) != NULL)
	{
		if( hTempItem == m_hItemDragS )
		{
			HTREEITEM  hNewItemTemp = CopyBranch(m_hItemDragS, NULL, TVI_LAST);
			HTREEITEM  hNewItem     = CopyBranch(
				hNewItemTemp, hParentItem, m_hItemDragD
			);
			DeleteItem(hNewItemTemp);
			SelectItem(hNewItem);
			KG_PROCESS_SUCCESS(true);
		}
	}

	hNewItem = CopyBranch(m_hItemDragS, hParentItem, m_hItemDragD);
	KG_PROCESS_ERROR(hNewItem);

	if (m_eMoveType == MOVE_TYPE_CUT)
	{
		OnDel((WPARAM)m_hItemDragS, 0);
		m_hItemDragS = NULL;
	}

	nRetCode = AfterStick(hNewItem, m_eMoveType);
	KG_PROCESS_ERROR(nRetCode);

	//ExpandItem(hNewItem);
	SelectItem(hNewItem);

Exit1:
	nResult = true;
Exit0:
	Invalidate();
	return nResult;
}

void KGTreeCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult)
{
    if (m_bDragable == false)
        return;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	if ((GetTickCount() - m_dwDragStart) < DRAG_DELAY)
		return;

	m_hItemDragS = pNMTreeView->itemNew.hItem;
	m_hItemDragD = NULL;

	m_pDragImage = CreateDragImage(m_hItemDragS);
	if (!m_pDragImage)
		return;

	m_bDragging = true;
	m_pDragImage->BeginDrag(0, CPoint(8, 8));
	CPoint  pt = pNMTreeView->ptDrag;
	ClientToScreen(&pt);
	m_pDragImage->DragEnter(this, pt);

	SetCapture();

	m_nScrollTimerID   = SetTimer(2, 40, NULL);
	m_nDrawLineTimerID = SetTimer(4, 40, NULL);
}

void KGTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	HTREEITEM  hItem;
	UINT       flags;

	if (m_nHoverTimerID)
	{
		KillTimer(m_nHoverTimerID);
		m_nHoverTimerID = 0;
	}

	m_nHoverTimerID = SetTimer(1, 800, NULL);
	m_HoverPoint    = point;

	if (m_bDragging)
	{
		CPoint  pt = point;
		CImageList::DragMove(pt);

		CImageList::DragShowNolock(false);
		hItem = HitTest(point, &flags);
		if (hItem)
		{
			SelectDropTarget(hItem);

			CRect rect;
			GetItemRect(hItem, &rect, true);
			rect.left  -= 35;
			m_curPointLeft.x = rect.left;
			m_curPointLeft.y = rect.bottom;
			m_curPointRigh.x = rect.right;
			m_curPointRigh.y = rect.bottom;

			m_hItemDragD = hItem;
		}

		CImageList::DragShowNolock(true);

		CRect  rect;
		GetClientRect(&rect);
		if (point.x < rect.left + 20)
			m_hItemDragD = NULL;
	}

	CTreeCtrl::OnMouseMove(nFlags, point);
}

void KGTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	CTreeCtrl::OnLButtonUp(nFlags, point);

	if (m_bDragging)
	{
		m_bDragging = false;
		CImageList::DragLeave(this);
		CImageList::EndDrag();
		ReleaseCapture();
		delete m_pDragImage;

		SelectDropTarget(NULL);
		m_eMoveType = MOVE_TYPE_CUT;
		OnStick((WPARAM)m_hItemDragD, (LPARAM)0);

		KillTimer(m_nScrollTimerID);
		KillTimer(m_nDrawLineTimerID);
	}
}

HTREEITEM KGTreeCtrl::CopyItem(
	HTREEITEM hItem, HTREEITEM htiNewParent, HTREEITEM htiAfter
)
{
	int nResult  = false;
	int nRetCode = false;

	TV_INSERTSTRUCT  tvstruct;
	HTREEITEM        hNewItem = NULL;
	CString          sText;

	tvstruct.item.hItem = hItem;
	tvstruct.item.mask  =
		TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	GetItem(&tvstruct.item);
	sText = GetItemText(hItem);
	tvstruct.item.cchTextMax = sText.GetLength();
	tvstruct.item.pszText    = sText.LockBuffer();

	tvstruct.hParent         = htiNewParent;
	tvstruct.hInsertAfter    = htiAfter;
	tvstruct.item.mask       = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	hNewItem = InsertItem(&tvstruct);
	sText.ReleaseBuffer();

	SetItemData(hNewItem, GetItemData(hItem));
	SetItemState(
		hNewItem, GetItemState(hItem, TVIS_STATEIMAGEMASK), TVIS_STATEIMAGEMASK
	);

	nRetCode = AfterCopyItem(
		hItem, hNewItem, htiNewParent, htiAfter
	);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return hNewItem;
}

HTREEITEM KGTreeCtrl::CopyBranch(
	HTREEITEM htiBranch, HTREEITEM htiNewParent, HTREEITEM htiAfter
)
{
	HTREEITEM  hChild;
	HTREEITEM  hNewItem = CopyItem(htiBranch, htiNewParent, htiAfter);

	hChild = GetChildItem(htiBranch);
	htiAfter = NULL;

	while (hChild != NULL)
	{
		htiAfter = CopyBranch(hChild, hNewItem, htiAfter);
		hChild = GetNextSiblingItem(hChild);
	}

	return  hNewItem;
}

void KGTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	HTREEITEM hHitItem = HitTest(point);
	RECT rect;
	m_dwDragStart = GetTickCount();
	GetItemRect(hHitItem, &rect, true);
	if (point.x > rect.right)
		SelectItem(hHitItem);
	CTreeCtrl::OnLButtonDown(nFlags, point);
}

void KGTreeCtrl::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == m_nHoverTimerID)
	{
		KillTimer(m_nHoverTimerID);
		m_nHoverTimerID = 0;
		HTREEITEM  trItem = 0;
		UINT  uFlag = 0;
		trItem = HitTest(m_HoverPoint, &uFlag);
		if (trItem && m_bDragging)
		{
			//SelectItem(trItem);
			Expand(trItem, TVE_EXPAND);
			Invalidate();

			CRect rect;
			GetItemRect(trItem, &rect, true);
			rect.left -= 35;
			m_curPointLeft.x = rect.left;
			m_curPointLeft.y = rect.bottom;
			m_curPointRigh.x = rect.right;
			m_curPointRigh.y = rect.bottom;
		}
	}
	else if(nIDEvent == m_nScrollTimerID)
	{
		m_TimerTicks++;
		CPoint  pt;
		GetCursorPos(&pt);
		CRect  rect;
		GetClientRect(&rect);
		ClientToScreen(&rect);

		HTREEITEM  hItem = GetFirstVisibleItem();

		if ( pt.y < rect.top +10)
		{
			int  slowscroll = 6 - (rect.top + 10 - pt.y )/20;
			if( 0 == (m_TimerTicks % ((slowscroll > 0) ? slowscroll : 1)) )
			{
				CImageList::DragShowNolock ( false );
				SendMessage( WM_VSCROLL,SB_LINEUP );
				SelectDropTarget( hItem );
				m_hItemDragD = hItem;
				CImageList::DragShowNolock ( true );
			}
		}
		else if( pt.y > rect.bottom - 10 )
		{
			int  slowscroll = 6 - (pt.y - rect.bottom + 10)/20;
			if( 0 == (m_TimerTicks % ((slowscroll > 0) ? slowscroll : 1)) )
			{
				CImageList::DragShowNolock ( false );
				SendMessage( WM_VSCROLL,SB_LINEDOWN );
				int  nCount = GetVisibleCount();
				for( int i=0 ; i<nCount-1 ; i++ )
					hItem = GetNextVisibleItem( hItem );
				if( hItem )
					SelectDropTarget( hItem );
				m_hItemDragD = hItem;
				CImageList::DragShowNolock ( true );
			}
		}
	}
	else if (nIDEvent == m_nDrawLineTimerID)
	{
		CClientDC dc(this);

		CPen penP(PS_SOLID, 2, GetBkColor());
		dc.SelectObject(&penP);
		dc.MoveTo(m_pointLeft);
		dc.LineTo(m_pointRigh);
		dc.MoveTo(m_pointLeft.x, m_pointLeft.y - 5);
		dc.LineTo(m_pointLeft.x, m_pointLeft.y + 5);

		CPen penN(PS_SOLID, 2, RGB(122, 178, 255));
		dc.SelectObject(&penN);
		dc.MoveTo(m_curPointLeft);
		dc.LineTo(m_curPointRigh);
		dc.MoveTo(m_curPointLeft.x, m_curPointLeft.y - 5);
		dc.LineTo(m_curPointLeft.x, m_curPointLeft.y + 5);

		m_pointLeft.x = m_curPointLeft.x;
		m_pointLeft.y = m_curPointLeft.y;
		m_pointRigh.x = m_curPointRigh.x;
		m_pointRigh.y = m_curPointRigh.y;
	}
	else
		CTreeCtrl::OnTimer(nIDEvent);
}

HTREEITEM KGTreeCtrl::GetFirstSibingItem(HTREEITEM hTreeItem)
{
	HTREEITEM hResultItem = NULL;
	HTREEITEM hPrevItem   = NULL;

	KG_PROCESS_ERROR(hTreeItem);

	hPrevItem = hTreeItem;
	while (hPrevItem)
	{
		hResultItem = hPrevItem;
		hPrevItem = GetPrevSiblingItem(hPrevItem);
	}

Exit0:
	return hResultItem;
}

HTREEITEM KGTreeCtrl::GetLastSibingItem(HTREEITEM hTreeItem)
{
	HTREEITEM hResultItem = NULL;
	HTREEITEM hNextItem   = NULL;

	KG_PROCESS_ERROR(hTreeItem);

	hNextItem = hTreeItem;
	while (hNextItem)
	{
		hResultItem = hNextItem;
		hNextItem = GetNextSiblingItem(hNextItem);
	}

Exit0:
	return hResultItem;
}

HTREEITEM KGTreeCtrl::FindItembySubString(CString strText, HTREEITEM hTreeItem)
{
	HTREEITEM FindNode = NULL;

	//没有数据返回NULL
	int nCount = GetCount();
	if ( !nCount ) return NULL;

	if (!hTreeItem)
		hTreeItem = GetRootItem();

	FindNode = hTreeItem;

	CString ItemText = GetItemText(FindNode);
	int i= ItemText.Find(strText);

	if( i != -1)
		return FindNode;

	HTREEITEM BrotherNode = FindNode;
	if(ItemHasChildren(FindNode))
	{
		FindNode = GetChildItem(FindNode);
		FindNode = FindItembySubString(strText, FindNode); 
		KG_PROCESS_SUCCESS(FindNode);
	}

	FindNode = GetNextSiblingItem(BrotherNode);
	KG_PROCESS_ERROR(FindNode);

	FindNode = FindItembySubString(strText, FindNode);
	KG_PROCESS_SUCCESS(FindNode);
	 	
Exit0:
	return NULL;
Exit1:
	return FindNode;
}

HTREEITEM KGTreeCtrl::FindNextItembySubString(CString strText, HTREEITEM hTreeItem)
{
	//没有数据返回NULL
	int nCount = GetCount();
	if ( !nCount ) return NULL;
	
	HTREEITEM FindNode = NULL;

	if (!hTreeItem)
		hTreeItem = GetRootItem();

	if (ItemHasChildren(hTreeItem))
	{
		FindNode = GetChildItem(hTreeItem);
		FindNode = FindItembySubString(strText,FindNode);
		KG_PROCESS_SUCCESS(FindNode);
	}
	
	FindNode = GetNextSiblingItem(hTreeItem);
	if (FindNode)
	{
		FindNode = FindItembySubString(strText,FindNode);
		KG_PROCESS_SUCCESS(FindNode);
	}
	
	while((hTreeItem = GetParentItem(hTreeItem)) != 0)
	{
		FindNode = GetNextSiblingItem(hTreeItem);
		if (FindNode)
		{
			FindNode = FindItembySubString(strText,FindNode);
			KG_PROCESS_SUCCESS(FindNode);
		}
	}
	
	return NULL;
Exit1:
	return FindNode;
}

int KGTreeCtrl::GetSibingCount(HTREEITEM hTreeItem)
{
	int nResult  = false;
	int nRetCode = false;

	HTREEITEM hTempItem = NULL;
	int nCount  = 0;

	KG_PROCESS_ERROR(hTreeItem);

	hTempItem = GetParentItem(hTreeItem);

	if (hTempItem)
	{
		hTempItem = GetChildItem(hTempItem);
	}
	else
	{
		hTempItem = GetRootItem();
	}

	while (hTempItem)
	{
		nCount++;
		hTempItem = GetNextSiblingItem(hTempItem);
	}

	nResult = nCount;
Exit0:
	return nResult;
}

void KGTreeCtrl::SortTree(HTREEITEM hTreeItem)
{
	HTREEITEM hSubItem = NULL;
	if (!hTreeItem)
	{
		hTreeItem = GetRootItem();
//		hTreeItem = GetChildItem(TVI_ROOT);
		KG_PROCESS_ERROR(hTreeItem);
		SortChildren(NULL);
	}
	hSubItem = hTreeItem;
	SortChildren(hSubItem);
	HTREEITEM BrotherItem = hSubItem;
	if (ItemHasChildren(hSubItem))
	{
		hSubItem = GetChildItem(hSubItem);
		SortTree(hSubItem);
	}
	hSubItem = GetNextSiblingItem(BrotherItem);
	KG_PROCESS_ERROR(hSubItem);

	SortTree(hSubItem);
Exit0:
	return ;
}

void KGTreeCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int nResult  = false;
	int nRetCode = false;

	switch (nChar)
	{
	case 'C' :
		{
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				HTREEITEM hItem = GetSelectedItem();
				KG_PROCESS_ERROR(hItem);
				OnCopy((WPARAM)hItem, (LPARAM)MOVE_TYPE_COPY);
			}
		}
		break;
	case 'X' :
		{
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				HTREEITEM hItem = GetSelectedItem();
				KG_PROCESS_ERROR(hItem);
				OnCopy((WPARAM)hItem, (LPARAM)MOVE_TYPE_CUT);
			}
		}
		break;
	case VK_DELETE :
		{
			HTREEITEM hItem = GetSelectedItem();
			KG_PROCESS_ERROR(hItem);
			OnDel((WPARAM)hItem, (LPARAM)0);
		}
		break;
	case 'V' :
		{
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				HTREEITEM hItem = GetSelectedItem();
				KG_PROCESS_ERROR(hItem);
				OnStick((WPARAM)hItem, 0);
			}
		}
		break;
	default:
		break;
	}

	nResult = true;
Exit0:
	return CTreeCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}
