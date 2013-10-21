////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSceneHelpers.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-2-13 10:26:05
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "KSceneSceneHelpers.h"
#include "KGHelperClasses.h"
#include "IEKG3DScene.h"
#include "IEKG3DEnvironment.h"
#include "SceneEditor.h"
#include "KG3DInterface.h"
#include "IEEditor.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////////
namespace KSCENEEDITOR_HELPERCLASSES
{
	BOOL CALLBACK MyEnableWindow( HWND hWnd, LPARAM lPARAM)	///自定义回调函数，用于控件遍历，禁止控件
	{
		ENABLE_WINDOW_PARAM* pTempParam = reinterpret_cast<ENABLE_WINDOW_PARAM*>(lPARAM);
		if (!pTempParam)
		{
			return FALSE;
		}
		int ID = GetDlgCtrlID( hWnd );
		if (pTempParam->m_npJumpList)
		{
			for (INT i = 0; i < pTempParam->m_nIDCount; i++)
			{
				if (ID == pTempParam->m_npJumpList[i])
				{
					return TRUE;///和排除列表中的相同，排除掉，继续找下去
				}
			}
		}
		::EnableWindow( hWnd, pTempParam->m_bEnable);
		return TRUE;
	}

	BOOL LockChildWindows( HWND hWnd, BOOL bLock, INT* pJumpIDList, INT nIDCount )
	{
		ENABLE_WINDOW_PARAM tempParam;
		tempParam.m_bEnable = !bLock;
		tempParam.m_npJumpList = pJumpIDList;
		tempParam.m_nIDCount = nIDCount;
		return ::EnumChildWindows(hWnd, MyEnableWindow, reinterpret_cast<LPARAM>(&tempParam));
	}
	BOOL SynACButtonWithABool( CButton& cbButton, BOOL& bRetAndDefault, BOOL bToButtonOrToValue/* = TRUE*/ )
	{
		if (bToButtonOrToValue)///设定Button
		{
			cbButton.SetCheck(bRetAndDefault ? BST_CHECKED : BST_UNCHECKED);
			return bRetAndDefault;
		}
		int nRet = cbButton.GetCheck();
		bRetAndDefault = (nRet == BST_CHECKED)?TRUE : FALSE;
		return bRetAndDefault;
	}

	BOOL SynACButtonWithABool( CButton& cbButton, unsigned uRetAndDefault, BOOL bToButtonOrToValue/* = TRUE*/ )
	{
		BOOL bValue = uRetAndDefault == 0? FALSE : TRUE;
		return SynACButtonWithABool(cbButton, bValue, bToButtonOrToValue);
	}
	BOOL SynACheckWndWithABool( CWnd* pWnd, BOOL& bRetAndDefault, BOOL bToButtonOrToValue/* = TRUE*/ )
	{
		KG_PROCESS_ERROR(pWnd);
		if (bToButtonOrToValue)///设定Button
		{
			::SendMessage(pWnd->GetSafeHwnd(), BM_SETCHECK, bRetAndDefault == TRUE ? BST_CHECKED : BST_UNCHECKED, 0);
			return bRetAndDefault;
		}
		int nRet = (int)::SendMessage(pWnd->GetSafeHwnd(), BM_GETCHECK, 0, 0);
		bRetAndDefault = (nRet == BST_CHECKED)?TRUE : FALSE;
Exit0:
		return bRetAndDefault;
	}

