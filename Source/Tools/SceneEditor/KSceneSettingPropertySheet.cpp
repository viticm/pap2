// KSceneSettingPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSettingPropertySheet.h"

//#include "KGHelperClasses.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// KSceneSettingPropertySheet

IMPLEMENT_DYNAMIC(KSceneSettingPropertySheet, CPropertySheet)

//#define WM_DWGC_CHANGE_TO_PAGE	(WM_USER + 401)		///WPARAM = Page序列号, LPARAM=是否锁定
/*
KSceneSettingPropertySheet::KSceneSettingPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
	,m_bLockChange(FALSE)
{
	
}

KSceneSettingPropertySheet::KSceneSettingPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
	,m_bLockChange(FALSE)
{
	
}

KSceneSettingPropertySheet::KSceneSettingPropertySheet()
	:m_bLockChange(FALSE)
{
	
}
KSceneSettingPropertySheet::~KSceneSettingPropertySheet()
{
}*/



BEGIN_MESSAGE_MAP(KSceneSettingPropertySheet, CPropertySheet)
	//ON_MESSAGE(WM_DWGC_TO_KEY_TYPE_PAGE, OnMsgToKeyTypePage)
	//ON_MESSAGE(WM_DWGC_CHANGE_TO_PAGE, OnMsgChangeToPage)
	//ON_MESSAGE(WM_DWGC_LOCK_TAB, OnMsgLockPage)
	//ON_MESSAGE(WM_IS_REGISTER_PAGE, OnMsgIsRegisterPage)
END_MESSAGE_MAP()

/*
HRESULT KSceneSettingPropertySheet::AddIndexPage( DWORD dwType, CPropertyPage* pPage )
{
	KG_PROCESS_ERROR(pPage);
	{
		int nPageCount = GetPageCount();
		AddPage(pPage);
		KG_PROCESS_ERROR(nPageCount+1 == GetPageCount());
		int nIndex = GetPageIndex(pPage);
		INDEX_MAP::_Pairib pr = m_IndexMap.insert(INDEX_MAP::value_type(dwType, nIndex));
		KG_PROCESS_ERROR(pr.second);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}*/



/*
LRESULT KSceneSettingPropertySheet::OnMsgToKeyTypePage( WPARAM wParam, LPARAM lParam )
{
	StructChangePage* pStruct = (StructChangePage*)wParam;
	_ASSERTE(pStruct && pStruct->IsValidInMemory());
	INDEX_MAP::iterator it = m_IndexMap.find(pStruct->m_EnvEffectHandle.m_dwType);
	KG_PROCESS_ERROR(it != m_IndexMap.end());
	///把设Key状态改在这里做//<TASK>
	m_StructChangePage = *pStruct;
	this->PostMessage(WM_DWGC_CHANGE_TO_PAGE, it->second, lParam);
	return TRUE;
Exit0:
	return FALSE;
}*/


/*
LRESULT KSceneSettingPropertySheet::OnMsgChangeToPage( WPARAM wParam, LPARAM lParam )
{
	INT nPageIndex = _kg_to_smaller_size_cast<INT>(wParam);
	_KG3D_DEBUG_SELF_ASSERTE(nPageIndex >= 0 && nPageIndex < this->GetPageCount());
	KG_PROCESS_ERROR(SetActivePage(nPageIndex));
	this->SendMessage(WM_DWGC_LOCK_TAB, lParam == 0 ? FALSE : TRUE, 0);
	return TRUE;
Exit0:
	_KG3D_DEBUG_SELF_ASSERTE(NULL && "KSceneSettingPropertySheet::OnMsgChangeToPage失败\n");
	return FALSE;
}*/


/*
KSceneSettingPropertySheet::StructChangePage KSceneSettingPropertySheet::GetData()
{
	return m_StructChangePage;
}
*/

/*
LRESULT KSceneSettingPropertySheet::OnMsgLockPage( WPARAM wParam, LPARAM lParam )
{
	CTabCtrl* pTabCtl = GetTabControl();
	if (pTabCtl)
	{
		m_bLockChange = wParam == 0 ? FALSE : TRUE;
		pTabCtl->EnableWindow(!m_bLockChange);
	}
	return (LRESULT)m_bLockChange;
}
*/

/*
LRESULT KSceneSettingPropertySheet::OnMsgIsRegisterPage( WPARAM wParam, LPARAM lParam )
{
	DWORD dwType = _kg_to_smaller_size_cast<DWORD>(wParam);
	INDEX_MAP::iterator it = m_IndexMap.find(dwType);
	if (it != m_IndexMap.end())
	{
		return 1;
	}
	return 0;
}*/


/*
bool KSceneSettingPropertySheet::IsInKeyEditState()
{
	return m_StructChangePage.m_nKeyIndex != 0;
}
*/

// KSceneSettingPropertySheet message handlers

/*
BOOL KSceneSettingPropertySheet::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class
	NMHDR* pnmh = (LPNMHDR) lParam;

	// the sheet resizes the page whenever it is activated
	// so we need to resize it to what we want
	if (TCN_SELCHANGE == pnmh->code)
		// user-defined message needs to be posted because page must
		// be resized after TCN_SELCHANGE has been processed
	{
		if (TRUE == m_bLockChange)
		{
			return TRUE;
		}
	}

	return CPropertySheet::OnNotify(wParam, lParam, pResult);
}*/

