////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGBuildIniFileDlg.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-11-10 16:40:15
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KGBUILDINIFILE_H_
#define _KGBUILDINIFILE_H_ 

class KGBuildIniFileDlg : public CDialog 
{
	DECLARE_DYNAMIC(KGBuildIniFileDlg)
public:
	KGBuildIniFileDlg(CString* pszFileName, CWnd* pParent = NULL); 
	CString GetName();
	BOOL IsBuildedAFile();
	virtual ~KGBuildIniFileDlg();
	enum { IDD = IDD_MAKEINIFILE };

public:
	CString*   m_pszFileName;
	CString    m_szName;
	CComboBox  m_comboxType;
	int        m_nSelect;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};

#endif // _KGBUILDINIFILE_H_