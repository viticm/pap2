////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSceneTreeCtrl.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-2-21 16:11:32
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KSceneSceneTreeCtrl.h"
#include "KSceneSceneHelpers.h"
#include <math.h>

class KSceneCustomScrollBar;

IMPLEMENT_DYNAMIC(KSceneSceneTreeCtrl, CTreeCtrl)

KSceneSceneTreeCtrl::KSceneSceneTreeCtrl()
:m_pScrollBarVert(NULL)
,m_pEventHandler(NULL)
{
	//ZeroMemory(m_FuncCallbackArray, _countof(m_FuncCallbackArray)*sizeof(LPVOID));
	//ZeroMemory(m_CallbackParamArray, _countof(m_CallbackParamArray)*sizeof(DWORD_PTR));
}

KSceneSceneTreeCtrl::~KSceneSceneTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(KSceneSceneTreeCtrl, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_SELCHANGING, &KSceneSceneTreeCtrl::OnTvnSelchanging)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, &KSceneSceneTreeCtrl::OnTvnItemexpanding)
	ON_WM_VSCROLL()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, &KSceneSceneTreeCtrl::OnTvnItemexpanded)
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, &KSceneSceneTreeCtrl::OnTvnDeleteitem)
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &KSceneSceneTreeCtrl::OnTvnSelchanged)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

HRESULT KSceneSceneTreeCtrl::SetEventHandler( KSceneSceneTreeCtrlEventHandler* pEventHandler )
{
	m_pEventHandler = pEventHandler;
	return S_OK;
}
BOOL KSceneSceneTreeCtrl::IsItemHandleValid( HTREEITEM hItem )
{
	return KSH::CTreeCtrlIsHandleValid(*this, hItem);
}
VOID KSceneSceneTreeCtrl::SynScrollBar()
{
	//this->EnableScrollBar(SB_VERT, ESB_DISABLE_BOTH);
	if(!m_pScrollBarVert)
		return;
	SCROLLINFO tempScrollInfo;
	this->GetScrollInfo(SB_VERT, &tempScrollInfo, SIF_ALL);
	m_pScrollBarVert->SetScrollInfo(&tempScrollInfo, TRUE);


	INT nRange = tempScrollInfo.nMax - tempScrollInfo.nMin - tempScrollInfo.nPage + 1;
	if ((UINT)abs(nRange) > 0)
	{
		m_pScrollBarVert->EnableWindow(TRUE);
	}
	else
	{
		m_pScrollBarVert->EnableWindow(FALSE);
	}
	this->ShowScrollBar(SB_VERT, FALSE);
	this->ShowScrollBar(SB_HORZ, FALSE);
}

BOOL KSceneSceneTreeCtrl::BindVScrollBar( KSceneCustomScrollBar* pScrollBarOutter )
{
	CScrollBar* pScrollBar = (CScrollBar*)pScrollBarOutter;
	if (pScrollBar && !m_pScrollBarVert)
	{
		m_pScrollBarVert = pScrollBar;

		SCROLLINFO tempScrollInfo;
		this->GetScrollInfo(SB_VERT, &tempScrollInfo, SIF_ALL);
		pScrollBar->SetScrollInfo(&tempScrollInfo, TRUE);
		return TRUE;
	}
	else if (!pScrollBar && m_pScrollBarVert)
	{
		m_pScrollBarVert = NULL;
		return TRUE;
	}
	return FALSE;
}

// KSceneSceneTreeCtrl message handlers