	BOOL SynACheckWndWithABool( CWnd* pWnd, unsigned uRetAndDefault, BOOL bToButtonOrToValue /*= TRUE*/ )
	{
		BOOL bValue = uRetAndDefault == 0? FALSE : TRUE;
		return SynACheckWndWithABool(pWnd, bValue, bToButtonOrToValue);
	}
	LPCTSTR GetRTTITypeName( DWORD dwType )
	{
		static TCHAR tczRTTIName[IKG3DRTTITypeDescStruct::MAX_RTTI_NAME_LENGTH];
		IKG3DRTTIType* pType = NULL;
		KG_COM_PROCESS_ERROR(g_pKG3DRTTITypeManager->GetRTTIType(dwType, &pType));
		KG_PROCESS_ERROR(pType);
		KG_COM_PROCESS_ERROR(pType->GetName(tczRTTIName, _countof(tczRTTIName)));
		tczRTTIName[_countof(tczRTTIName) - 1] = '\0';///强制加上一个结束
		return tczRTTIName;
Exit0:
		return NULL;
	}
	CString GetRTTITypeName(const KG3DENVEFF_HANDLE& EnvEffHandle )
	{
		CString TempString(_T("")); 
		LPCTSTR tczName = GetRTTITypeName(EnvEffHandle.m_dwType);
		KG_PROCESS_ERROR(tczName);
		TempString.Format(_T("%s_%d"), tczName, EnvEffHandle.m_nIndex);
Exit0:
		return TempString;
	}
	
	
	//////////////////////////////////////////////////////////////////////////
	void CWndDestroyer(CWnd* pCWnd)
	{
		_ASSERTE(pCWnd);
		pCWnd->DestroyWindow();
	}	
	//////////////////////////////////////////////////////////////////////////
	VOID CWndReSizeClient( CWnd* pCWnd, INT nNewWidth, INT nNewHeight, BOOL bNeedRePaint /*= FALSE*/ )
	{
		KG_PROCESS_ERROR(pCWnd && nNewWidth > 0 && nNewHeight > 0 && nNewWidth <= 1024 && nNewHeight <= 800);
		{
			CRect TempRect;
			CRect TempClientRect;
			pCWnd->GetWindowRect(&TempRect);
			pCWnd->GetClientRect(&TempClientRect);
			INT nWidthDiff = nNewWidth - TempClientRect.Width();
			INT nHeightDiff = nNewHeight - TempClientRect.Height();
			TempRect.bottom += nHeightDiff;
			TempRect.right += nWidthDiff;
			pCWnd->MoveWindow(&TempRect, bNeedRePaint);
		}
Exit0:
		return;
	}

	VOID CWndMoveY( CWnd* pCWnd, INT nNewY, BOOL bIsClientY /*= FALSE*/, CWnd* pOwnerWin /*= NULL*/, BOOL bNeedRePaint /*= FALSE*/ )
	{
		KG_PROCESS_ERROR(pCWnd && nNewY > 0 && nNewY <= 800);
		{
			CRect TempRect;
			pCWnd->GetWindowRect(&TempRect);
			if (bIsClientY)
			{
				KG_PROCESS_ERROR(pOwnerWin);
				pOwnerWin->ScreenToClient(&TempRect);
			}
			TempRect.MoveToY(nNewY);
			pCWnd->MoveWindow(&TempRect, bNeedRePaint);
		}
Exit0:
		return;
	}
	INT	CWndGetWindowWidth(CWnd* pCWnd)
	{
		_ASSERTE(pCWnd);
		CRect TempRect;
		pCWnd->GetWindowRect(&TempRect);
		return TempRect.Width();
	}
	INT CWndGetWindowHeight(CWnd* pCWnd)
	{
		_ASSERTE(pCWnd);
		CRect TempRect;
		pCWnd->GetWindowRect(&TempRect);
		return TempRect.Height();
	}
	CSize CWndGetWindowSize(CWnd* pCWnd)
	{
		_ASSERTE(pCWnd);
		CRect TempRect;
		pCWnd->GetWindowRect(&TempRect);
		CSize TempSize;
		TempSize.SetSize(TempRect.Width(), TempRect.Height());
		return TempSize;
	}

