////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEdit4PairParamDlg.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-4-25 22:30:25
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGVALUEEDIT4PAIRPARAMDLG_H_
#define _INCLUDE_KGVALUEEDIT4PAIRPARAMDLG_H_

#include "KG3DObjectEditor.h"

class KGValueEdit4PairParamDlg : public CDialog
{
	DECLARE_DYNAMIC(KGValueEdit4PairParamDlg)

public :
	KGValueEdit4PairParamDlg(
		ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, 
		CWnd* pParent = NULL
	);
	virtual ~KGValueEdit4PairParamDlg();
	enum { IDD = IDD_DIALOG_4PAIR };

	virtual BOOL OnInitDialog();
	int UpdateParam(CString* pStringParam);

public :
	CString				m_stringParam_1;
	CString				m_stringParam_2;
	CString				m_stringParam_3;
	CString				m_stringParam_4;

	int					m_nParam_1;
	int					m_nParam_2;
	int					m_nParam_3;
	int					m_nParam_4;

	CButton				m_bnParam_1;
	CButton				m_bnParam_2;
	CButton				m_bnParam_3;
	CButton				m_bnParam_4;

	CSpinButtonCtrl		m_spinCtrl_1;
	CSpinButtonCtrl		m_spinCtrl_2;
	CSpinButtonCtrl		m_spinCtrl_3;
	CSpinButtonCtrl		m_spinCtrl_4;

	ITabFile*			m_pTabFile;
	IIniFile*			m_pIniFile;
	LPKGLISTITEM		m_pListItem;
	CWnd*				m_pParant;

protected :
	virtual void DoDataExchange(CDataExchange* pDX);   
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()
;
};

#endif //_INCLUDE_KGVALUEEDIT4PAIRPARAMDLG_H_
