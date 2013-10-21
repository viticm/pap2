////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGSFXBingPage.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-8-31 12:18:26
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGSFXBINGPAGE_H_
#define _INCLUDE_KGSFXBINGPAGE_H_

#include "KGLilisPanel.h"
#include "KGResourseMiniViewWnd.h"
#include "KGListCtrl.h"
#include "resource.h"
#include "KGLilisPanel.h"
#include "KEnterEdit.h"

#define SFX_FILE_PATH TEXT("\\Data\\source\\other\\ÌØÐ§")

//------------------------------------------------------------------------------
// Name : KGSFXTree
// Desc : list for sfx
//------------------------------------------------------------------------------
class KGSFXTree : public CTreeCtrl
{
    //DECLARE_DYNAMIC(KGSFXBindTree)

public :
    KGSFXTree();
    ~KGSFXTree();
    void Init();

public :
    //KGResourseMiniViewWnd   m_wndMinView;
    //CString                 m_strSelPath;

public :
    DECLARE_MESSAGE_MAP()
    BOOL PreTranslateMessage(MSG* pMsg);
};

//------------------------------------------------------------------------------
// Name : KGSFXlist
// Desc : 
//------------------------------------------------------------------------------
class KGSFXList : public KGListCtrl
{
public :
    DECLARE_MESSAGE_MAP()
    afx_msg LRESULT OnSetScanl(WPARAM wParam, LPARAM lParam);
};

//------------------------------------------------------------------------------
// Name : KGSFXBindPage
// Desc : Page for bind
//------------------------------------------------------------------------------
class KGSFXBindPage : public CPropertyPage
{
	DECLARE_DYNAMIC(KGSFXBindPage)

public:
	KGSFXBindPage();   // standard constructor
	virtual ~KGSFXBindPage();
	enum { IDD = IDD_DIALOG_SFX_BIND };
    BOOL OnInitDialog();
    BOOL OnSetActive();
    BOOL OnKillActive();
    int  FillTree(HTREEITEM hTreeItem, LPCTSTR szPath);
    void UpdateSFXSceneMode(DWORD dwMode);
    void UpdateSFXSuedeSFX(int nFlage /* 0 : add  1 : update */);
    void UpdateSceneSFXIndex();
    void UpdateSFXScanl();
    int  UpdateBondInfo(LPSTR pszBoneName);

    void UpdateSFXSuedeList();
    void UpdateModelPage();
    CString GetSelSFXName();

    void OnOK();

public :
    KGSFXList               m_list;
    KGSFXTree               m_tree;
    CComboBox               m_combo;
    CImageList              m_imageListTree;
    int                     m_nBindNum;
    CString                 m_strSelPath;
    KEnterEdit              m_editScanl;

    KGLilisButton           m_buttonAdd;
    KGLilisButton           m_buttonDel;

    //
    int                     m_nCurEditIndex;
    int                     m_nEditSFXScanlIndex;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support   
	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedSfxBindDel();
    afx_msg void OnBnClickedSfxBindAdd();
    afx_msg void OnCbnSelchangeCombo();
    afx_msg void OnLvnItemActivateList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg LRESULT OnSetScanl(WPARAM wParam, LPARAM lParam);
};

//------------------------------------------------------------------------------
// Name : GET_SFX_BIND_PAGE
// Desc : 
//------------------------------------------------------------------------------
#define  GET_SFX_BIND_PAGE() \
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
    KGSFXBindPage* pBindPage =  pView->m_paneSFXEditerWnd.GetBindPage();    \
    if (!pBindPage)                  \
        return;                      \

#endif //_INCLUDE_KGSFXBINGPAGE_H_
