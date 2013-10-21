////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGPropertySheet.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-9-12 13:02:49
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGPROPERTYSHEET_H_
#define _INCLUDE_KGPROPERTYSHEET_H_

#define TABCTRL_ID 100
#include "vector"
using namespace std;

class KGPropertySheet : public CWnd
{
	DECLARE_DYNAMIC(KGPropertySheet)

public :
    int AddPage(CPropertyPage* pPageWnd, DWORD dwTemplateID);
    int GetActiveIndex() const;
    CPropertyPage* GetActivePage() const;
    BOOL SetActivePage(int nPage);


public :
	KGPropertySheet();
	virtual ~KGPropertySheet();

public :
    CTabCtrl    m_tabCtrl;
    CFont       font;
    CImageList  m_imageList;
    int         m_nCurIndex;

    vector<CPropertyPage*>  m_vecPages;

protected :
	DECLARE_MESSAGE_MAP()
    virtual void PreSubclassWindow();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
    afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
};

#endif //_INCLUDE_KGPROPERTYSHEET_H_
