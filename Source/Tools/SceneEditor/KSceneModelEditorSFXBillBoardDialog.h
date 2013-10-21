#if !defined(AFX_KSCENEMODELEDITORSFXBILLBOARDDIALOG_H__B7E3DA58_4608_42E8_B12F_24F9B8BDE3D2__INCLUDED_)
#define AFX_KSCENEMODELEDITORSFXBILLBOARDDIALOG_H__B7E3DA58_4608_42E8_B12F_24F9B8BDE3D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneModelEditorSFXBillBoardDialog.h : header file
///*
//#include ".\kmodelsfxbillboard.h"*/
/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorSFXBillBoardDialog dialog

class KSceneModelEditor;

class KSceneModelEditorSFXBillBoardDialog : public CDialog
{
// Construction
public:
	KSceneModelEditorSFXBillBoardDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KSceneModelEditorSFXBillBoardDialog)
	enum { IDD = IDD_SFX_BILLBOARD };
	CComboBox	m_ComboMotionType;
	CComboBox	m_ComType;
	CSpinButtonCtrl	m_Spin_NumSprite;
	CListBox	m_ListBoxSprite;
	int		m_nNumSprite;
	CString	m_scTextureName;
	float	m_fAnimationLength;
	BOOL	m_bZBuffer;
	BOOL	m_bSort;
	float	m_fSpriteSize;
	BOOL	m_bPositioning;
	float	m_fBeta;
	float	m_fGama;
	float	m_fRadiu;
	float	m_fSpeed;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneModelEditorSFXBillBoardDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:

	/*void SetBillBoard(KSceneModelEditor* pEditor,
		ModelSFX::KModelSFXBillBoard* pBillBoard)
	{
		m_lpSceneModelEditor = pEditor;
		m_lpModelSFXBillBoard = pBillBoard;
	}*/
protected:
	//K3DScene::ENTITYLIST m_listEntityRender;
	//K3DScene::ENTITYLIST m_listEntitySeletable;
	//K3DScene::ENTITYLIST m_listEntitySeleted;

	void FillListBoxSprite();

	//KSceneModelEditor* m_lpSceneModelEditor;
	//ModelSFX::KModelSFXBillBoard* m_lpModelSFXBillBoard;
	// Generated message map functions
	//{{AFX_MSG(KSceneModelEditorSFXBillBoardDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditNumsprite();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelchangeListSprite();
	afx_msg void OnChangeEditSize();
	afx_msg void OnCheckPosition();
	afx_msg void OnSelchangeComboMotiontype();
	afx_msg void OnChangeEditRadiu();
	afx_msg void OnChangeEditSpeed();
	afx_msg void OnChangeEditBeta();
	afx_msg void OnChangeEditGama();
	afx_msg void OnCheckSort();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeComboType();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENEMODELEDITORSFXBILLBOARDDIALOG_H__B7E3DA58_4608_42E8_B12F_24F9B8BDE3D2__INCLUDED_)
