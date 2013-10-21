////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGSFXModelPane.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-9-1 3:05:24
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGSFXMODELPANE_H_
#define _INCLUDE_KGSFXMODELPANE_H_

#include "KGLilisPanel.h"
#include "KGSFXModelViewPage.h"
#include "resource.h"
#include "KSceneSFXEditorTextureDlg.h"
#include "KGSFXGlobPage.h"
#include "KGPropertySheet.h"
#include "KGSFXEditSockBonePage.h"

//------------------------------------------------------------------------------
// Name : KGSFXModelViewSheet
// Desc : 
//------------------------------------------------------------------------------
class KGSFXModelViewSheet : public KGPropertySheet
{
public :
    KGSFXModelViewSheet()
    {
 
    }
    ~KGSFXModelViewSheet()
    {
    }
    int UpdateUI()
    {
        m_pageTex.UpdateUI();
        return TRUE;
    }

    IEKG3DSceneSFXEditor* GetSfxScene();

public :
    KSceneSFXEditorTextureDlg m_pageTex;
    KGSFXModelViewPage m_pageModelView;
    KGSFXGlobPage      m_pageGlobPage;
    KGSFXEditSockBonePage m_pageSockBone;


public :
    DECLARE_MESSAGE_MAP()
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnPaint();

};

//------------------------------------------------------------------------------
// Name : KGSFXModelPane
// Desc : 
//------------------------------------------------------------------------------

class KGSFXModelPane : public CDialog
{
	DECLARE_DYNAMIC(KGSFXModelPane)

public:
	KGSFXModelPane(CWnd* pParent = NULL);   
	virtual ~KGSFXModelPane();
	enum { IDD = IDD_PANE_SFX_MODEL_VIEW };

    int SetDependWnd(CView* pDependWnd);

    BOOL  PreTranslateMessage(MSG* pMsg);
    DWORD UpdateWndState();
    void  OnLButtonDblClick();
    int   UpdateUI();
    IEKG3DSceneSFXEditor* GetSfxScene();

public :
    KGSFXModelViewSheet   m_sheet;
    //KGSFXModelViewPage  m_pageModelView;
    //KGLilisPanel        m_panel;
    CView*   m_pDependWnd;
    DWORD    m_dwWndState;

protected:
    BOOL    OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    void OnOK(){};
    void OnCancel(){};
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnPaint();
    afx_msg void OnBnClickedButton1();
};

#endif //_INCLUDE_KGSFXMODELPANE_H_