	BOOL CTreeCtrlCopyAllChildItems( CTreeCtrl& Tree, HTREEITEM hSrc, HTREEITEM hDec )
	{
		INT nChildCount = CTreeCtrlGetChildItemCount(Tree, hSrc);
		KG_PROCESS_SUCCESS(nChildCount <= 0);
		{
			TCHAR TempBuffer[64];
			HTREEITEM hChildItem = Tree.GetChildItem(hSrc);
			///清空hDes
			
			INT nInserted = 0;
			CTreeCtrlDeleteAllChildItems(Tree, hDec);
			while (hChildItem && nInserted < nChildCount)
			{
				TVITEM TempItem;
				TempItem.hItem = hChildItem;
				TempItem.pszText = TempBuffer;
				TempItem.cchTextMax = _countof(TempBuffer);
				TempItem.mask =  TVIF_HANDLE|TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_STATE|TVIF_TEXT;///没有Child
				BOOL bRet = Tree.GetItem(&TempItem);
				if (bRet)
				{
					TVINSERTSTRUCT InsertStruct;
					InsertStruct.hParent = hDec;
					InsertStruct.hInsertAfter = TVI_LAST;
					TempItem.mask &= ~TVIF_HANDLE;
					InsertStruct.item = TempItem;
					HTREEITEM hRetItem = Tree.InsertItem(&InsertStruct);
					if (hRetItem)
					{
						CTreeCtrlCopyAllChildItems(Tree, hChildItem, hRetItem);
					}
				}
				hChildItem = Tree.GetNextSiblingItem(hChildItem);
				nInserted++;
			}
		}
Exit1:
		return TRUE;
/*
Exit0:
		return FALSE;*/

	}

	VOID CTreeCtrlDeleteAllChildItems( CTreeCtrl& Tree, HTREEITEM hDes )
	{
		INT nChildCount = CTreeCtrlGetChildItemCount(Tree, hDes);
		if (nChildCount > 0)
		{
			HTREEITEM hItem = Tree.GetChildItem(hDes);
			int nDelCount = 0;
			while (hItem && nDelCount <= nChildCount)
			{
				Tree.DeleteItem(hItem);
				hItem = Tree.GetChildItem(hDes);
				nDelCount++;
			}
		}
	}
	INT CTreeCtrlGetChildItemCount(CTreeCtrl& Tree, HTREEITEM hDes)
	{
		if (Tree.ItemHasChildren(hDes))
		{
			HTREEITEM hChild = Tree.GetChildItem(hDes);
			INT nCount = 0;
			while (hChild)
			{
				nCount++;
				hChild = Tree.GetNextSiblingItem(hChild);
			}
			return nCount;
		}
		return 0;
	}

	HTREEITEM CTreeCtrlInsertItem( CTreeCtrl& Tree
							, CString Text
							, HTREEITEM hParent /*= TVI_ROOT  */
							, HTREEITEM hInsertAfter /*= TVI_LAST  */
							, LPARAM* plParam /*= NULL  */
							, INT nImage /*= -1  */
							, INT nSelectedImage /*= -1  */
							, UINT* puState /*= NULL  */
							, UINT uStateMask /*= 0  */ )
	{
		TVITEM TempItem;
		CString TempString = Text;
		;
		UINT uElementMask = TVIF_TEXT;
		TempItem.pszText = TempString.GetBuffer();
		if(plParam)
		{
			TempItem.lParam = *plParam;
			uElementMask |= TVIF_PARAM;
		}
		if (nImage >= 0)
		{
			TempItem.iImage = nImage;
			uElementMask |= TVIF_IMAGE;
		}
		else
		{
			TempItem.iImage = 0;
		}
		if (nSelectedImage >= 0)
		{
			TempItem.iSelectedImage = nSelectedImage;
			uElementMask |= TVIF_SELECTEDIMAGE;
		}
		else
		{
			TempItem.iSelectedImage = 0;
		}
		if (puState)
		{
			TempItem.state = *puState;
			TempItem.stateMask = uStateMask;
			uElementMask |= TVIF_STATE;
		}
		TempItem.mask = uElementMask;
		
		TVINSERTSTRUCT InsertStruct;
		InsertStruct.hInsertAfter = hInsertAfter;
		InsertStruct.hParent = hParent;
		InsertStruct.item = TempItem;

		HTREEITEM hTreeItemRet = Tree.InsertItem(&InsertStruct);
		KG_PROCESS_ERROR(hTreeItemRet);
		TempString.ReleaseBuffer();
		return hTreeItemRet;
Exit0:
		TempString.ReleaseBuffer();
		return FALSE;
	}

