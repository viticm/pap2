////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGInIFileEditDlg.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-7-13 19:46:08
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGINIFILEEDITDLG_H_
#define _INCLUDE_KGINIFILEEDITDLG_H_

#include "KGListCtrl.h"
#include "KGTreeCtrl.h"
#include "resource.h"

#define  ID_BN_SHOW   10
#define  ID_ED_SHOW   20
#define  ID_SP_SHOW   40
#define  ID_CB_SHOW	  50

class KGIniListCtrl : public KGListCtrl
{
public :
    int BandDate(vector<CString> vecStrIniProperty, CString strIniDateType);
    int UpdateProperty(int nIndex);
    int Save();
    int RetrievesItemData(LPKGLISTITEM pItem);
    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    int Build(IIniFile** ppIniFile);

public :
    KGIniListCtrl();
    virtual ~KGIniListCtrl();

public :
    struct _InIInfo
    {
        IIniFile*      pIniFile;
        CString        strPath;

        _InIInfo()
        {
            pIniFile = NULL;
            strPath  = _T("");
        }

        ~_InIInfo()
        {
            SAFE_RELEASE(pIniFile);
        }
    };

    CString             m_strIniDateType;
 
    vector<_InIInfo*>   m_vecIniProperty;
    IIniFile*           m_pIniDateType;

    CButton			    m_bnEditShow;
    CEdit			    m_editEdit;
    CSpinButtonCtrl     m_spinCtrl;
    CComboBox		    m_comboxCtrl;

    CString			    m_pszState[3];
    int                 m_nCurIndex;

    LPKGLISTITEM        m_lpCurItem;
    CFont               m_comboxFont;

public :
    afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClick();
    DECLARE_MESSAGE_MAP()

};

class KGInIFileEditDlg : public CDialog
{
	DECLARE_DYNAMIC(KGInIFileEditDlg)

public :
	KGInIFileEditDlg(
        vector<CString> vecStrIniProperty, CString strIniDateType, CWnd* pParent = NULL
    );   
	virtual ~KGInIFileEditDlg();
	enum { IDD = IDD_INI_EDIT };

    BOOL OnInitDialog();

public :
    vector<CString> m_vecStrIniProperty;
    CString         m_strIniDateType;

    KGIniListCtrl   m_list;
    KGTreeCtrl      m_tree;

    CImageList	    m_imageList;

protected :
	virtual void DoDataExchange(CDataExchange* pDX);   
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()
};

#endif //_INCLUDE_KGINIFILEEDITDLG_H_
