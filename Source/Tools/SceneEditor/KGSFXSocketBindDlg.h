////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGSFXSocketBindDlg.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-9-8 15:45:47
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGSFXSOCKETBINDDLG_H_
#define _INCLUDE_KGSFXSOCKETBINDDLG_H_

#include "KGListCtrl.h"
#include "KGTreeCtrl.h"
#include "IEEditor.h"
#include "KGLilisPanel.h"

class KGSFXSocketBindDlg : public CDialog
{
	DECLARE_DYNAMIC(KGSFXSocketBindDlg)

public :
	KGSFXSocketBindDlg(CWnd* pParent = NULL);   
	virtual ~KGSFXSocketBindDlg();
	enum { IDD = IDD_DIALOG_SFX_SOCKET_BIND };

    int  Init();
    BOOL OnInitDialog();
    void FillSocket();
    void FillModels(HTREEITEM hTreeItem, LPCTSTR szPath);
    void Unbind();
    void Bind();

public :
    KGTreeCtrl m_treeSocket;
    KGTreeCtrl m_treeModels;

    CImageList m_imageListTree;

    KGLilisButton m_buttonUnBind;
    KGLilisButton m_buttonBind;
    CString       m_strBindInfo;

protected :
	virtual void DoDataExchange(CDataExchange* pDX);    
	DECLARE_MESSAGE_MAP()
    afx_msg void OnNMClickTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnSelchangedSocket(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnButtonClickUnbind();
    afx_msg void OnButtonClickBind();
};

#endif //_INCLUDE_KGSFXSOCKETBINDDLG_H_
