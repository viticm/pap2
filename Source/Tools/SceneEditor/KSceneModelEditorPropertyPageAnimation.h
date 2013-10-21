#pragma once
#include "KG3DAnimationContainer.h"

class KSceneModelEditorPropertyPageAnimation : public CPropertyPage
{
	DECLARE_DYNCREATE(KSceneModelEditorPropertyPageAnimation)
 
// Construction
public:
	KSceneModelEditorPropertyPageAnimation();
	~KSceneModelEditorPropertyPageAnimation(); 
 
// Dialog Data 
	//{{AFX_DATA(KSceneModelEditorPropertyPageAnimation)
	enum { IDD = IDD_MODELEDITOR_ANIMATION };
	CSliderCtrl	m_SliderCtrl_Position;
	CListBox	m_ListBox_Animation;
	//////////////////////////////////////////////////////////////////////////
	//For sequence
	CListBox    m_ListBox_Sequence;
	int         m_nStartFrame;
	int         m_nEndFrame;
	float       m_fTweenTime; 
	float       m_fSpd;
	float	    m_fTotalLength;
	//////////////////////////////////////////////////////////////////////////
	
	float	m_fFrameTime;
	CString	m_strAnimationName;
	DWORD	m_dwNumFrame;
	CButton m_ctlEnableWarper;
	float	m_fSpeed; 
	//}}AFX_DATA

    float   m_fFloorSlipSpeed;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(KSceneModelEditorPropertyPageAnimation)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	DWORD m_dwPlayMethod;
	void SetModel(IEKG3DModel* pModelEx);
	void SetModel2(IEKG3DModel* pModelEx);
	
protected:

	// Generated message map functions
	//{{AFX_MSG(KSceneModelEditorPropertyPageAnimation)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonSave();
	
	afx_msg void OnLbnSelchangeListAnimation();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonRename();
	afx_msg void OnBnClickedButtonSaveseparate();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonSavesequ();
	afx_msg void OnBnClickedButtonLoadsequ();
	afx_msg void OnBnClickedButtonPlaysequ();
	afx_msg void OnLbnSelchangeListSequ();
	afx_msg void OnBnClickedButtonPause();
	afx_msg void OnBnClickedButtonAniwarper();
	afx_msg void OnBnClickedCheckEnablehalf();
	afx_msg void OnBnClickedButtonNormalize();
	afx_msg void OnBnClickedCheckPlayonce();
	afx_msg void OnBnClickedButtonFloorTex();
	afx_msg void OnBnClickedButtonTop();
	afx_msg void OnBnClickedButtonBottom();
	afx_msg void OnBnClickedButtonPlay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	void UpdateUIState(BOOL bAniLoaded);
	virtual BOOL PreTranslateMessage(MSG* pMsg );
	HRESULT OpenAnimationFile();
	HRESULT SaveAnimation2File();
	void FillSequenceEditBox();

	IEKG3DModel* m_lpModel;
	BOOL    m_bInited;
	KG3DAnimationContainer m_AnimationContainer;
	BOOL m_bLoop;
	BOOL m_bPaused;
};