void KSceneSceneTreeCtrl::OnTvnSelchanging(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
void KSceneSceneTreeCtrl::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hLastSeletedItem = pNMTreeView->itemOld.hItem;
	HTREEITEM hSelectedItem = pNMTreeView->itemNew.hItem;
	//CallbackCall<CALLBACK_ITEM_SELCHANGED>(hSelectedItem, (DWORD_PTR)hLastSeletedItem);
	if(NULL != m_pEventHandler)
	{
		m_pEventHandler->OnItemSelectChange(*this, hSelectedItem, hLastSeletedItem);
	}
	*pResult = 0;
}
void KSceneSceneTreeCtrl::OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	//SynScrollBar();

	//CallbackCall<CALLBACK_ITEM_EXPANDING>(pNMTreeView->itemNew.hItem, NULL);
	if(NULL != m_pEventHandler)
	{
		m_pEventHandler->OnItemExpanding(*this, pNMTreeView->itemNew.hItem);
	}

	*pResult = 0;
}
void KSceneSceneTreeCtrl::OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here]
	HTREEITEM hItemExpanded = pNMTreeView->itemNew.hItem;

	///不允许两层展开	

	{
		//意思是禁止发送Expand事件
		//TypeCallbackFrameWork::CallbackStoper<CALLBACK_ITEM_EXPAND> StopCallback(this);
		KSceneSceneTreeCtrlEventHandler* pEventHandlerSaved = m_pEventHandler;
		m_pEventHandler = NULL;

		HTREEITEM hChild = this->GetChildItem(hItemExpanded);
		while (hChild)
		{
			//_KG3D_DEBUG_OUTPUT1("%s", this->GetItemText(hChild));
			this->Expand(hChild, TVE_COLLAPSE);
			hChild = this->GetChildItem(hChild);
		}

		m_pEventHandler = pEventHandlerSaved;
	}


	//CallbackCall<CALLBACK_ITEM_EXPAND>(hItemExpanded, NULL);

	if (NULL != m_pEventHandler)
	{
		m_pEventHandler->OnItemExpanded(*this, hItemExpanded);
	}

	SynScrollBar();
	*pResult = 0;
}
void KSceneSceneTreeCtrl::OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	//CallbackCall<CALLBACK_ITEM_DELETING>(pNMTreeView->itemOld.hItem, NULL);
	if (NULL != m_pEventHandler)
	{
		m_pEventHandler->OnItemDelete(*this, pNMTreeView->itemOld.hItem);
	}
	SynScrollBar();

	*pResult = 0;
}

void KSceneSceneTreeCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	CTreeCtrl::OnVScroll(nSBCode, nPos, NULL);
	if (pScrollBar != m_pScrollBarVert)
	{
		SynScrollBar();
	}
}

LRESULT KSceneSceneTreeCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	LRESULT lResult= CTreeCtrl::WindowProc(message, wParam, lParam);
	switch(message)
	{

	case TVM_INSERTITEM:
		{
			SynScrollBar();
			if (lResult)///失败的话是NULL
			{
				//CallbackCall<CALLBACK_ITEM_INSERT>((HTREEITEM)lResult, NULL);
				if (NULL != m_pEventHandler)
				{
					m_pEventHandler->OnItemInserted(*this, (HTREEITEM)lResult);
				}
			}
		}
		break;		
	}
	return lResult;
}

HRESULT KSceneSceneTreeCtrl::TraverseItems(HTREEITEM hItem, TreeItemVisitor& visitor)
{
	return EnumChildItems(hItem, visitor);
}

//BOOL KSceneSceneTreeCtrl::TraverseItemsOnce( TypeFuncCallback Func, DWORD_PTR dwParam, HTREEITEM hItem /*= TVI_ROOT*/ )
//{
//	TypeFuncCallback OldCallback;
//	DWORD_PTR OldParam;
//	BOOL bRet = this->CallbackSetFuncAndParam<CALLBACK_ITEM_ENUM>(Func, dwParam, &OldCallback, &OldParam);
//	if(! bRet)
//	return FALSE;

//	bRet = TraverseItems(hItem);

//	this->CallbackSetFuncAndParam<CALLBACK_ITEM_ENUM>(OldCallback, OldParam);
//	return bRet;

