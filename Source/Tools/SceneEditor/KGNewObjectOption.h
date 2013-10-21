#pragma once
#include "KG3DObjectEditor.h"

class KGNewObjectOption : public CDialog
{
	DECLARE_DYNAMIC(KGNewObjectOption)

public :
	CString GetObjectName();
	int		IsNewRepresentId();
	int		GetSelRepresentId();

public :
	KGNewObjectOption(ITabFile* pTabFile, CWnd* pParent = NULL);   // standard constructor
	virtual ~KGNewObjectOption();
	virtual BOOL OnInitDialog();

	enum { IDD = IDD_DIALOG_BUILE_OBJECT_ALERT };

public :
	CComboBox m_comboxRepresentIdList;
	ITabFile* m_pTabFile; 
	CString   m_stringObjectName;
	int  	  m_nNewRepresentIdFlag;
	int  	  m_nOldRepresentIdFlag;
	int		  m_nOldRepresentId;


protected :
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedCheckNew();
	afx_msg void OnBnClickedCheckOld();
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

};
