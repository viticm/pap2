////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSettingPropertySheet.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-3-17 15:48:42
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESETTINGPROPERTYSHEET_H_
#define _INCLUDE_KSCENESETTINGPROPERTYSHEET_H_
#include "IEKG3DEnvironment.h"
#include <map>
////////////////////////////////////////////////////////////////////////////////
#pragma once



// KSceneSettingPropertySheet
#define WM_DWGC_TO_KEY_TYPE_PAGE (WM_USER + 400)	///WPARAM = StructChangePage*类型，LPARAM=是否锁定，1为TRUE, LRESULT=成功TRUE/FALSE，只能Send

#define WM_DWGC_LOCK_TAB (WM_USER + 402)	///WPARAM = 是否锁定，LRESULT = 是否锁定
#define WM_IS_REGISTER_PAGE (WM_USER + 403) ///WPARAM = dwType, LRESULT = 是否注册
class KSceneSettingPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(KSceneSettingPropertySheet)
/*
public:
	struct StructChangePage 
	{
	private:
		DWORD	m_FateCode;
	public:
		KG3DENVEFF_HANDLE m_EnvEffectHandle;
		INT m_nKeyIndex;
		StructChangePage()
		{
			ZeroMemory(this, sizeof(StructChangePage));
			m_FateCode = 0xFA7EC0DE;
		};
		StructChangePage(const StructChangePage& Other)
		{
			memcpy_s(this, sizeof(StructChangePage), &Other, sizeof(StructChangePage));
			m_FateCode = 0xFA7EC0DE;
		}
		bool IsValidInMemory()
		{
			return m_FateCode == 0xFA7EC0DE;
		}///用特殊值检查内存状态
	};*/

/*
private:
	typedef map<DWORD, int> INDEX_MAP;
	INDEX_MAP m_IndexMap;*/

	//StructChangePage m_StructChangePage;
	//BOOL m_bLockChange;
/*
private:
	afx_msg LRESULT OnMsgChangeToPage(WPARAM wParam, LPARAM lParam);*/

public:
	/*
	KSceneSettingPropertySheet();
	
	KSceneSettingPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
		KSceneSettingPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
		virtual ~KSceneSettingPropertySheet();*/
	

	//HRESULT AddIndexPage(DWORD dwType, CPropertyPage* pPage);
	
	//afx_msg LRESULT OnMsgToKeyTypePage(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnMsgLockPage(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnMsgIsRegisterPage(WPARAM wParam, LPARAM lParam);
	
	//bool IsInKeyEditState();
	//StructChangePage GetData();

protected:
	DECLARE_MESSAGE_MAP()
	//virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};




#endif //_INCLUDE_KSCENESETTINGPROPERTYSHEET_H_
