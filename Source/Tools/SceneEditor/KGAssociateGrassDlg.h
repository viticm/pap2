////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGAssociateGrassDlg.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2007-4-12 18:18:46
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGASSOCIATEGRASSDLG_H_
#define _INCLUDE_KGASSOCIATEGRASSDLG_H_

#include "KSceneSceneEditorDialogGrass.h"
#include "KSceneEditorTopViewDialog.h"


interface IEKG3DSceneSceneEditor;
class IEKG3DTerrain;

// so crazy :P --------------------------------------

class KGGrassInfoModifyPane : public KSceneSceneEditorDialogGrass 
{
public :
    BOOL OnInitDialog();

    CStatic m_Lable_1;
    CStatic m_Lable_2;
    CStatic m_Lable_3;

    KGLilisButton m_Button_1;
    KGLilisButton m_Button_2;

    CEdit   m_Edit_1;
    CEdit   m_Edit_2;

    CStatic m_Lable_A;
    CStatic m_Lable_B;
    CStatic m_Lable_C;
    CStatic m_Lable_D;
    CStatic m_Lable_E;
    CStatic m_Lable_G;

    CEdit   m_Edit_A;
    CEdit   m_Edit_B;
    CEdit   m_Edit_C;
    CEdit   m_Edit_D;
    CEdit   m_Edit_E;
    CEdit   m_Edit_F;
    CEdit   m_Edit_G;
    CEdit   m_Edit_H;

    CButton m_Button_3;
    CButton m_Button_4;
    CButton m_Button_5;

    CButton m_Button_A;
    CButton m_Button_B;

    KGLilisButton m_Button_C;
    KGLilisButton m_Button_D;

    CListBox m_ListBoxMdlGrass;
    KGLilisButton  m_Button_E;


public :
    virtual void DoDataExchange(CDataExchange* pDX); 

  	DECLARE_MESSAGE_MAP()
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

class KGTopViewPane : public KSceneEditorTopViewDialog
{
public :
    DECLARE_MESSAGE_MAP()
    afx_msg void OnSetFocus(CWnd* pOldWnd);
};

class KGAssociateGrassDlg : public CDialog
{
	DECLARE_DYNAMIC(KGAssociateGrassDlg)

public :
	KGAssociateGrassDlg(CWnd* pParent = NULL);   
	virtual ~KGAssociateGrassDlg();

	enum { IDD = IDD_ASSOCIATE_GRASS };

    inline BOOL AttachScene(
        IEKG3DSceneSceneEditor* pSceneEditor,
        IEKG3DTerrain*          pTerrainEx,
        int                     nLayer,
        int                     nTexIndex
        )
    {
        m_lpSceneEditor = pSceneEditor;
        m_pTerrainEx    = pTerrainEx;
        m_nLayer        = nLayer;
        m_nTextureIndex = nTexIndex;

        return TRUE;
    };

    BOOL OnInitDialog();


public :
    KGGrassInfoModifyPane   m_GrassPane;
    KGTopViewPane           m_TopViewPane;

    IEKG3DTerrain*          m_pTerrainEx;
    IEKG3DSceneSceneEditor* m_lpSceneEditor;
    int		                m_nLayer;
    int		                m_nTextureIndex;

    //HCURSOR                 m_hCursorArrow;

    KGLilisButton           m_ButtonApply;
    CEdit                   m_EditMessage;

    CProgressCtrl           m_Progress;

    static LRESULT Progress(WPARAM wParam, LPARAM lParam);

protected :
	virtual void DoDataExchange(CDataExchange* pDX); 
    BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnBnClickedApplay();

};


#endif //_INCLUDE_KGASSOCIATEGRASSDLG_H_
