#if !defined(AFX_KSCENEMODELEDITORSWORD2CHECKTOOL_H__1175BF42_98D0_46BB_8FEE_A183EC71EFCB__INCLUDED_)
#define AFX_KSCENEMODELEDITORSWORD2CHECKTOOL_H__1175BF42_98D0_46BB_8FEE_A183EC71EFCB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneModelEditorSword2CheckTool.h : header file
//
#include "IEEditor.h"
//#include "KG3DSceneModelEditor.h"
/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorSword2CheckTool dialog
interface IEKG3DSceneModelEditor;
class KSceneModelEditorSword2CheckTool : public CDialog
{
// Construction
public:
	KSceneModelEditorSword2CheckTool(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KSceneModelEditorSword2CheckTool)
	enum { IDD = IDD_MODELEDITOR_CHECKTOOL };
	CComboBox	m_Combo_Socket;
	CListBox	m_ListBox_Weapon;
	CComboBox	m_Combo_Ani;
	CListBox	m_ListBox_Up;
	CListBox	m_ListBox_Head;
	CListBox	m_ListBox_Down;
	int		m_nMode;

	//////////////////////////////////////////////////////////////////////////
	//Add 7-26-2005 双人检查
	CComboBox	m_Combo_Socket1;
	CListBox	m_ListBox_Weapon1;
	CComboBox	m_Combo_Ani1;
	CListBox	m_ListBox_Up1;
	CListBox	m_ListBox_Head1;
	CListBox	m_ListBox_Down1;
	int		m_nMode1;

	//Add 7-26-2005 双人检查
	//////////////////////////////////////////////////////////////////////////
	
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneModelEditorSword2CheckTool)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	struct ModelInfo
	{
		IEKG3DModel* pModel;
		string  strModelName;
		ModelInfo()
		{
			pModel = NULL;
		}
		ModelInfo(IEKG3DModel* pInModel, LPCSTR strInModelName)
		{
			pModel = pInModel;
			strModelName = strInModelName;
		}
	};

	//
	IEKG3DModel* m_pCurModelBody;
	typedef list<ModelInfo> _ModelList;

	_ModelList m_listModelHead;
	_ModelList m_listModelUpBody;
	_ModelList m_listModelDownBody;
	_ModelList m_listModelWeapon;

	////////////////////////////////////////////////////////////////////////////
	////Add 7-26-2005 双人检查
	BOOL m_bSynAniChange;
	_ModelList m_listModelHead1;
	_ModelList m_listModelUpBody1;
	_ModelList m_listModelDownBody1;
	_ModelList m_listModelWeapon1;
	//Add 7-26-2005 双人检查 end
	/////////////////////////////////////////////////////////////////////////
	void OpenFromTabFile(_ModelList* pList,BOOL bAnimation, int nIndex = 0);

	void FillListBoxWeapon(int nIndex = 0);
	void FillListBoxHead(int nIndex = 0);
	void FillListBoxUpBody(int nIndex = 0);
	void FillListBoxDownBody(int nIndex = 0);

	void FillComboSocket(_ModelList* pModel, int nIndex = 0);
	void FillComboAnimation(_ModelList* pModel, int nIndex = 0);
	void UpdataModels();
	void PlayAnimation(int AniIndex, DWORD PlayMethod, float Speed, int nIndex  = 0); 

	IEKG3DSceneModelEditor*  m_pCurModelEditor;
	// Generated message map functions
	//{{AFX_MSG(KSceneModelEditorSword2CheckTool)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void SetModelEditor(IEKG3DSceneModelEditor* pModelEditor);
	afx_msg void OnBnClickedButtonAddhead();
	afx_msg void OnLbnSelchangeListHead();
	afx_msg void OnBnClickedButtonAddup();
	afx_msg void OnBnClickedButtonAdddown();
	afx_msg void OnLbnSelchangeListUp();
	afx_msg void OnLbnSelchangeListDown();
	afx_msg void OnBnClickedButtonAniopen();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnCbnSelchangeComboAniname();
	afx_msg void OnBnClickedButtonAddheadtab();
	afx_msg void OnBnClickedButtonAdduptab();
	afx_msg void OnBnClickedButtonAdddowntab();
	afx_msg void OnCbnSelchangeComboMode();
	afx_msg void OnBnClickedButtonAddweapen();
	afx_msg void OnLbnSelchangeListWeapen();
	afx_msg void OnCbnSelchangeComboSocket();
	afx_msg void OnBnClickedCheckEditup();
	afx_msg void OnBnClickedButtonAddhead2();
	afx_msg void OnBnClickedButtonAddheadtab2();
	afx_msg void OnBnClickedButtonHeaddel2();
	afx_msg void OnBnClickedButtonAddup2();
	afx_msg void OnBnClickedButtonAdduptab2();
	afx_msg void OnBnClickedButtonUpdel2();
	afx_msg void OnBnClickedCheckEditup2();
	afx_msg void OnBnClickedButtonAdddown2();
	afx_msg void OnBnClickedButtonAdddowntab3();
	afx_msg void OnBnClickedButtonDowndel2();
	afx_msg void OnBnClickedButtonAddweapen2();
	afx_msg void OnBnClickedButtonAddweapentab2();
	afx_msg void OnBnClickedButtonWeapondel2();
	afx_msg void OnBnClickedButtonPlay2();
	afx_msg void OnBnClickedButtonAniopen2();
	afx_msg void OnBnClickedButtonHeaddel();
	afx_msg void OnBnClickedButtonUpdel();
	afx_msg void OnBnClickedButtonDowndel();
	afx_msg void OnBnClickedButtonWeapondel();
	afx_msg void OnBnClickedButtonAddweapentab();
	afx_msg void OnLbnSelchangeListHead2();
	afx_msg void OnLbnSelchangeListUp2();
	afx_msg void OnLbnSelchangeListDown2();
	afx_msg void OnLbnSelchangeListWeapen2();
	afx_msg void OnCbnSelchangeComboAniname2();
	afx_msg void OnCbnSelchangeComboSocket2();
	afx_msg void OnCbnSelchangeComboMode2();
	afx_msg void OnBnClickedButton1();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENEMODELEDITORSWORD2CHECKTOOL_H__1175BF42_98D0_46BB_8FEE_A183EC71EFCB__INCLUDED_)