	BOOL SynWndWithText( CWnd* pED, LPTSTR lpStr, BOOL bToWndOrToValue /*= TRUE*/,  UINT uBufferSize /*= 0 */)
	{
		KG_PROCESS_ERROR(pED);
		if (bToWndOrToValue)
		{
			pED->SetWindowText(lpStr);
		}
		else
		{
			KG_PROCESS_ERROR(uBufferSize > (UINT)pED->GetWindowTextLength());//GetWindowTextLength无NULL结尾
			pED->GetWindowText(lpStr, uBufferSize);
		}
		return TRUE;
Exit0:
		return FALSE;
	}

	BOOL SynWndWithText( CWnd* pED, CString& Cs, BOOL bToWndOrToValue /*= TRUE*/ )
	{
		KG_PROCESS_ERROR(pED);
		if (bToWndOrToValue)
		{
			pED->SetWindowText(Cs);
		}
		else
		{
			pED->GetWindowText(Cs);
		}
		return TRUE;
Exit0:
		return FALSE;
	}

	BOOL CTreeCtrlIsHandleValid(CTreeCtrl& Tree, HTREEITEM hItem )
	{
		if(reinterpret_cast<LONG_PTR>(hItem) <= 0)	//负数有可能无来由的在MFC的内部实现中当掉，所以不允许
			return FALSE;
		TVITEM stItem;
		stItem.mask = TVIF_HANDLE;
		stItem.hItem = hItem;
		return Tree.GetItem(&stItem);
	}

	VOID CTreeCtrlTraverseDepthFirst(CTreeCtrl& Tree, CTreeCtrlVisitor& Visitor )
	{
		HTREEITEM hParentItem = TVI_ROOT;
		HTREEITEM hCurItem = Tree.GetChildItem(hParentItem);
		while (hCurItem)
		{
			Visitor.Accept(Tree, hCurItem);

			//不断深度遍历
			HTREEITEM hChildItem = Tree.GetChildItem(hCurItem);

			if(hChildItem)
			{
				hParentItem = hCurItem;
				hCurItem = hChildItem;
			}			
			else	//向旁支查找，如旁支没有，找父亲的旁支
			{
				while (hParentItem)
				{
					hCurItem = Tree.GetNextSiblingItem(hCurItem);
					if (hCurItem)
						break;
					if (hParentItem == TVI_ROOT)//到头了，如果还找不到Sibling，那么可以退出了
						break;
					hCurItem = hParentItem;
					hParentItem = Tree.GetParentItem(hCurItem);
					if (! hParentItem)
					{
						/*
						MS的设计真是让人费解，如果用TVI_ROOT来GetChildItem可以得到第一级子节点，那么
						为什么第一级节点反过来GetParent又不能得到TVI_ROOT。MS可能是为了速度把东西搞甚复杂了
						*/
						hParentItem = TVI_ROOT;
					}
				}
			}
		}
	}

	VOID CtreeCtrlSelectByOrder(CTreeCtrl& Tree, UINT Order )
	{
		UINT uCount = Tree.GetCount();
		if(Order >= uCount)
			return;

		HTREEITEM hItem = Tree.GetRootItem();
		for(UINT i = 0; i < Order; ++i)
		{
			if(! hItem)
				return;
			hItem = Tree.GetNextSiblingItem(hItem);
		}
		Tree.SelectItem(hItem);
	}
	
	
	
};///namespace KSCENEEDITOR_HELPERCLASSES
