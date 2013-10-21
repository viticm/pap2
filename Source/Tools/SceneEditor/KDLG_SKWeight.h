#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "KG3DAnimationContainer.h"
#include "IEEditor.h"
// KDLG_SKWeight dialog
 
class KDLG_SKWeight : public CDialog
{
	DECLARE_DYNAMIC(KDLG_SKWeight)

public:
	KDLG_SKWeight(IEKG3DModel* pModel,
				  KG3DAnimationContainer* pAnimationContainer,
		          CWnd* pParent = NULL);   // standard constructor
	virtual ~KDLG_SKWeight();
	

// Dialog Data
	enum { IDD = IDD_DIALOG_SKWEIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CListCtrl m_ctlAniTable;
	CTreeCtrl m_ctlSkeleton;
	IEKG3DModel* m_pModel;
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	int GetCurSelBoneIndex();
private:
	void ClearAnimationList();
	void FillAnimationWeightList();
	void FillBoneTree();
	void FillAnimationList();
	void OnEditWeight(int nItem, int nSubItem);
	afx_msg void OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult);
private:
	CComboBox m_ctlAnimationList;
private:
	CListCtrl m_ctlAniWeightList;
	afx_msg void OnBnClickedButtonAddanimation();
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
	BOOL LoadStrings();
	TCHAR m_strAniName[MAX_PATH];
	TCHAR m_strWeight[MAX_PATH];
	TCHAR m_strLoadCaption[MAX_PATH];
private:
	CButton m_ctlInheritWeight;
	CEdit m_ctlEditWeight;
	afx_msg void OnLvnItemchangedListAniweight(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemActivateListAniweight(NMHDR *pNMHDR, LRESULT *pResult);

	

	virtual BOOL PreTranslateMessage(MSG* pMsg);
private:
	float m_fWeight;
	int   m_nCurBoneSet;
	KG3DAnimationContainer* m_pAnimationContainer;	
protected:
	virtual void OnOK();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonRemove();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonSave();
};
