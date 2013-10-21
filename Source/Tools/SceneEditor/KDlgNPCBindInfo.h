#pragma once

#include "KGNPCItemListTree.h"
#include "kgpropertylistctrl.h"
#include "afxcmn.h"
#include "afxwin.h"

#include "IEEditor.h"//by dengzhihui Refactor 2006年11月15日

// KDlgNPCBindInfo dialog

class KDlgNPCBindInfo : public CDialog
{
	enum enuModelPartID
	{
		MAIN_MODEL,
		FACE,
		HAT,
		LEFT_HAND,
		LEFT_PELVIS,
		RIGHT_HAND,
		RIGHT_PELVIS,
		SPINE,
	};

	DECLARE_DYNAMIC(KDlgNPCBindInfo)

	enum enuType
	{
		ITEM_TYPE_WEAPON = 0,
		ITEM_TYPE_OTHER,
	};

public:
	struct BindInfo
	{
		enum enu
		{
			FACE,
			LEFTHAND,
			RIGHTHAND,
			LEFTPELVIS,
			RIGHTPELVIS,
			SPINE0,
			SPINE1,
			COUNT
		};

		CString strNames[7];
		
		void UpdateData(CDataExchange* pDx);
	};

	KDlgNPCBindInfo(LPVOID pScene, 
		ITabFile* pTabFile, 
		IIniFile* pIniFile, 
		LPKGLISTITEM pListItem, 
		CWnd* pParent = NULL);   // standard constructor

	virtual ~KDlgNPCBindInfo();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_MEDIUM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSetweapon();
public:
	afx_msg void OnBnClickedButtonSetitem();
private:
	KGNPCItemListTree m_Weapon;
	KGNPCItemListTree m_Item;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioLefthand();
	afx_msg void OnBnClickedRadioRighthand();
	afx_msg void OnBnClickedRadioLeftpelvis();
	afx_msg void OnBnClickedRadio1Rightpelvis();
	afx_msg void OnBnClickedRadioSpine0();
	afx_msg void OnBnClickedRadioSpine1();
	afx_msg void OnCbnSelchangeComboType();
	CTreeCtrl m_ctlItemListTree;
	CComboBox m_ctlType;
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	void BindItemByBindInfo();
protected:
	virtual void OnCancel();
	BindInfo m_BindInfo;
	IEKG3DSceneObjectEditor* m_pScene;//by dengzhihui Refactor 2006年11月15日
	ITabFile*			m_pTabFile;
	IIniFile*			    m_pIniFile;
	LPKGLISTITEM		m_pListItem;

public:
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonOpenface();
	afx_msg void OnNMClickTreeItem(NMHDR *pNMHDR, LRESULT *pResult);
};
