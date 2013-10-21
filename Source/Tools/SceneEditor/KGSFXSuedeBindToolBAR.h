////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGSFXSuedeBindToolBAR.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-9-2 10:09:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGSFXSUEDEBINDTOOLBAR_H_
#define _INCLUDE_KGSFXSUEDEBINDTOOLBAR_H_

#include "KEnterEdit.h"
#include "KGLilisPanel.h"

class KGSFXSuedeBindToolBar : public CDialog
{
	DECLARE_DYNAMIC(KGSFXSuedeBindToolBar)

public:
	KGSFXSuedeBindToolBar(CWnd* pParent = NULL);  
	virtual ~KGSFXSuedeBindToolBar();
	enum { IDD = IDD_DIALOG_SUEDE_TOOL };

    void AddSFXSuedeSFX(LPSTR pszPath);
    void CoverSFXSuedeSFX(LPSTR pszPath);
    void UpdateTransRota();

    BOOL OnInitDialog();

    INT     m_checkSFXLoop;
    INT     m_checkSFXPlay;
    INT     m_checkMDLLoop;
    INT     m_checkMDLPlay;
    INT     m_checkPlay;

    KEnterEdit m_editTran_X;
    KEnterEdit m_editTran_Y;
    KEnterEdit m_editTran_Z;

    KEnterEdit m_editRota_X;
    KEnterEdit m_editRota_Y;
    KEnterEdit m_editRota_Z;

    FLOAT     m_nTrans_X;
    FLOAT     m_nTrans_Y;
    FLOAT     m_nTrans_Z;
    FLOAT     m_nRota_X;
    FLOAT     m_nRota_Y;
    FLOAT     m_nRota_Z;

    KGLilisButton   m_buttonLoad;
    KGLilisButton   m_buttonSave;
    KGLilisButton   m_bottonRSMdl;
    KGLilisButton   m_bottonRSFX;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	DECLARE_MESSAGE_MAP()
    void OnOK(){};
    void OnCancel(){};
    afx_msg  void OnTimer(UINT_PTR nIDEvent);
    afx_msg LRESULT OnEditEnter(WPARAM wParam, LPARAM lParam);
    afx_msg void OnBnClickedButtonSave();
    afx_msg void OnBnClickedButtonLoad();
    afx_msg void OnBnClickedSFXLoop();
    afx_msg void OnBnClickedSFXPlay();
    afx_msg void OnBnClickedMDLLoop();
    afx_msg void OnBnClickedMDLPlay();
    afx_msg void OnBnClickedResetSFX();
    afx_msg void OnBnClickedResetMDL();
    afx_msg void OnBnClickedPlay();
};

#endif //_INCLUDE_KGSFXSUEDEBINDTOOLBAR_H_
