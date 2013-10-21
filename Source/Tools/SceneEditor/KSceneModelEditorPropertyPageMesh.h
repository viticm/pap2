#pragma once

#include "KSceneModelEditorMeshSocketSetDialog.h"
#include "afxwin.h"
#include "IEKG3DMesh.h"
#include "afxcmn.h"

class KSceneModelEditorPropertyPageMesh : public CPropertyPage
{
	DECLARE_DYNCREATE(KSceneModelEditorPropertyPageMesh)

// Construction
public:
	KSceneModelEditorPropertyPageMesh(); 
	~KSceneModelEditorPropertyPageMesh();

	
// Dialog Data
	//{{AFX_DATA(KSceneModelEditorPropertyPageMesh)
	enum { IDD = IDD_MODELEDITOR_MESH };
	CButton	m_Static_LOD;
	CButton	m_ButtonBone;
	CButton	m_ButtonSocket;
	CListBox	m_ListBoxSocket;
	CTreeCtrl	m_TreeBone;
	CString	m_scSocketName;
	int		m_nLOD;
	DWORD   m_dwBoneIndex;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(KSceneModelEditorPropertyPageMesh)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	IEKG3DMesh*  m_oldMesh;
	IEKG3DModel* m_lpCurModel;
	IEKG3DModel* m_lpCurModelParent;

	KSceneModelEditorMeshSocketSetDialog* m_lpDialogSocketSet;
// Implementation
protected:
	IEKG3DMesh* GetReleasedMesh();
	void  SetBBoxScaleGroundEnable(BOOL bVal);
	void  FillListBoxSocket();
	void ExpandChildNode(HTREEITEM hItem, BOOL bExpandNext, BOOL bExpand);
	BOOL m_bExpandBones;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg );
	virtual BOOL OnInitDialog();
	void SetCurModel(IEKG3DModel* pCurModel,IEKG3DModel* pParent);
	// Generated message map functions
	//{{AFX_MSG(KSceneModelEditorPropertyPageMesh)
	afx_msg void OnSelchangeListSocket();
	afx_msg void OnButtonSrename();
	void OnBnClickedButtonNomralHalfSphere();
	afx_msg void OnClickTreeBone(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTreeBone(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonSadd();
	afx_msg void OnButtonSdelete();
	afx_msg void OnButtonBind();
	afx_msg void OnButtonSunbind();
	afx_msg void OnCheckProgress();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButtonSset();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonBssave();
	afx_msg void OnBnClickedButtonBsload();
	afx_msg void OnBnClickedButtonNormalSpe();
	afx_msg void OnBnClickedButtonExpand();
	afx_msg void OnBnClickedButtonCollapse();
	afx_msg void OnBnClickedCheckShowbones();
	afx_msg void OnBnClickedButtonChangemodel();
	afx_msg void OnBnClickedCheckShowbbox();
	afx_msg void OnBnClickedCheckShowpicthon();
	afx_msg void OnBnClickedCheckPartbbox();
	afx_msg void OnBnClickedBunCreatbonebox();
    afx_msg void OnBnClickBillBoard();
public:
	CButton m_ctlCheckProgress;
	float m_fWeight;
	float m_fScaleX;
	float m_fScaleY;
	float m_fScaleZ;
	CSliderCtrl m_SliderX;
	CSliderCtrl m_SliderY;
	CSliderCtrl m_SliderZ;
	
	afx_msg void OnNMCustomdrawSliderX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderZ(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSavetolow();
	afx_msg void OnEnChangeEditLod();
	float m_fLod;
	afx_msg void OnBnClickedButtonSavetomedium();
	afx_msg void OnNMCustomdrawSliderBonescale(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditBonescale();
};

