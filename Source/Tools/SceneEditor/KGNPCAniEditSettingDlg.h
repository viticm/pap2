////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGNPCAniEditSettingDlg.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-11-14 13:17:57
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KGNPCANIEDITSETTINGDLG_H_
#define _KGNPCANIEDITSETTINGDLG_H_

#include "KGObjectPropertyEdit.h"

class KGProjectEditConfigSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(KGProjectEditConfigSettingDlg)

public:
	KGProjectEditConfigSettingDlg(PROPERTY_EDIT_TYPE eType, CWnd* pParent = NULL);   
	virtual ~KGProjectEditConfigSettingDlg();

	enum { IDD = IDD_NPCANI_SETTING };

public:
	CString m_szDirectory;
	CString m_szNamePlate;
	CString m_szTemplate;
	CString m_szFilePath;

	PROPERTY_EDIT_TYPE m_eEditType;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
public:
	CString GetFilePath();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedOk();
};

#endif // _KGNPCANIEDITSETTINGDLG_H_