//}
HRESULT KSceneSceneTreeCtrl::TraverseItemsExpanded( HTREEITEM hItem, TreeItemVisitor& visitor )
{
	return EnumChildExpandedItems(hItem, visitor);
}
//HRESULT KSceneSceneTreeCtrl::TraverseItemsExpandedOnce(HTREEITEM hItem /*= TVI_ROOT*/ )
//{
//	/*TypeFuncCallback OldCallback;
//	DWORD_PTR OldParam;
//	
//	BOOL bRet = this->CallbackSetFunc<CALLBACK_ITEM_ENUM>(Func, &OldCallback);
//	this->CallbackSetParam<CALLBACK_ITEM_ENUM>(dwParam, &OldParam);
//	_ASSERTE(bRet);
//	_ASSERTE(CallbackGet<CALLBACK_ITEM_ENUM>() == Func);*/
//	
//	HRESULT hr = TraverseItemsExpanded(hItem);
//	
//	/*this->CallbackSetFunc<CALLBACK_ITEM_ENUM>(OldCallback);
//	this->CallbackSetParam<CALLBACK_ITEM_ENUM>(OldParam);*/
//	return hr ;
//}
HRESULT KSceneSceneTreeCtrl::EnumChildItems(HTREEITEM hItem, TreeItemVisitor& visitor)
{
	//_ASSERTE(nLevel <= ENUM_LIMIT_MAX_LEVEL);
	HRESULT hr = E_FAIL;
	for(HTREEITEM hChild = this->GetChildItem(hItem)
		; hChild
		; hChild = this->GetNextSiblingItem(hChild))
	{
		/*nEnumCounted++;
		TreeEnumCallBackStruct TempCallBackStruct;
		TempCallBackStruct.m_nEnumCounted = nEnumCounted;
		TempCallBackStruct.m_nLevel = nLevel;

		DWORD_PTR dwRet = CallbackCall<CALLBACK_ITEM_ENUM>(hChild, (DWORD_PTR)&TempCallBackStruct);
		if (!dwRet)
		return FALSE;		

		if (!EnumChildItem(hChild, nLevel+1, nEnumCounted))
		return FALSE;*/

		hr = visitor.Accept(hChild);
		if(FAILED(hr))
			return hr;

		hr = EnumChildItems(hChild, visitor);
		if (FAILED(hr))
			return hr;
	}
	return hr;	
}
BOOL KSceneSceneTreeCtrl::IsItemExpanded( HTREEITEM hItem )
{
	UINT nExpanded = this->GetItemState(hItem, TVIS_EXPANDED);
	if (nExpanded & TVIS_EXPANDED)//!= TVIS_EXPANDED
		return TRUE;
	return FALSE;
}
HRESULT KSceneSceneTreeCtrl::EnumChildExpandedItems( HTREEITEM hItem, TreeItemVisitor& visitor)
{
	HRESULT hr = E_FAIL;

	//_ASSERTE(nLevel <= ENUM_LIMIT_MAX_LEVEL);
	for(HTREEITEM hChild = this->GetChildItem(hItem)
		; hChild
		; hChild = this->GetNextSiblingItem(hChild))
	{	
		//nEnumCounted++;
		//TreeEnumCallBackStruct TempCallBackStruct;
		//TempCallBackStruct.m_nEnumCounted = nEnumCounted;
		//TempCallBackStruct.m_nLevel = nLevel;

		//DWORD_PTR dwRet = CallbackCall<CALLBACK_ITEM_ENUM>(hChild, (DWORD_PTR)&TempCallBackStruct);

		//if (!dwRet)
		//	return FALSE;

		//if (!(IsItemExpanded(hChild)))//!= TVIS_EXPANDED
		//	continue;

		//if (!EnumChildExpandedItem(hChild, nLevel+1, nEnumCounted))
		//	return FALSE;

		if(! IsItemExpanded(hChild))
			continue;

		hr = visitor.Accept(hChild);
		if (FAILED(hr))
			return hr;

		hr = EnumChildExpandedItems(hChild, visitor);
		if(FAILED(hr))
			return hr;
	}
	return hr;
}
/*DWORD_PTR CALLBACK KSceneSceneTreeCtrl::EnumExpandedCountCallback(KSceneSceneTreeCtrl* pTreeCtrl
, HTREEITEM hItem
, DWORD_PTR pTreeParam
, DWORD_PTR pExtraParam)
{
_ASSERTE(hItem);
*(INT*)pExtraParam += 1;
_ASSERTE(((TreeEnumCallBackStruct*)pTreeParam)->m_nEnumCounted == *(INT*)pExtraParam);
return TRUE;
}*/
/*DWORD_PTR CALLBACK KSceneSceneTreeCtrl::EnumExpandedItemPosCallback( KSceneSceneTreeCtrl* pTreeCtrl, HTREEITEM hItem, DWORD_PTR pTreeEnumCallBackStruct , DWORD_PTR pExtraParam )
{
struct TypeTemp 
{
HTREEITEM hItemToCompare;
INT m_nPos;
BOOL m_bRet;
};
TypeTemp* pRealParam = (TypeTemp*)pExtraParam;
_ASSERTE(hItem && pRealParam);
pRealParam->m_nPos += 1;
_ASSERTE(((TreeEnumCallBackStruct*)pTreeEnumCallBackStruct)->m_nEnumCounted == pRealParam->m_nPos);
if (hItem == pRealParam->hItemToCompare)
{
pRealParam->m_bRet = TRUE;
return FALSE;
}
return TRUE;
}*/
INT KSceneSceneTreeCtrl::GetItemExpandedPos( HTREEITEM hItem )
{
	struct TypeTemp : public TreeItemVisitor
	{
		HTREEITEM m_hItemToCompare;
		INT m_nPos;
		BOOL m_bIsItemToComparedExpanded;

		virtual HRESULT Accept(HTREEITEM hItem)
		{
			++m_nPos;
			if (hItem == m_hItemToCompare)
			{	
				m_bIsItemToComparedExpanded = TRUE;
				return E_FAIL;
			}
			return S_OK;
		}
	};

	TypeTemp visitor;
	visitor.m_nPos = 0;
	visitor.m_hItemToCompare = hItem;
	visitor.m_bIsItemToComparedExpanded = FALSE;

	this->TraverseItemsExpanded(TVI_ROOT, visitor);
	if (visitor.m_bIsItemToComparedExpanded)
	{
		return visitor.m_nPos;
	}
	return -1;
}
INT KSceneSceneTreeCtrl::GetExpandedTreeItemCount(HTREEITEM hItem /*= TVI_ROOT*/)
{
	/*INT nCount = 0;
	if (!TraverseItemsExpandedOnce(&EnumExpandedCountCallback, (DWORD_PTR)&nCount, hItem))
	{
	return -1;
	}
	return nCount;*/

	struct TypeVisitor : public TreeItemVisitor 
	{
		UINT m_uCount;
		virtual HRESULT Accept(HTREEITEM Item)
		{
			++m_uCount;
			return S_OK;
		}
	};
	TypeVisitor visitor;
	visitor.m_uCount = 0;

	TraverseItemsExpanded(hItem, visitor);;

	return visitor.m_uCount;
}

