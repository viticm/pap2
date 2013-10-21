//////////////////////////////////////////////////////////////////////////////////
////
////  FileName    : KSceneSFXEditorSFX_BIND.h
////  Version     : 1.0
////  Creator     : Zeb
////  Create Date : 2005-10-18 17:11:48
////  Comment     : 
////
//////////////////////////////////////////////////////////////////////////////////
//
//#ifndef _INCLUDE_KSCENESFXEDITORSFXBIND_H_
//#define _INCLUDE_KSCENESFXEDITORSFXBIND_H_
//
//////////////////////////////////////////////////////////////////////////////////
//// KSceneSFXEditorSFX_BIND dialog
//#include "KSceneSFXEditorDoc.h"
//#include "Resource.h"
//#include "KEnterEdit.h"
//
//class KSceneSFXEditorSFXBind : public CPropertyPage
//{
//	DECLARE_DYNAMIC(KSceneSFXEditorSFXBind)
//
//public:
//	KSceneSFXEditorSFXBind();
//	virtual ~KSceneSFXEditorSFXBind();
//
//// Dialog Data
//	enum { IDD = IDD_SFXEDITOR_SFX_BIND };
//
//    void SetSFXEditorDoc( KSceneSFXEditorDoc* pDoc )
//    {
//        ASSERT( pDoc );
//        m_pDoc = pDoc;
//    }
//
//    void UpdateUI();
//
//    void UpdateBoneListUI();
//    void UpdateBoneListBindInfoUI();
//    void UpdateLauncherListUI();
//    void UpdateBillboardListUI();
//
//    void UpdateBindInfo();
//
//private:
//    KSceneSFXEditorDoc *m_pDoc;
//
//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//
//	DECLARE_MESSAGE_MAP()
//
//    KEnterEdit  m_editMeshName;
//    KEnterEdit  m_editMtlName;
//    KEnterEdit  m_editAniName;
//
//    CListCtrl   m_listBone;
//    CListCtrl   m_listLauncher;
//
//    CListCtrl   m_listBillboard;
//
//    void BindSfx();
//
//public:
//    afx_msg void OnBnClickedButtonBrowseMesh();
//    afx_msg void OnBnClickedButtonBrowseMtl();
//    afx_msg void OnBnClickedButtonBrowseAni();
//    virtual BOOL OnInitDialog();
//    virtual BOOL OnSetActive();
//    afx_msg void OnBnClickedButtonBindToBone();
//    afx_msg void OnBnClickedRadioPlay();
//    afx_msg void OnBnClickedRadioStop();
//    afx_msg void OnBnClickedButtonBindCancel();
//    afx_msg void OnBnClickedButtonBillboardBindToBone();
//    afx_msg void OnBnClickedButtonBillboardBindCancel();
//};
//
//
//#endif //_INCLUDE_KSCENESFXEDITORSFXBIND_H_
