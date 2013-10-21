
#ifndef _KGOBJECTEDITERPROJECTLISTDLG_H_
#define _KGOBJECTEDITERPROJECTLISTDLG_H_

#include "KGControlColumnDlg.h"
#include "KGPropertyListCtrl.h"

class KGClassButton : public CButton
{
public :
	KGClassButton::KGClassButton();
	virtual KGClassButton::~KGClassButton();
	int LoadIcon(HICON hIcon);

protected :
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	DECLARE_MESSAGE_MAP()

public :
	HICON   m_hButtonIcon;
	int     m_nStateFlag;
	int		m_nSelFlag;
};

class KGObjectEditerProjectListDlg : public CDialog
{
	DECLARE_DYNAMIC(KGObjectEditerProjectListDlg)

public :
	SHOW_MODEL GetShowMode();
	int	SetShowMode(SHOW_MODEL eShowMode);
	int BandData(
		ITabFile** ppTabFile_L, IIniFile** ppIniFile_L,
		ITabFile** ppTabFile_R, IIniFile** ppIniFile_R
	);
	int UpdateProperty_L(int nPos, int nIdFlag = false);
	int UpdateProperty_R(int nPos, int nIdFlag = false);

	int UpdateProperty(int nPos);

public :
	KGObjectEditerProjectListDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~KGObjectEditerProjectListDlg();
	enum { IDD = IDD_DIALOG_OBJECT_EDITER_LIST };
	BOOL OnInitDialog();

public :
	KG3DObjectEditor*  m_pParentWindow;
	KGPropertyListCtrl m_listPropertyView_L;
	KGPropertyListCtrl m_listPropertyView_R;
    KGPropertyListCtrl m_listPropertyView_A;
	CComboBox		   m_comboxCtrl_L;
	CComboBox		   m_comboxCtrl_R;
	CStatic			   m_staticCtrl_L;
	CStatic			   m_staticCtrl_R;
	CFont			   m_fontStatic;
	//KGClassButton      m_bnShowLogicList;
	//KGClassButton	   m_bnShowRepresent;
	SHOW_MODEL         m_eShowMode;

	CTabCtrl		   m_tabCtrl;

protected :
	virtual void DoDataExchange(CDataExchange* pDX);    
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg void OnSize(UINT nType, int cx, int cy);	
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg LRESULT OnListItemUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateScene(WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH  OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void   OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnColumnclickListProperty(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnBnClickedLogic();
public:
	afx_msg void OnBnClickedRepresent();
public:
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif //_KGOBJECTEDITERPROJECTLISTDLG_H_