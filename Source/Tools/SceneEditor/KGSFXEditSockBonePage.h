////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGSFXEditSockBonePage.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-9-29 14:31:25
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGSFXEDITSOCKBONEPAGE_H_
#define _INCLUDE_KGSFXEDITSOCKBONEPAGE_H_

#include "resource.h"
class KGSFXEditSockBonePage : public CPropertyPage
{
	DECLARE_DYNAMIC(KGSFXEditSockBonePage)

public :
	KGSFXEditSockBonePage();
	virtual ~KGSFXEditSockBonePage();
	enum { IDD = IDD_DIALOG_SOCK_BONE };

public :
    BOOL OnSetActive();
    BOOL OnKillActive();
    BOOL OnInitDialog();

    void FillSock();
    void FillBone();
    void FillComb();

public :
    CImageList  m_imageListTree;
    CTreeCtrl   m_sock;
    CTreeCtrl   m_bone;
    CComboBox   m_comb;

    BOOL        bModifyInit;

protected :
	virtual void DoDataExchange(CDataExchange* pDX);   
	DECLARE_MESSAGE_MAP()
    afx_msg void OnCbnSelchangeModel();
    afx_msg void OnTvnSelchangedBone(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnSelchangedSock(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedLink();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnNMDblclkBone(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkSock(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnCbnSetfocusModel();
};

#endif //_INCLUDE_KGSFXEDITSOCKBONEPAGE_H_
