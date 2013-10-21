#pragma once
#include "afxcmn.h"
#include "afxwin.h"

interface IEKG3DModel;
class KG3DAnimationContainer;
interface IEKG3DAnimationComposer;
// KDlgAnimationComposer dialog

class KDlgAnimationComposer : public CDialog
{
	DECLARE_DYNAMIC(KDlgAnimationComposer)

public:
	KDlgAnimationComposer(CWnd* pParent = NULL);   // standard constructor
	virtual ~KDlgAnimationComposer();
	void SetModel(IEKG3DModel* pModel);
	void SetAnimationContainer(KG3DAnimationContainer* pContainer);

// Dialog Data
	enum { IDD = IDD_DIALOG_ANICOMBINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonAddtocomposer();
	afx_msg void OnBnClickedButtonWarperstart();
	afx_msg void OnBnClickedButtonWarperdisable();
	afx_msg void OnTvnSelchangingTreeSkeleton(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSelChangeComposer();
	afx_msg void OnSelChangeAnimationInUse();
	void OnOK();
	IEKG3DModel *m_pModel;

private:
	void OnUpdateComposerUI(BOOL);
	void UpdateInfo(IEKG3DAnimationComposer *pComposer,
		int nCurAnimation,
		int nCurBone);

	CTreeCtrl m_ctlSkeletonTree;
	afx_msg void OnBnClickedButtonAddcomposer();
	afx_msg void OnBnClickedButtonDelcomposer();
	void OnSelectBone(int nIndex);
	CComboBox m_ctlComposerList;
	CComboBox m_ctlAnimationInUse;
	CComboBox m_ctlTypeRot;
	CComboBox m_ctlTypeScale;
	CComboBox m_ctlTypeTranslate;
	afx_msg void UpdateFromUI();
	CComboBox m_ctlAnimation;
	KG3DAnimationContainer* m_pAnimationContainer;
private:
	float m_fComposerWeight;
	afx_msg void OnBnClickedButtonChangeanimation();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonLoad();
};
