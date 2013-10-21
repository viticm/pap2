#if !defined(AFX_KSCENEMODELEDITORSFXSPEEDTREEDIALOG_H__CE60882C_56C8_4811_9C48_3A0EC7898AF7__INCLUDED_)
#define AFX_KSCENEMODELEDITORSFXSPEEDTREEDIALOG_H__CE60882C_56C8_4811_9C48_3A0EC7898AF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneModelEditorSFXSpeedTreeDialog.h : header file
//
#include "KColorButton.h"
/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorSFXSpeedTreeDialog dialog
interface IEKG3DModelSpeedTree ;
interface IELeaf;
//class KG3DSceneModelEditor;


class KSceneModelEditorSFXSpeedTreeDialog : public CDialog
{
// Construction
public:
	KSceneModelEditorSFXSpeedTreeDialog(CWnd* pParent = NULL);   // standard constructor
	void SetSpeedTree(IEKG3DModelSpeedTree* pTree,IEKG3DSceneModelEditor* pEditor)
	{
		m_lpSpeedTree = pTree;
		m_lpModelEditor = pEditor;
	}
// Dialog Data
	//{{AFX_DATA(KSceneModelEditorSFXSpeedTreeDialog)
	enum { IDD = IDD_SFX_SPEEDTREE,
	ENU_LEAF_NODE = 1,
	ENU_BONE_NODE = 0,
	CHECKBOX_WIDTH = 18,};

	KColorButton	m_ColorButton_Diffuse;
	KColorButton	m_ColorButton_Ambient;
	CTreeCtrl	m_Tree;
	float	m_fSize;
	/*float	m_fPitch;
	float	m_fRoll; 
	float	m_fWave;
	float	m_fAccelerate;
	float	m_fMaxSpeed;
	*/
	int     m_nPitch;
	int     m_nRoll;
	int     m_nWave;
	float   m_fAccelerate;
	int     m_nMaxSpeed;
	float   m_fLeafRadius;
	int    m_nLeafAuto;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneModelEditorSFXSpeedTreeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	D3DCOLORVALUE    m_Ambient;
	D3DCOLORVALUE    m_Diffuse;

	IEKG3DModelSpeedTree* m_lpSpeedTree;
	IEKG3DSceneModelEditor* m_lpModelEditor;
	struct BoneNodeInfo
	{
		HTREEITEM hItem;
		BOOL bChecked;
		BOOL bExpanded;
		BoneNodeInfo();
		BoneNodeInfo(HTREEITEM, BOOL, BOOL);
	};

	std::vector<BoneNodeInfo>m_vecBoneNodes;
	BoneNodeInfo* FindNode(HTREEITEM hItem);
	
	HTREEITEM GetCurSelectedBone(DWORD &dwIndex);
	void ToggleMoveLeaf();
	void SetLeafUV(DWORD Type);
	void SetLeafSize(float size);
	void SetLeafAmbient(D3DCOLORVALUE Ambient);
	void SetLeafDiffuse(D3DCOLORVALUE Ambient);
	void SetLeafWave(float Wave);

	// Generated message map functions
	//{{AFX_MSG(KSceneModelEditorSFXSpeedTreeDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	afx_msg void OnBnClickedButtonAddleaf();
	afx_msg void OnBnClickedCheckMove();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnBnClickedRadio10();
	afx_msg void OnBnClickedRadio6();
	afx_msg void OnBnClickedButtonAmbient();
	afx_msg void OnBnClickedButtonDiffuse();
	afx_msg void OnBnClickedButtonSaveAs();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckDrawbone();
	afx_msg void OnBnClickedButtonAuto();
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnBnClickedButtonLoadmtl();
	afx_msg void OnBnClickedButtonChangeleaftex();
	afx_msg void OnBnClickedCheckShowLeaf();
	afx_msg LRESULT OnColorChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnItemexpandedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	void OnSelectLeaf(IELeaf* pLeaf, 
		BOOL bSelected);

	void Help_DeleteAllLeaf(HTREEITEM hItem);
	void DeleteAllLeaf();
	virtual void OnOK();
	void OnDeleteLeaf(HTREEITEM hLeaf, BOOL bUpdateUI);
	void OnDeleteLeafOnNode(HTREEITEM hNode);
public:
	static void OnUpdateUI(IEKG3DModelSpeedTree* pSpdTree, void* pDialog);
private:
	BOOL IsCheckBoxClicked(HTREEITEM hItem, 
		CPoint& ptMouse);
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedCheckIsolate();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENEMODELEDITORSFXSPEEDTREEDIALOG_H__CE60882C_56C8_4811_9C48_3A0EC7898AF7__INCLUDED_)
