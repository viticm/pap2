////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGListCtrl.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-11-5 16:49:17
//  Comment     : 2006-2-24 Update To V2.0
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "StdAfx.h"
#include "KGListCtrl.h"
int KGLISTITEMDATELINK::AddAfter(
								 LPKGLISTITEM pBeforeItem, LPKGLISTITEM pAddItem
								 )
{
	int nResult  = false;
	int nRetCode = false;

	KG_PROCESS_ERROR(pBeforeItem);
	KG_PROCESS_ERROR(pAddItem);

	nRetCode = FindItem(pBeforeItem);
	KG_PROCESS_ERROR(nRetCode);

	pAddItem->pNexSiblingItem    = pBeforeItem->pNexSiblingItem;
	pBeforeItem->pNexSiblingItem = pAddItem;
	pAddItem->pPerantItem        = pBeforeItem->pPerantItem;

	if (pAddItem->pPerantItem)
	{
		pAddItem->nTitleLever = pAddItem->pPerantItem->nTitleLever + 1;
	}
	else
	{
		pAddItem->nTitleLever = 0;
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGLISTITEMDATELINK::AddDepend(
								  LPKGLISTITEM pParentItem, LPKGLISTITEM pAddItem
								  )
{
	int nResult  = false;
	int nRetCode = false;

	LPKGLISTITEM pBeforeItem = NULL;
	LPKGLISTITEM pTempItem   = NULL;

	KG_PROCESS_ERROR(pAddItem);

	if (pParentItem)
	{	
		KG_PROCESS_ERROR(pHead);
		nRetCode = FindItem(pParentItem);
		KG_PROCESS_ERROR(nRetCode);

		pTempItem = pParentItem->pFirstChildItem;
		if (!pTempItem)
		{
			pParentItem->pFirstChildItem = pAddItem;
			pAddItem->pPerantItem		 = pParentItem;
			pAddItem->nTitleLever		 = pParentItem->nTitleLever + 1;
		}
		else
		{
			while (pTempItem)
			{
				pBeforeItem = pTempItem;
				pTempItem   = pTempItem->pNexSiblingItem;
			}
			AddAfter(pBeforeItem, pAddItem);
		}

	}
	else
	{
		if (!pHead)
		{
			pHead = pAddItem;
			pAddItem->pPerantItem = NULL;
			pAddItem->nTitleLever = 0;
		}
		else
		{
			pTempItem = pHead;
			while (pTempItem)
			{
				pBeforeItem = pTempItem;
				pTempItem   = pTempItem->pNexSiblingItem;
			}
			AddAfter(pBeforeItem, pAddItem);
		}
	}

	nResult = true;	 
Exit0:
	return nResult;
}

int KGLISTITEMDATELINK::IsTitle(LPKGLISTITEM pItem)
{
	int nResult  = false;
	int nRetCode = false;

	KG_PROCESS_ERROR(pItem);
	KG_PROCESS_ERROR(pItem->pFirstChildItem);

	nResult = true;
Exit0:
	return nResult;
}
KGLISTITEMDATELINK::KGLISTITEMDATELINK()
{
	pHead            = NULL;
	nCurrentIndex    = 0;
	arrayFindedItem.RemoveAll();
}

int KGLISTITEMDATELINK::Refrush()
{
	if (arrayFindedItem.GetSize() >= 1)
	{
		arrayFindedItem.RemoveAll();
	}
	nCurrentIndex = 0;
	return true;
}

int KGLISTITEMDATELINK::DelItem(LPKGLISTITEM pDelItem)
{
	int nResult  = false;
	int nRetCode = false;

	LPKGLISTITEM pPreItem = pDelItem->pPerantItem;

	KG_PROCESS_ERROR(pDelItem);
	nRetCode = FindItem(pDelItem);
	KG_PROCESS_ERROR(nRetCode);

	DelSubset(pDelItem->pFirstChildItem);

	if (pPreItem)
	{
		pPreItem = pPreItem->pFirstChildItem;
		if (pPreItem == pDelItem)
		{
			pPreItem->pFirstChildItem = pDelItem->pNexSiblingItem;
		}
		else
		{
			while (pPreItem->pNexSiblingItem != pDelItem && pPreItem)
			{
				pPreItem = pPreItem->pNexSiblingItem;
			}
			KG_PROCESS_ERROR(pPreItem);
			pPreItem->pNexSiblingItem = pDelItem->pNexSiblingItem;
		}
	}
	else
	{
		pPreItem = pHead;
		if (pPreItem == pDelItem)
		{
			pHead = pDelItem->pNexSiblingItem;
		}
		else
		{
			while (pPreItem->pNexSiblingItem != pDelItem && pPreItem)
			{
				pPreItem = pPreItem->pNexSiblingItem;
			}
			KG_PROCESS_ERROR(pPreItem);
			pPreItem->pNexSiblingItem = pDelItem->pNexSiblingItem;
		}
	}

	SAFE_DELETE(pDelItem);

	nRetCode = true;
Exit0:
	return nResult;
}

int KGLISTITEMDATELINK::DelSubset(LPKGLISTITEM pDelItem)
{	
	if (pDelItem)
	{
		DelSubset(pDelItem->pFirstChildItem);
		DelSubset(pDelItem->pNexSiblingItem);
		SAFE_DELETE(pDelItem);
	}
	return true;
}

int KGLISTITEMDATELINK::IsVisual(LPKGLISTITEM pItem)
{
	int nResult  = false;

	ASSERT(pItem);

	LPKGLISTITEM pTempItem = pItem->pPerantItem;

	while (pTempItem)
	{
		if (!pTempItem->nItemState)
			break;
		pTempItem = pTempItem->pPerantItem;
	}

	if (!pTempItem)
		nResult = true;

	return nResult;
}

int KGLISTITEMDATELINK::FindItemByText(CString szItemText, LPKGLISTITEM pStartItem)
{
	int nResult  = false;
	int nRetCode = false;

	INT_PTR nSubItemCount = 0;
	INT_PTR i = 0;

	KG_PROCESS_ERROR(szItemText != "");
	KG_PROCESS_ERROR(pStartItem);

	nSubItemCount = pStartItem->arryItemText.GetCount();
	for (i = 0; i < nSubItemCount; i++)
	{
		if (pStartItem->arryItemText.GetAt(i).szItemText.Find(szItemText) != -1)
		{
			arrayFindedItem.Add(pStartItem);
			break;
		}
	}

	FindItemByText(szItemText, pStartItem->pFirstChildItem);
	FindItemByText(szItemText, pStartItem->pNexSiblingItem);

	nResult = true;
Exit0:
	return nResult;
}

LPKGLISTITEM KGLISTITEMDATELINK::FindNextItem()
{
	int nResult  = false;
	int nRetCode = false;

	LPKGLISTITEM pRetItem = NULL;
	INT_PTR nFindedItemArraySize = arrayFindedItem.GetCount();

	if (nCurrentIndex < nFindedItemArraySize)
	{
		pRetItem = arrayFindedItem.GetAt(nCurrentIndex);
		nCurrentIndex++;
	}

	return pRetItem;
}

LPKGLISTITEM KGLISTITEMDATELINK::FindLastVisualItem(LPKGLISTITEM pFirsttem)
{
	LPKGLISTITEM pTempItem = pFirsttem;

	ASSERT(pTempItem);

	while (pTempItem->pNexSiblingItem)
	{
		pTempItem = pTempItem->pNexSiblingItem;
	}
	if (pTempItem->pFirstChildItem)
	{
		return FindLastVisualItem(pTempItem->pFirstChildItem);
	}
	else
	{
		return pTempItem;
	}
}

LPKGLISTITEM KGLISTITEMDATELINK::FindLastVisualChild(LPKGLISTITEM pParentItem)
{
	LPKGLISTITEM pRetItem = pParentItem;

	if (pRetItem)
	{
		if (pRetItem->nItemState)
			pRetItem = pRetItem->pFirstChildItem;
		else 
			pRetItem = NULL;
	}
	else
	{
		pRetItem = pHead;
	}

	if (pRetItem)
	{
		pRetItem = FindLastVisualItem(pRetItem);
	}


	return pRetItem;
}

int KGLISTITEMDATELINK::FindItem(LPKGLISTITEM pFindItem)
{
	ASSERT(pFindItem);
	return FindItem(pHead, pFindItem);
}

int KGLISTITEMDATELINK::FindItem(
								 LPKGLISTITEM pBeginItem, LPKGLISTITEM pFindItem
								 )
{
	ASSERT(pFindItem);

	int nFindedFlag = false;

	if (pBeginItem)
	{
		if (pBeginItem == pFindItem)
			nFindedFlag = true;
		else
			nFindedFlag = true;
		if (!nFindedFlag)
			nFindedFlag = FindItem(pBeginItem->pFirstChildItem, pFindItem);
		if (!nFindedFlag)		
			nFindedFlag = FindItem(pBeginItem->pNexSiblingItem, pFindItem);
	}

	return nFindedFlag;
}

int KGLISTITEMDATELINK::Release()
{
	DelSubset(pHead);
	pHead = NULL;
	return true;
}

KGListCtrl::KGListCtrl()
{
	m_nToolTipFlag = false;
}

KGListCtrl::~KGListCtrl(void)
{
	m_listDataTree.Release();
}

int KGListCtrl::DrawRect(
						 CDC* dc, CRect rt, COLORREF colorFrame, COLORREF colorBack, int nFlag
						 )
{
	int nLen = (int)((rt.right - rt.left) * 0.5); 

	CPoint point_top(rt.left + nLen, rt.top + 2); 
	CPoint point_dwn(rt.left + nLen, rt.bottom - 1);
	CPoint point_lef(rt.left + 2, rt.top + nLen);
	CPoint point_rig(rt.right - 1, rt.top + nLen);

	dc->FillRect(&rt, &CBrush(colorBack));
	CPen pen(PS_SOLID, 1, colorFrame);
	dc->SelectObject(&pen);
	dc->MoveTo(rt.left,  rt.top);
	dc->LineTo(rt.right, rt.top);
	dc->LineTo(rt.right, rt.bottom);
	dc->LineTo(rt.left,  rt.bottom);
	dc->LineTo(rt.left,  rt.top);

	if (nFlag)
	{
		dc->MoveTo(point_top);
		dc->LineTo(point_dwn);
		dc->MoveTo(point_lef);
		dc->LineTo(point_rig);
	}
	else
	{
		dc->MoveTo(point_lef);
		dc->LineTo(point_rig);
	}

	return true;
}

int KGListCtrl::FindItemByText(CString szItemText, LPKGLISTITEM pStartItem /* = NULL */)
{
	int nResult  = false;
	int nRetCode = false;

	KG_PROCESS_ERROR(szItemText != "");
	if (!pStartItem)
	{
		pStartItem = m_listDataTree.GetHead();
	}
	nRetCode = m_listDataTree.Refrush();
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_listDataTree.FindItemByText(szItemText, pStartItem);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;

}

LPKGLISTITEM KGListCtrl::FindNextItem()
{
	return m_listDataTree.FindNextItem();
}

int KGListCtrl::FindItemPos(LPKGLISTITEM pItem)
{
	int nResult = -1;

	KG_PROCESS_ERROR(pItem);

	for (int i = 0; i < GetItemCount(); i++)
	{
		LPKGLISTITEM pTempItem = (LPKGLISTITEM)GetItemData(i);
		if (pTempItem == pItem)
		{
			nResult = i;
			break;
		}
	}

Exit0:
	return nResult;
}

int KGListCtrl::InsertDepend(LPKGLISTITEM pParentItem, LPKGLISTITEM pInsertItem)
{
	int nResult  = false;
	int nRetCode = false;

	LPKGLISTITEM pPreItem = m_listDataTree.FindLastVisualChild(pParentItem);
	int nInsertPos  = 0;
	int nColsNumber = GetHeaderCtrl()->GetItemCount();

	KG_PROCESS_ERROR(pInsertItem);

	nRetCode = m_listDataTree.AddDepend(pParentItem, pInsertItem);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = !m_listDataTree.IsVisual(pInsertItem);
	KG_PROCESS_SUCCESS(nRetCode);

	if (pPreItem)
	{
		nInsertPos = FindItemPos(pPreItem) + 1;
	}
	else
	{
		if (pParentItem)
		{
			nInsertPos = FindItemPos(pParentItem) + 1;
		}
		else
		{
			nInsertPos = 0;
		}
	}

	if (nColsNumber > pInsertItem->arryItemText.GetCount())
	{
		nColsNumber = (int)pInsertItem->arryItemText.GetCount();
	}

	InsertItem(nInsertPos, NULL);
	SetItemData(nInsertPos, (DWORD_PTR)pInsertItem);
	RetrievesItemData(pInsertItem);

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KGListCtrl::InsertAfter(LPKGLISTITEM pBeforeItem, LPKGLISTITEM pInsertItme)
{
	int nResult  = false;
	int nRetCode = false;

	LPKGLISTITEM pPreItem = m_listDataTree.FindLastVisualChild(pBeforeItem);
	int nInsertPos  = 0;
	int nColsNumber = GetHeaderCtrl()->GetItemCount();

	KG_PROCESS_ERROR(pBeforeItem);
	KG_PROCESS_ERROR(pInsertItme);

	nRetCode   = !m_listDataTree.IsVisual(pBeforeItem);
	KG_PROCESS_SUCCESS(nRetCode);

	if (
		(pBeforeItem->nItemState) &&
		(pPreItem)
		)
	{
		if (pBeforeItem->pNexSiblingItem)
		{
			nInsertPos = FindItemPos(pBeforeItem->pNexSiblingItem);
		}
		else
		{
			nInsertPos = FindItemPos(pPreItem) + 1;
		}
	}
	else
	{
		nInsertPos = FindItemPos(pBeforeItem) + 1;
	}

	if (nColsNumber > pInsertItme->arryItemText.GetCount())
	{
		nColsNumber = (int)pInsertItme->arryItemText.GetCount();
	}

	nRetCode = m_listDataTree.AddAfter(pBeforeItem, pInsertItme);
	KG_PROCESS_ERROR(nRetCode);
	InsertItem(nInsertPos, NULL);
	SetItemData(nInsertPos, (DWORD_PTR)pInsertItme);
	RetrievesItemData(pInsertItme);

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

BOOL KGListCtrl::Update(LPKGLISTITEM pItem, int nRePaint)
{
	int nItem = FindItemPos(pItem);
	KG_PROCESS_ERROR(nItem != -1);
	RetrievesItemData(pItem);
	if (nItem != -1 && nRePaint)
	{
		return CListCtrl::Update(nItem);
	}
	if (GetParent())
		GetParent()->SendMessage(WM_USER_BACKUP_TAB_FILE);
Exit0:
	return true;
}

BOOL KGListCtrl::Update(int nItem, int nRePaint)
{
	LPKGLISTITEM pItem = (LPKGLISTITEM)GetItemData(nItem);
	RetrievesItemData(pItem);
	if (GetParent())
		GetParent()->SendMessage(WM_USER_BACKUP_TAB_FILE);
	return CListCtrl::Update(nItem);
}

CString KGListCtrl::GetItemText(int nItem, int nSubItem)
{
	CString szText = "";
	if (nItem > -1 && nItem < GetItemCount())
	{
		LPKGLISTITEM pItem = (LPKGLISTITEM)GetItemData(nItem);
		if (
			nSubItem > -1 && 
			nSubItem < pItem->arryItemText.GetCount()
			)
		{
			szText = pItem->arryItemText.GetAt(nSubItem).szItemText;
		}
	}
	return szText;

}

int KGListCtrl::RemoveAll()
{
	DeleteAllItems();
	m_listDataTree.Release();	

	return true;
}

int KGListCtrl::ShrinkItem(LPKGLISTITEM pShrinkItem)
{
	ASSERT(pShrinkItem);
	int nShrinkPos		 = FindItemPos(pShrinkItem) + 1;
	int nShrinkItemLever = pShrinkItem->nTitleLever;
	ASSERT(nShrinkPos);

	while (nShrinkPos < GetItemCount())
	{
		LPKGLISTITEM pTemp = (LPKGLISTITEM)GetItemData(nShrinkPos);
		if (pTemp->nTitleLever > nShrinkItemLever)
			DeleteItem(nShrinkPos);
		else
			break;
	}

	return true;
}

int KGListCtrl::ExpandItem(LPKGLISTITEM pExpandItem, LPKGLISTITEM pPreItem)
{
	int nInsertPos  = 0;
	int nColsNumber = GetHeaderCtrl()->GetItemCount();
	LPKGLISTITEM pTemp = NULL;

	if (pExpandItem)
	{
		nInsertPos = FindItemPos(pPreItem) + 1;
		ASSERT(nInsertPos);
		InsertItem(nInsertPos, NULL);
		SetItemData(nInsertPos, (DWORD_PTR)pExpandItem);
		if (nColsNumber > pExpandItem->arryItemText.GetCount())
		{
			nColsNumber = (int)pExpandItem->arryItemText.GetCount();
		}

		if (pExpandItem->nItemState)
		{
			ExpandItem(pExpandItem->pFirstChildItem, pExpandItem);
		}
		pTemp = m_listDataTree.FindLastVisualChild(pExpandItem);
		if (!pTemp)
			pTemp = pExpandItem;
		ExpandItem(pExpandItem->pNexSiblingItem, pTemp);
	}

	return true;
}

int KGListCtrl::Expand(LPKGLISTITEM pExpandItem)
{
	ASSERT(pExpandItem);
	if (pExpandItem->nItemState)
	{
		ShrinkItem(pExpandItem);
		pExpandItem->nItemState = false;
	}
	else
	{
		ExpandItem(pExpandItem->pFirstChildItem, pExpandItem);
		pExpandItem->nItemState = true;
	}
	Update(FindItemPos(pExpandItem));
	return true;
}

int KGListCtrl::ExpandAll()
{
	int nResult  = false;
	int nRetCode = false;

	int i = 0;
	for (i = 0; i < GetItemCount(); i++)
	{
		LPKGLISTITEM pItem = (LPKGLISTITEM)GetItemData(i);
		if (pItem->arryItemText.GetCount() == 1 && !pItem->nItemState)
		{
			Expand(pItem);
		}
	}

	nResult = true;
	//Exit0:
	return nResult;

}

int KGListCtrl::ShrinkAll()
{
	int nResult  = false;
	int nRetCode = false;

	int i = 0;
	for (i = GetItemCount() - 1; i >= 0; i--)
	{
		LPKGLISTITEM pItem = (LPKGLISTITEM)GetItemData(i);
		if (pItem->arryItemText.GetCount() == 1 && pItem->nItemState)
		{
			Expand(pItem);
		}
	}

	nResult = true;
	//Exit0:
	return nResult;

}

void KGListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CClientDC dc(this);
	CDC* pDC = &dc;
	pDC->SelectObject(GetFont());

	int uRowCount   = CListCtrl::GetHeaderCtrl()->GetItemCount();
	CString* pszText = new CString[uRowCount];
	CRect*   pRect   = new CRect[uRowCount];

	BOOL bIsSelected = FALSE;
	BOOL bIsFocused  = FALSE;
	BOOL bIsDisabled = FALSE;

	COLORREF colorFrame = RGB(255, 255, 255);
	COLORREF colorText  = RGB(255, 255, 255);
	COLORREF colorBack  = RGB(255, 255, 255);
	COLORREF colorExpandFrame = RGB(255, 255, 255);
	COLORREF colorExpandBack  = RGB(255, 255, 255);

	if(lpDrawItemStruct->itemID == -1 || uRowCount < 1) 
	{
		return;
	}

	bIsSelected  = (lpDrawItemStruct->itemState & ODS_SELECTED);
	bIsFocused   = (lpDrawItemStruct->itemState & ODS_FOCUS);
	bIsDisabled  = (lpDrawItemStruct->itemState & ODS_DISABLED);

	CRect rcItem = lpDrawItemStruct->rcItem;

	pDC->SetBkMode(TRANSPARENT);
	LPKGLISTITEM pItem = (LPKGLISTITEM)GetItemData(lpDrawItemStruct->itemID);
	KG_PROCESS_ERROR(pItem);

	int uDrawCound = uRowCount;
	if (uRowCount > (int)pItem->arryItemText.GetCount())
	{
		uDrawCound = (int)pItem->arryItemText.GetCount();
	}

	for (int i = 0; i < uDrawCound; i++)
	{
		pszText[i]  = pItem->arryItemText.GetAt(i).szItemText;
		CListCtrl::GetSubItemRect(
			lpDrawItemStruct->itemID, i, LVIR_BOUNDS, pRect[i]
		);
	}

	if (CString(pszText[0]) == "FK")
	{
		int a = 0;
		a++;
	}

	(*pRect).left += rcItem.Height() * pItem->nTitleLever;
	for (int i = 0; i < uDrawCound ; i++)
	{
		if (!i)
		{
			LPKGLISTITEM pParent = pItem;
			while (pParent->pPerantItem)
				pParent = pParent->pPerantItem;
			int nTitleLever = pParent->nTitleLever;
			while (pParent && pParent->nTitleLever < pItem->nTitleLever)
			{
				CRect rect = rcItem;
				rect.left += rcItem.Height() * pParent->nTitleLever;
				pDC->FillRect(
					&rect, &CBrush(pParent->arryItemText.GetAt(i).colorBack)
					);
				CPen pen(PS_SOLID, 1, pParent->arryItemText.GetAt(i).colorFrame);
				CPen* pPenSave = pDC->SelectObject(&pen);
				pDC->MoveTo(rect.left,  rect.top);
				pDC->LineTo(rect.left,  rect.bottom);
				pDC->SelectObject(pPenSave);
				pParent = FindJunior(pParent);
				if (pItem->pPerantItem && pItem->pPerantItem->nTitleLever == pParent->nTitleLever)
					pParent = pItem->pPerantItem;

				//pParent = pParent->pFirstChildItem;
				//nTitleLever++;
			}
		}
		if (bIsSelected)
		{
			colorExpandFrame = pItem->arryItemText.GetAt(i).colorExpandRectFrameSel;
			colorExpandBack  = pItem->arryItemText.GetAt(i).colorExpandRectBackSel;
			colorBack		 = pItem->arryItemText.GetAt(i).colorBackSel;
			colorFrame		 = pItem->arryItemText.GetAt(i).colorFrameSel;
			colorText		 = pItem->arryItemText.GetAt(i).colorTextSel;
		}
		else 
		{
			colorExpandFrame = pItem->arryItemText.GetAt(i).colorExpandRectFrame;
			colorExpandBack  = pItem->arryItemText.GetAt(i).colorExpandRectBack;
			colorText		 = pItem->arryItemText.GetAt(i).colorText;
			colorFrame		 = pItem->arryItemText.GetAt(i).colorFrame;
			colorBack		 = pItem->arryItemText.GetAt(i).colorBack;
		}

		if (!i && !pItem->pFirstChildItem)
			pRect[i].left += 1;
		pDC->FillRect(pRect[i], &CBrush(colorBack)); 

		CPen pen(PS_SOLID, 1, colorFrame);
		CPen* pPenSave = pDC->SelectObject(&pen);
		pDC->MoveTo(pRect[i].left,  pRect[i].bottom);
		pDC->LineTo(pRect[i].left,  pRect[i].top);
		pDC->LineTo(pRect[i].right, pRect[i].top);
		pDC->SelectObject(pPenSave);

		if (m_listDataTree.IsTitle(pItem))//(uDrawCound == 1)
		{
			CRect rect  = (*pRect);
			rect.left   += 5;
			rect.top    += 3;
			rect.bottom -= 4;
			rect.right = rect.left + (rect.bottom - rect.top);
			rcItem.left = (*pRect).left;
			(*pRect).left += rcItem.Height() + 4;
			DrawRect(
				pDC, &rect, colorExpandFrame, colorExpandBack, !pItem->nItemState
				);
		}
		else
		{
			pRect[i].left += 12;
		}
		pDC->SetTextColor(colorText);
		pDC->DrawText(
			pszText[i], -1, pRect[i], DT_LEFT | DT_SINGLELINE  | DT_VCENTER
			);
	}

Exit0:
	SAFE_DELETE_ARRAY(pRect);
	SAFE_DELETE_ARRAY(pszText);
}


BEGIN_MESSAGE_MAP(KGListCtrl, CListCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_TIMER()
	ON_WM_KEYUP()
END_MESSAGE_MAP()

BOOL KGListCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (m_nToolTipFlag)
		m_toolTipCtrl.RelayEvent(pMsg);
	return CListCtrl::PreTranslateMessage(pMsg);
}

void KGListCtrl::OnPaint()
{
	CListCtrl::OnPaint();

	CClientDC dc(this->GetParent());
	CRect rect;
	GetWindowRect(&rect);

	if (::GetFocus() == this->m_hWnd)
	{
		//dc.Draw3dRect(&rect, RGB(255, 2, 2), RGB(255, 2, 2));
	}
	else
	{
		//dc.Draw3dRect(&rectClient, GetBkColor(), GetBkColor());
	}

}

void KGListCtrl::OnTimer(UINT_PTR nIDEvert)
{
	if (nIDEvert == 1 && m_nToolTipFlag)
	{
		m_toolTipCtrl.Activate(FALSE);
		KillTimer(1);
	}

	return CListCtrl::OnTimer(nIDEvert);
}

void KGListCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CListCtrl::OnSetFocus(pOldWnd);
	//Invalidate(false);
}

void KGListCtrl::OnKillFocus(CWnd* pOldWnd)
{
	CListCtrl::OnSetFocus(pOldWnd);
	//Invalidate(false);
}

void KGListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_nToolTipFlag)
	{
		CString szTipText = _T("");
		CRect	rect;
		SIZE    size;
		int nSubItemCount = 0;
		int nItemPos	  = 0;
		int nCurSubItem	  = 0;
		LPKGLISTITEM lpItem = NULL;
		nItemPos = HitTest(point);
		KG_PROCESS_ERROR(nItemPos != -1);
		nSubItemCount = GetHeaderCtrl()->GetItemCount();
		lpItem = (LPKGLISTITEM)GetItemData(nItemPos);
		KG_PROCESS_ERROR(lpItem);

		ZeroMemory(&size ,sizeof(size));
		if (*(lpItem->szTipText) == '\0')
		{
			for (nCurSubItem = nSubItemCount - 1; nCurSubItem >= 0; nCurSubItem--)
			{
				GetSubItemRect(nItemPos, nCurSubItem, LVIR_BOUNDS, rect);
				if (rect.PtInRect(point))
				{
					szTipText = GetItemText(nItemPos, nCurSubItem);
					CDC* pDC = GetDC();
					if (pDC && pDC->m_hDC)
						GetTextExtentPoint(
						pDC->m_hDC, szTipText.GetBuffer(), szTipText.GetLength(),
						&size
						);
					if (nCurSubItem == nSubItemCount - 1)
					{
						CRect rectClient;
						GetWindowRect(rectClient);
						ScreenToClient(rectClient);
						rect.right = rectClient.right;
						if (size.cx < rect.Width())
						{
							szTipText = _T(""); 
						}
					}
					else
					{
						if (size.cx < GetColumnWidth(nCurSubItem))
						{
							szTipText = _T("");
						}
					}
					break;
				}
			}
		}
		else
		{
			szTipText = lpItem->szTipText;
		}
		m_toolTipCtrl.Activate(TRUE);
		m_toolTipCtrl.UpdateTipText(szTipText.GetBuffer(), this);
		KillTimer(1);
		SetTimer(1, 2000, NULL);
	}

Exit0:
	return CListCtrl::OnMouseMove(nFlags, point);
}

void KGListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nResult  = false;
	int nRetCode = false;

	int   nItem   = HitTest(point);
	CRect rect;
	int   nRigPos = 0;
	KG_PROCESS_ERROR(nItem != -1);
	LPKGLISTITEM pHitItem = (LPKGLISTITEM)GetItemData(nItem);
	KG_PROCESS_ERROR(pHitItem);
	GetItemRect(nItem, &rect, LVIR_BOUNDS);
	nRigPos = rect.left + rect.Height() * (pHitItem->nTitleLever + 1);
	KG_PROCESS_ERROR(point.x < nRigPos);
	nRetCode = Expand(pHitItem);
	KG_PROCESS_ERROR(pHitItem->arryItemText.GetCount() == 1);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return CListCtrl::OnLButtonDown(nFlags, point);
}

void KGListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int nResult  = false;
	int nRetCode = false;

	int nItem  = 0;

	POSITION pos = GetFirstSelectedItemPosition();
	KG_PROCESS_ERROR(pos);
	nItem    = GetNextSelectedItem(pos);
	KG_PROCESS_ERROR(nItem != -1);
	LPKGLISTITEM pExpendIten = (LPKGLISTITEM)GetItemData(nItem);
	KG_PROCESS_ERROR(pExpendIten);
	KG_PROCESS_ERROR(pExpendIten->arryItemText.GetCount() == 1);
	nRetCode = Expand(pExpendIten);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return CListCtrl::OnLButtonDblClk(nFlags, point);
}

void KGListCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int nResult  = false;
	int nRetCode = false;

	POSITION pos = GetFirstSelectedItemPosition();
	int nItem    = GetNextSelectedItem(pos);	
	POINT  point;
	CPoint HitPoint;

	KG_PROCESS_ERROR(nItem != -1);
	::GetCursorPos(&point);
	::ScreenToClient(this->m_hWnd, &point);
	HitPoint.x = point.x;
	HitPoint.y = point.y;
	DWORD dwData = MAKELONG(point.x, point.y);
	if (nChar == 'C' && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
	{
	}
	else if (nChar == 'V' && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
	{
	}
	else if (nChar == VK_RETURN)
	{
		//OnLButtonDblClk(nFlags, HitPoint);
		//GetParent()->SendMessage(WM_LBUTTONDBLCLK, MK_LBUTTON, dwData);
	}
	else if (nChar == VK_DELETE)
	{
	}

	nResult = true;
Exit0:
	return;
}

void KGListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CImageList m_ImageList;
	m_ImageList.Create(1, 16, ILC_COLOR24, 0, 0);
	KGListCtrl::SetImageList(&m_ImageList, LVSIL_SMALL); 
	return CListCtrl::OnSize(nType, cx, cy);
}

int KGListCtrl::SetupToolTip(int nDelayTime, COLORREF colorBack, COLORREF colorText)
{
	int nResult  = false;
	int nRetCode = false;

	KG_PROCESS_ERROR(this);
	if (!m_nToolTipFlag)
	{
		m_toolTipCtrl.Create(this);
		m_toolTipCtrl.AddTool(this);
		m_toolTipCtrl.SetDelayTime(nDelayTime);  
		m_toolTipCtrl.SetTipBkColor(colorBack);
		m_toolTipCtrl.SetTipTextColor(colorText);
		m_nToolTipFlag = true;
	}

	nResult = true;
Exit0:
	return nResult;
}

LPKGLISTITEM KGListCtrl::FindJunior(LPKGLISTITEM pItem)
{
	ASSERT(pItem);
	LPKGLISTITEM pItemReturn = NULL;

	if (pItem->pFirstChildItem)
		pItemReturn = pItem->pFirstChildItem;
	else
	{
		LPKGLISTITEM pParent = pItem->pPerantItem;
		if (!pParent)
			pParent = m_listDataTree.GetHead();
		if (pParent)
		{
			LPKGLISTITEM pSlibing = pParent->pFirstChildItem;
			while (pSlibing)
			{
				if (pSlibing->pFirstChildItem)
				{
					pItemReturn = pSlibing->pFirstChildItem;
					break;
				}
				pSlibing = pSlibing->pNexSiblingItem;
			}
		}
	}
	return pItemReturn;
}