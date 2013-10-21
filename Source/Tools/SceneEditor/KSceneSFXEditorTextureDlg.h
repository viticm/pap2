////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorTextureDlg.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-9-5 9:16:32
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESFXEDITORTEXTUREDLG_H_
#define _INCLUDE_KSCENESFXEDITORTEXTUREDLG_H_

#include "KGResourseMiniViewWnd.h"
#include "KGListCtrl.h"
#include "KGLilisPanel.h"

class KSceneSFXEditorTextureDlg : public CPropertyPage
{
    DECLARE_DYNAMIC(KSceneSFXEditorTextureDlg)

public:
    KSceneSFXEditorTextureDlg();   
    virtual ~KSceneSFXEditorTextureDlg();
    enum { IDD = IDD_SFXEDITOR_TEXTURE_PANEL };

    virtual BOOL OnInitDialog();
    virtual void OnOK();
    void UpdateUI();
    void UpdateTexturePreView();
    BOOL OnSetActive();

    void UpdateSelState(int nFlag); // 0 : normal 1 : hight light
    void CreatePreViewWnd();

public :
    KGListCtrl   m_listTexture;
    CEdit        m_editCutNum;
    int          m_nSelect;

    KGResourseMiniViewWnd   m_wndMinView;

    KGLilisButton   m_buttonAdd;
    KGLilisButton   m_buttonDel;

    struct _ITEMDATE
    {
        int nIndex; // texture index
        int nType;  // 0 : title  1 : id  2 : file path  3 : cut
    };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);   
    DECLARE_MESSAGE_MAP()
    afx_msg void OnButtonClickAdd();
    afx_msg void OnButtonClickDel();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLvnItemchangedListTexture(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMClickListTexture(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnItemActivateListTexture(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEnSetfocusEditCutNum();
    afx_msg void OnEnKillfocusEditCutNum();
    afx_msg void OnLvnKeydownListTexture(NMHDR *pNMHDR, LRESULT *pResult);
};


#endif //_INCLUDE_KSCENESFXEDITORTEXTUREDLG_H_