HTREEITEM KSceneSceneTreeCtrl::GetRootItemOfItem( HTREEITEM hItem )
{
	HTREEITEM hCurItem = hItem;
	HTREEITEM hParentItem = GetParentItem(hCurItem);
	while (hParentItem)
	{
		hCurItem = hParentItem;
		hParentItem = GetParentItem(hCurItem);
	}
	return hCurItem;
}

INT KSceneSceneTreeCtrl::GetChildItemCount( HTREEITEM hItem )
{
	return KSH::CTreeCtrlGetChildItemCount(*this, hItem);
}

BOOL KSceneSceneTreeCtrl::CopyAllChildItems( HTREEITEM hSrc, HTREEITEM hDes )
{
	return KSH::CTreeCtrlCopyAllChildItems(*this, hSrc, hDes);
}


void KSceneSceneTreeCtrl::OnDestroy()
{
	///Destroy之前退掉所有的Callback绑定，省的出错
	/*
	ZeroMemory(m_FuncCallbackArray, _countof(m_FuncCallbackArray)*sizeof(LPVOID));
	ZeroMemory(m_CallbackParamArray, _countof(m_CallbackParamArray)*sizeof(DWORD_PTR));*/
	//CallbackUnBindAll();

	m_pEventHandler = NULL;

	CTreeCtrl::OnDestroy();

	// TODO: Add your message handler code here
}



BOOL KSceneSceneTreeCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	BOOL bRet = CTreeCtrl::OnMouseWheel(nFlags, zDelta, pt);
	if (m_pScrollBarVert)
	{
		INT nDelta = (INT)zDelta / WHEEL_DELTA;
		SCROLLINFO SI;
		SI.cbSize = sizeof(SCROLLINFO);
		SI.fMask = SIF_ALL;
		this->GetScrollInfo(SB_VERT, &SI);
		INT nRange = SI.nMax - SI.nMin - SI.nPage + 1;
		INT nPos = SI.nPos - nDelta;
		MT_LIMIT_TO_RANGE(nPos, nRange, 0);
		this->PostMessage(WM_VSCROLL, MAKELONG(SB_THUMBTRACK, nPos), NULL);
	}
	return bRet;
}

HTREEITEM KSceneSceneTreeCtrl::InsertItemEx(CString Text
											, HTREEITEM hParent /*= TVI_ROOT  */
											, HTREEITEM hInsertAfter /*= TVI_LAST  */
											, LPARAM* plParam /*= NULL  */
											, INT nImage /*= -1  */
											, INT nSelectedImage /*= -1  */
											, UINT* puState /*= NULL  */
											, UINT uStateMask /*= 0  */ )
{
	return KSH::CTreeCtrlInsertItem(*this, Text, hParent, hInsertAfter,plParam, nImage, nSelectedImage, puState, uStateMask);
}

HTREEITEM KSceneSceneTreeCtrl::GetNextSiblingOrUpperSiblingItem( HTREEITEM hItem )
{
	HTREEITEM hSiblingItem = GetNextSiblingItem(hItem);
	while(!hSiblingItem)///如果是最后一个Item，跳到父亲再向下找
	{
		HTREEITEM hParent = GetParentItem(hItem);
		if (!hParent)
		{
			break;
		}
		hSiblingItem = GetNextSiblingItem(hParent);
	}
	return hSiblingItem;
}



////////////////////////////////////////////////////////////////////////////////

