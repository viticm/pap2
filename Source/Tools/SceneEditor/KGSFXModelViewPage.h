////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGSFXModelViewPage.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-9-3 9:39:41
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGSFXMODELVIEWPAGE_H_
#define _INCLUDE_KGSFXMODELVIEWPAGE_H_

#include "KGLilisPanel.h"
#include "resource.h"

//------------------------------------------------------------------------------
// Name : KGSFXBindBonePicture
// Desc : 
//------------------------------------------------------------------------------
class KGSFXBindBonePicture : public CDialog
{
    DECLARE_DYNAMIC(KGSFXBindBonePicture)

public:
    KGSFXBindBonePicture(CWnd* pDependWnd = NULL);   
    virtual ~KGSFXBindBonePicture();
    enum { IDD = IDD_DIALOG_BIND_TAB };

    vector<CWnd*> m_vecCellWnd;
    RECT          m_rectCurCell;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    
    BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnLButtonDblClk(UINT uClass, CPoint point);

};

struct ModelInfoPair
{
    string strName;
    HTREEITEM hTreeItem;

    ModelInfoPair(string name, HTREEITEM item)
        : strName(name), hTreeItem(item)
    {
    }
};

class KGSFXModelViewPage : public CPropertyPage
{
	DECLARE_DYNAMIC(KGSFXModelViewPage)

public:
	KGSFXModelViewPage();   // standard constructor
	virtual ~KGSFXModelViewPage();
	enum { IDD = IDD_DIALOG_MODEL_VIEW };

	int  FillTreeFromFileMap();
    int  FillTree(HTREEITEM hTreeItem, LPCTSTR szPath, int* pIndex);
    int  FillComb();
    //void FillBone();
    CString GetSelMdlName();

    void UpdateBindModel();
    void UpdateMdelAin();
    void InitMdlAin();

    void UpdateInterface();

public :
    KGSFXBindBonePicture m_pictureBindBone;
    CImageList  m_imageListTree;
    CTreeCtrl   m_tree;
    CComboBox   m_comb;
    CListBox    m_listSearch;
    CProgressCtrl   m_Progress;

    CString     m_strSelPath;       // model path
    CString     m_strAinPath;       // ani path

    CWinThread* m_pFillThread;

	KGLilisButton	m_buttomReflush;
    CString     m_strStartPath;
    CEdit       m_EditSearch;

    vector<ModelInfoPair> m_ModelInfo;

    bool m_bExitSearchThread;
    bool m_bResearch;
    string m_strSearch;
    bool m_bThreadExit;
    HANDLE m_hSearch;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    BOOL    OnInitDialog();
    BOOL    OnSetActive();
    void    OnOK(){}
    void    OnCancel(){}

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnPaint();
    afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMClickTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedBlist();
    afx_msg void OnBnClickedBtabl();
    afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonRefush();
    afx_msg void OnEnChangeEditSearch();
    afx_msg void OnLbnSelchangeListSearch();
    afx_msg void OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnClose();
};

//------------------------------------------------------------------------------
// Name : GET_SFX_MOVE_VIEW_PAGE
// Desc : 
//------------------------------------------------------------------------------
#define  GET_SFX_MDLVIEW_PAGE() \
    CMDIFrameWnd* pMainWnd  = dynamic_cast<CMDIFrameWnd*>(AfxGetMainWnd()); \
    if (!pMainWnd)                   \
        return;                      \
    KSceneSFXEditorFrame* pAFrame =  \
        dynamic_cast<KSceneSFXEditorFrame*>(pMainWnd->GetActiveFrame());    \
    if (!pAFrame)                    \
        return;                      \
    KSceneSFXEditorView* pView =     \
        dynamic_cast<KSceneSFXEditorView*>(pAFrame->GetActiveView());       \
    if (!pView)                      \
        return;                      \
    KGSFXModelViewPage* pMDLViewPage =  &(pView->m_paneSFXModelView.m_sheet.m_pageModelView);    \
    if (!pMDLViewPage)                  \
        return;                      \


#endif //_INCLUDE_KGSFXMODELVIEWPAGE_H_
