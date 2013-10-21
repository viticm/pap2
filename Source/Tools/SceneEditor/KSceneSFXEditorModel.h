////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorModel.h
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2006-1-4 14:42:11
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESFXEDITORMODEL_H_
#define _INCLUDE_KSCENESFXEDITORMODEL_H_

////////////////////////////////////////////////////////////////////////////////
#include "KSceneSFXEditorDoc.h"
#include "Resource.h"
#include "KEnterEdit.h"
#include "KGLilisPanel.h"

// KSceneSFXEditorModel dialog

class KSceneSFXEditorModel : public CDialog
{
	DECLARE_DYNAMIC(KSceneSFXEditorModel)

public:
	KSceneSFXEditorModel(CWnd* pWnd = NULL);
	virtual ~KSceneSFXEditorModel();

    void SetSFXEditorDoc( KSceneSFXEditorDoc* pDoc )
    {
        ASSERT( pDoc );
        m_pDoc = pDoc;
    }


    void UpdateUI();
    void Check();

    void OnFillSubsetComboBox();

    void OnSetAnimationSpeed();
	void OnSetAnimationStart();

    void UpdateModelSelected();
    void OnOK(){};
    void OnCancel(){};

// Dialog Data
	enum { IDD = IDD_SFXEDITOR_MODEL };

    CEdit       m_editMeshName;
    CEdit       m_editMtlName;
    CEdit       m_editAniName;

	DWORD		m_dwNumFrame;
	FLOAT		m_fFrameTime;

    KGLilisButton m_buttonAdd;
    KGLilisButton m_buttonDel;

    
    KEnterEdit  m_editAnimationSpeed;
    KEnterEdit  m_editAnimationStart;
    
    CComboBox   m_comboSubset;
    CStatic     m_picSubsetColor;

    // mutil models
    DWORD       m_dwCurIndex;

    KGLilisButton m_buttonMdl;
    KGLilisButton m_buttonMtl;
    KGLilisButton m_buttonAin;

    CComboBox     m_comboModels;



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    KSceneSFXEditorDoc *m_pDoc;

    D3DXCOLOR   m_Color;

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonBrowseMesh();
    afx_msg void OnBnClickedButtonBrowseAni();
protected:
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnCbnDropdownComboSubset();
    afx_msg void OnStnClickedPicSubsetColor();
    afx_msg void OnCbnSelchangeComboSubset();
    afx_msg void OnBnClickedButtonBrowseMtl();
public:
    virtual BOOL OnSetActive();
public:
    BOOL OnInitDialog();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnCbnSelchangeComboModel();
	afx_msg void OnBnClickedLoop();
public:
    afx_msg void OnBnClickedButtonAdd();
public:
    afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedBillbord();
};


#endif //_INCLUDE_KSCENESFXEDITORMODEL_H_
